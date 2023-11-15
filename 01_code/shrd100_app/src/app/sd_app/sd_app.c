/*
 * sd_app.c
 *
 *  Created on: 2022年12月30日
 *      Author: A22745
 */


/***************************** Include Files *********************************/
#include "FreeRTOS.h"
#include "task.h"
#include <queue.h>
#include "ff.h"
#include "xsdps.h"
#include "xtime_l.h"
#include "sleep.h"

#include "xsdps.h"		/* SD device driver */
#include "stdio.h"
#include "xil_types.h"
#include "../../cfg/version.h"
#include "../../cfg/shrd_config.h"
#include "sd_app.h"
#include "../../srv/log/log.h"
#include "../../srv/cli/cli_if.h"
#include "../../hal/sd/sd_hal.h"
#include "../../drv/sd/sd_drv.h"
#include "../../srv/alink/alink_msg_proc.h"
#include "../../srv/alink/checksum.h"
#include "../../hal/sd/sd_ff_hal.h"
#include "sd_buf.h"


/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/************************** Variable Definitions *****************************/

static alink_msg_t mLogPacketMsg;

static FIL log_info_file;
static FIL log_data_file;
static FIL log_debug_file;

char *DiskPath = "0:/";  //  string pointer to the logical drive number
static char list_FileName[32] = "log_last_name_list.text"; // name of the log_info
static char InfoFileName[32] = "info__20230101000000_0001.txt"; // name of the log_info
static char DataFileName[32] = "data__20230101000000_0001.txt"; // name of the log_data
static char DebugFileName[32] = "debug_20230101000000_0001.txt"; // name of the log_debug

/* 0: file named by last filename, 1: file named by gps date and time */
static uint32_t FileNamedByDateTime = 0;

/***************** Macros (Inline Functions) Definitions *********************/

#define LOG_SAFETY_SPACE ( SD_MAX_SIZE_BYTE * 6 / 10 )

/************************** Function Prototypes ******************************/
static char SystemTime[20] = {0};

void SetC2SystemTime( char *buff )
{
	memcpy( SystemTime , buff , sizeof(SystemTime));
}

static uint8_t GetHeaderChecksum(alink_msg_head_t *header)
{
	uint8_t *pkt = (uint8_t *)header;
	uint8_t checksum = 0;
	uint8_t i = 0;

	for (i = 0; i < ALINK_CORE_HEADER_LEN; i++)
	{
		checksum += pkt[i];
	}

	return checksum;
}
uint32_t RequestSysTime()
{
	uint16_t length = 0;
	uint8_t cnt = 0;
	alink_msg_t mPacketMsg;
	uint16_t crc = 0;

	memset(mPacketMsg.buffer, 0, 1024);

	mPacketMsg.msg_head.magic = 0xFD;
    mPacketMsg.msg_head.len_lo = (length & 0x00FF);
	mPacketMsg.msg_head.len_hi = (length & 0xFF00) >> 8;
	mPacketMsg.msg_head.seq = 0;
    mPacketMsg.msg_head.destid = DEV_C2;
    mPacketMsg.msg_head.sourceid = DEV_DRONEID;
    mPacketMsg.msg_head.msgid = 0xE3;
    mPacketMsg.msg_head.ans = 1;

	mPacketMsg.msg_head.checksum = GetHeaderChecksum(&mPacketMsg.msg_head);
	crc = crc_calculate((const uint8_t *)((&mPacketMsg.msg_head.magic) + 1), length + 8);
	mPacketMsg.buffer[cnt++] = crc & 0xFF;
	mPacketMsg.buffer[cnt++] = (crc & 0xFF00) >> 8;

//	return ( sizeof(mPacketMsg.msg_head) + cnt );

	AlinkMsgSendRecv(0, &mPacketMsg, NULL, 0);

}
char *GetC2SystemTime()
{
	char *ret = NULL;
	if( strlen(SystemTime) == 0 )
	{
		RequestSysTime();
		return ret;
	}
	else if( strlen(SystemTime) == 14 )
	{
		return SystemTime;
	}
	else
	{
		return ret;
	}
}


uint32_t ViewRootDir(TCHAR *DiskPath , uint64_t* fsize )
{
    FRESULT result;
    uint32_t cnt = 0;
	DIR DirInf;			/* Pointer to directory object to create */
	FILINFO FileInf;


    result = FsOpenDir( &DirInf , DiskPath );
    if( result != FR_OK )
    {
    	return result ;
    }

    for (cnt = 0; ;cnt++)
    {
        result = FsReadDir(&DirInf, &FileInf);         /* 读取目录项，索引会自动下移 */
        if (result != FR_OK || FileInf.fname[0] == 0)
        {
            break;
        }

        if (FileInf.fname[0] == '.')
        {
            continue;
        }
        fsize[0] += FileInf.fsize;
    }

    FsClossDir(&DirInf);

    return cnt;
}

void LogSpaceMmc()
{
    FRESULT result;
	DIR DirInf;			/* Pointer to directory object to create */
	FILINFO FileInf;
	uint64_t fsize= 0x00;

	ViewRootDir( DiskPath , &fsize );

	if( fsize < LOG_SAFETY_SPACE )
	{
		return ;
	}

    result = FsOpenDir( &DirInf , DiskPath );

	while( fsize >= LOG_SAFETY_SPACE )
	{
		for (uint32_t i = 0; i < 3 ; )
		{
			result = FsReadDir(&DirInf, &FileInf);         /* 读取目录项，索引会自动下移 */
			if (result != FR_OK || FileInf.fname[0] == 0)
			{
				break;
			}

			if (FileInf.fname[0] == '.')
			{
				continue;
			}

			if( ( strcmp( FileInf.fname , list_FileName ) != 0x00 )
					&& ( strcmp( FileInf.fname , InfoFileName ) != 0x00 )
					&& ( strcmp( FileInf.fname , DataFileName ) != 0x00 )
					&& ( strcmp( FileInf.fname , DebugFileName ) != 0x00 ))
			{
				i++;
				fsize -=  FileInf.fsize;

				result = FsUnlinkFile( DiskPath , FileInf.fname );
			}
		}
	}
    FsClossDir(&DirInf);

}

void PacketLogListData( uint8_t cmd_source_id , uint8_t msgid )
{
    FRESULT result;
	DIR DirInf;			/* Pointer to directory object to create */
	FILINFO FileInf;
	uint16_t length = 0;
	uint64_t fsize ;
	alink_msg_t *response_msg = &mLogPacketMsg ;
	uint32_t index = 0x08 , name_len = 0x00;
	uint32_t file_cnt = 0x00 ,file_num = 0x00 ;
	uint32_t pkg_total_num = 0x01 , pkg_cur_num = 0x00 ;

	memset(response_msg->buffer, 0, RECV_MAX_BUFF);

	file_num = ViewRootDir( DiskPath , &fsize );
    if( file_num <= 4 )
    {
		response_msg->msg_head.magic = 0xFD;
		response_msg->msg_head.seq = 0;
		response_msg->msg_head.destid = cmd_source_id;
		response_msg->msg_head.sourceid = DEV_DRONEID;
		response_msg->msg_head.msgid = msgid;
		response_msg->msg_head.ans = 0;
		response_msg->msg_head.seq = (uint8_t)pkg_cur_num ;
		length = 0x08;
		response_msg->msg_head.len_lo = (length & 0x00FF);
		response_msg->msg_head.len_hi = (length & 0xFF00) >> 8;
		memcpy(&response_msg->buffer[0], &pkg_total_num, sizeof(uint32_t));
		memcpy(&response_msg->buffer[4], &pkg_cur_num , sizeof(uint32_t) );
		response_msg->buffer[index] = '\0';
		AlinkMsgSendRecv(0, response_msg, NULL, 0);
    	return;
    }
    else
    {
    	file_num -= 4 ;
    }

	pkg_total_num = ( file_num + 29 ) / 30 ;

	memset(response_msg->buffer, 0, RECV_MAX_BUFF);
    response_msg->msg_head.magic = 0xFD;
	response_msg->msg_head.seq = 0;
    response_msg->msg_head.destid = cmd_source_id;
    response_msg->msg_head.sourceid = DEV_DRONEID;
    response_msg->msg_head.msgid = msgid;
    response_msg->msg_head.ans = 0;

    FsOpenDir(&DirInf, DiskPath); /* 如果不带参数，则从当前目录开始 */

    while(file_cnt != file_num)
    {
		result = FsReadDir(&DirInf, &FileInf);         /* 读取目录项，索引会自动下移 */
        if (result != FR_OK || FileInf.fname[0] == 0)
        {
            break;
        }

        if( ( strcmp( FileInf.fname , list_FileName ) != 0x00 )
        		&& ( strcmp( FileInf.fname , InfoFileName ) != 0x00 )
				&& ( strcmp( FileInf.fname , DataFileName ) != 0x00 )
				&& ( strcmp( FileInf.fname , DebugFileName ) != 0x00 ))
        {

    		name_len = strlen(FileInf.fname);
    		memcpy(&response_msg->buffer[index], &name_len, sizeof(uint32_t));
    		index += 4 ;

    		memcpy(&response_msg->buffer[index], &FileInf.fsize , sizeof(uint32_t) );
    		index += 4 ;

    		memcpy(&response_msg->buffer[index], FileInf.fname , name_len );
    		index += name_len ;

    		file_cnt++;
			if( ( file_cnt%30 == 0x00 ) || ( file_cnt == file_num ) )
			{
				length = index ;
				response_msg->msg_head.seq = (uint8_t)pkg_cur_num ;
				response_msg->msg_head.len_lo = (length & 0x00FF);
				response_msg->msg_head.len_hi = (length & 0xFF00) >> 8;
				response_msg->buffer[index] = '\0';
				memcpy(&response_msg->buffer[0], &pkg_total_num, sizeof(uint32_t));
				memcpy(&response_msg->buffer[4], &pkg_cur_num , sizeof(uint32_t) );
				AlinkMsgSendRecv(0, response_msg, NULL, 0);
				index = 0x08 ;
				pkg_cur_num++;
				memset( response_msg->buffer , 0x00 , RECV_MAX_BUFF );
			}
        }

    }
    f_closedir(&DirInf);

}

void PacketLogData( uint32_t log_id , char *fname , uint8_t cmd_source_id , uint8_t msgid )
{
    FRESULT result;
    FIL file;
    uint32_t bw = 0x00 ;
	uint16_t length = 0;
	alink_msg_t *response_msg = &mLogPacketMsg ;
	uint32_t pkg_total_num = 0x01 , pkg_cur_num = 0x00 , data_len = 16*1024 ;
	XTime tick,tick1,tick2 ;

	memset(response_msg->buffer, 0, RECV_MAX_BUFF);
    response_msg->msg_head.magic = 0xFD;
	response_msg->msg_head.seq = 0;
    response_msg->msg_head.destid = cmd_source_id;
    response_msg->msg_head.sourceid = DEV_DRONEID;
    response_msg->msg_head.msgid = msgid;
    response_msg->msg_head.ans = 0;

	result = FsOpenFile(&file, DiskPath, fname , FA_OPEN_EXISTING | FA_READ );

    if ((result != FR_OK)||(file.obj.objsize == 0x00))
    {

		memcpy(&response_msg->buffer[0], &log_id, sizeof(uint32_t));
		memcpy(&response_msg->buffer[4], &pkg_total_num, sizeof(uint32_t));
		memcpy(&response_msg->buffer[8], &pkg_cur_num, sizeof(uint32_t));
		data_len = 0x00 ;
		memcpy(&response_msg->buffer[12], &data_len, sizeof(uint32_t));

		length = 16 ;
		response_msg->msg_head.seq = 0x00 ;
		response_msg->msg_head.len_lo = (length & 0x00FF);
		response_msg->msg_head.len_hi = (length & 0xFF00) >> 8;
		response_msg->buffer[bw+16] = '\0';
		AlinkMsgSendRecv(0, response_msg, NULL, 0);
    }
    else
    {
    	pkg_total_num = ( file.obj.objsize + (data_len-1) ) / data_len ;

    	for( pkg_cur_num = 0x00 ; pkg_cur_num < pkg_total_num ; pkg_cur_num ++ )
    	{
    		memcpy(&response_msg->buffer[0], &log_id, sizeof(uint32_t));
    		memcpy(&response_msg->buffer[4], &pkg_total_num, sizeof(uint32_t));
    		memcpy(&response_msg->buffer[8], &pkg_cur_num, sizeof(uint32_t));

    		result = FsReadFile( &file, &response_msg->buffer[16], data_len, &bw);

    		if( pkg_cur_num == ( pkg_total_num -1 ) )
    		{
        		for( uint32_t i = bw-1 ; i > 0 ; i-- )
        		{
            		if( ( response_msg->buffer[16+i] == '\r' ) || ( response_msg->buffer[16+i] == '\n' ) )
            		{
            			break;
            		}
            		response_msg->buffer[16+i] = '\0';
        		}
    		}

    		memcpy(&response_msg->buffer[12], &bw, sizeof(uint32_t));
    		length = 16 + bw ;
    		response_msg->msg_head.seq = (uint8_t)pkg_cur_num ;
    		response_msg->msg_head.len_lo = (length & 0x00FF);
    		response_msg->msg_head.len_hi = (length & 0xFF00) >> 8;
    		response_msg->buffer[16 + bw] = '\0';
    		XTime_GetTime(&tick1);
    		AlinkMsgSendRecv(0, response_msg, NULL, 0);
    		XTime_GetTime(&tick2);
    		tick = ( tick2 - tick1 ) * 1000000 / COUNTS_PER_SECOND ;
    		usleep(1000);
    		memset( response_msg->buffer , 0x00 , RECV_MAX_BUFF );
    	}
    }
    FsClossFile(&file);
}
void DeleteAllLogFile()
{
    FRESULT result;
    uint32_t cnt = 0;
	DIR DirInf;			/* Pointer to directory object to create */
	FILINFO FileInf;

    result = FsOpenDir( &DirInf , DiskPath );

    for (cnt = 0; ;cnt++)
    {
		result = FsReadDir(&DirInf, &FileInf);         /* 读取目录项，索引会自动下移 */
        if (result != FR_OK || FileInf.fname[0] == 0)
        {
            break;
        }

        if (FileInf.fname[0] == '.')
        {
            continue;
        }

        if( ( strcmp( FileInf.fname , list_FileName ) != 0x00 )
        		&& ( strcmp( FileInf.fname , InfoFileName ) != 0x00 )
				&& ( strcmp( FileInf.fname , DataFileName ) != 0x00 )
				&& ( strcmp( FileInf.fname , DebugFileName ) != 0x00 ))
        {
			result = FsUnlinkFile( DiskPath , FileInf.fname );
        }
    }

    FsClossDir(&DirInf);

    return ;
}

/*
*********************************************************************************************************
*    函 数 名: ReadFileData
*    功能说明: 读取文件armfly.txt前128个字符，并打印到串口
*    形    参：无
*    返 回 值: 无
*********************************************************************************************************
*/

void CreateLogFile()
{
    FRESULT result;
    FIL file;
    uint32_t bw  ;
    int  index = 0x00 ;
    char path[32] = {0x00} , gnss_date_array[15] = {0x00};
	uint64_t gnss_date = 0x00 ;
	char index_str[5] = {0x00} ;

	FsMount(DiskPath);

//	FsCreateFs( DiskPath );

    result = FsOpenFile(&file, DiskPath , list_FileName , FA_WRITE | FA_READ );
    if (result != FR_OK)
    {
        FsCreateFs( DiskPath );

        FsOpenFile(&file, DiskPath , list_FileName , FA_CREATE_ALWAYS | FA_WRITE );

    }
	itoa( gnss_date , gnss_date_array ,4);
    if( file.obj.objsize == 0x00)
    {
        sprintf(InfoFileName,	"info__20230505205959_0001.txt" );
        sprintf(DataFileName,	"data__20230505205959_0001.txt" );
        sprintf(DebugFileName,	"debug_20230505205959_0001.txt" );
    }
    else
    {
    	if( gnss_date != 0x00 )
    	{
            sprintf(InfoFileName,	"info__%s_0001.txt", gnss_date_array );
            sprintf(DataFileName,	"data__%s_0001.txt", gnss_date_array );
            sprintf(DebugFileName,	"debug_%s_0001.txt", gnss_date_array );
    	}
    	else
    	{
    		FsReadFile(&file, (uint8_t*)InfoFileName, 29, &bw);

    	    FsReadFile(&file, (uint8_t*)DataFileName, 29, &bw);

    	    FsReadFile(&file, (uint8_t*)DebugFileName, 29, &bw);

    		memcpy( gnss_date_array , &InfoFileName[6] , 14 );
    		memcpy( index_str , &InfoFileName[21] , 4 );
    		index = atoi(index_str)+1;
            sprintf(InfoFileName, "info__%s_%04d.txt", gnss_date_array , index );

    		memcpy( gnss_date_array , &DataFileName[6] , 14 );
    		memcpy( index_str , &DataFileName[21] , 0x4 );
    		index = atoi(index_str)+1;
            sprintf(DataFileName, "data__%s_%04d.txt", gnss_date_array , index );

    		memcpy( gnss_date_array , &DebugFileName[6] , 14 );
    		memcpy( index_str , &DebugFileName[21] , 0x4 );
    		index = atoi(index_str)+1;
            sprintf(DebugFileName, "debug_%s_%04d.txt", gnss_date_array , index );
    	}

    }
    FsSeekFile( &file , 0x00 );

    FsWriteFile(&file, (uint8_t*)InfoFileName, strlen(InfoFileName) , &bw );

    FsWriteFile(&file, (uint8_t*)DataFileName, strlen(InfoFileName) , &bw );

    FsWriteFile(&file, (uint8_t*)DebugFileName, strlen(InfoFileName) , &bw );

    FsClossFile(&file);

    FsOpenFile(&log_info_file, DiskPath , InfoFileName , FA_CREATE_ALWAYS | FA_WRITE | FA_READ );
    FsOpenFile(&log_data_file, DiskPath , DataFileName , FA_CREATE_ALWAYS | FA_WRITE | FA_READ );
    FsOpenFile(&log_debug_file, DiskPath , DebugFileName , FA_CREATE_ALWAYS | FA_WRITE | FA_READ );

}

static void RenameLogFileNameByDateTime()
{
    FRESULT result;
    FIL file;
    uint32_t bw;

	uint32_t data_string_len = 0;

	char *pDataTime = NULL;

	char NewInfoFileName[32] = {0x00}; // name of the log_info
	char NewDataFileName[32] = {0x00}; // name of the log_data
	char NewDebugFileName[32] = {0x00}; // name of the log_debug

	if (FileNamedByDateTime == 0)
	{
		pDataTime = GetC2SystemTime();

		if (pDataTime != NULL)
		{
			data_string_len = strlen(pDataTime);
		}
		else
		{

		}

		if( data_string_len != 0x00 )
    	{
    		FileNamedByDateTime = 1;

            sprintf(NewInfoFileName,	"info__%s_0001.txt", pDataTime );
            sprintf(NewDataFileName,	"data__%s_0001.txt", pDataTime );
            sprintf(NewDebugFileName,	"debug_%s_0001.txt", pDataTime );

            FsClossFile(&log_info_file);
            FsClossFile(&log_data_file);
            FsClossFile(&log_debug_file);

            FsRenameFile(DiskPath, InfoFileName, NewInfoFileName);
            FsRenameFile(DiskPath, DataFileName, NewDataFileName);
            FsRenameFile(DiskPath, DebugFileName, NewDebugFileName);

            memcpy(InfoFileName, NewInfoFileName, sizeof(NewInfoFileName));
            memcpy(DataFileName, NewDataFileName, sizeof(NewDataFileName));
            memcpy(DebugFileName, NewDebugFileName, sizeof(NewDebugFileName));

            FsOpenFile(&log_info_file, DiskPath, InfoFileName , FA_OPEN_APPEND | FA_WRITE | FA_READ );
            FsOpenFile(&log_data_file, DiskPath, DataFileName , FA_OPEN_APPEND | FA_WRITE | FA_READ );
            FsOpenFile(&log_debug_file, DiskPath , DebugFileName , FA_OPEN_APPEND | FA_WRITE | FA_READ );

            /* save the last file name */
            result = FsOpenFile(&file, DiskPath , list_FileName , FA_WRITE | FA_READ );
            if (result == FR_OK)
            {
				FsSeekFile( &file , 0x00 );

				FsWriteFile(&file, (uint8_t*)InfoFileName, strlen(InfoFileName) , &bw );

				FsWriteFile(&file, (uint8_t*)DataFileName, strlen(InfoFileName) , &bw );

				FsWriteFile(&file, (uint8_t*)DebugFileName, strlen(InfoFileName) , &bw );

				FsClossFile(&file);
            }
    	}
    	else
    	{
    		// do nothing
    	}
	}
	else
	{
		// do nothing
	}
}
/*
*********************************************************************************************************
*    函 数 名: CreateNewFile
*    功能说明: 在SD卡创建一个新文件，文件内容填写“<a href="http://www.armfly.com" target="_blank">www.armfly.com</a>”
*    形    参：无
*    返 回 值: 无
*********************************************************************************************************
*/
void SdProcess_Task(void *p_arg)
{

	while (1)
	{

		RenameLogFileNameByDateTime();

		vTaskDelay(100);

		CheckInfoBufSaveToSd(&log_info_file);
		CheckDataBufSaveToSd(&log_data_file);
		CheckDebugBufSaveToSd(&log_debug_file);

	}
}
int32_t InitSdProcess_Task(void)
{
	int32_t retVal = -1;

	CreateLogFile();

	LogSpaceMmc();

	if (xTaskCreate(SdProcess_Task, "SdProcess_Task", TASK_STACK_SIZE_SD_DATA_ROCESS, NULL, TASK_PRI_SDPROCESS, NULL) == 1)
	{
		retVal = 0;
	}

	return retVal;
}
