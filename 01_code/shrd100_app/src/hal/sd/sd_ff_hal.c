/*
 * sd_ff_hal.c
 *
 *  Created on: 2022锟斤拷12锟斤拷27锟斤拷
 *      Author: A22745
 */
/***************************** Include Files *********************************/
#include "xtime_l.h"
#include "stdio.h"
#include "xsdps.h"		/* SD device driver */
#include "ff.h"
#include "integer.h"
#include "sd_ff_hal.h"
#include "../../srv/log/log.h"
/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/


/************************** Variable Definitions *****************************/

FATFS	fs;
BYTE work[FF_MAX_SS];
char *FR_Table[20] ={
		"FR_OK",
		"FR_DISK_ERR",
		"FR_INT_ERR",
		"FR_NOT_READY",
		"FR_NO_FILE",
		"FR_NO_PATH",
		"FR_INVALID_NAME",
		"FR_DENIED",
		"FR_EXIST",
		"FR_INVALID_OBJECT",
		"FR_WRITE_PROTECTED",
		"FR_INVALID_DRIVE",
		"FR_NOT_ENABLED",
		"FR_NO_FILESYSTEM",
		"FR_MKFS_ABORTED",
		"FR_TIMEOUT",
		"FR_LOCKED",
		"FR_NOT_ENOUGH_CORE",
		"FR_TOO_MANY_OPEN_FILES",
		"FR_INVALID_PARAMETER"
};
/************************** Function Prototypes ******************************/
FRESULT FsMount( TCHAR *DiskPath )
{
    FRESULT result;

	result = f_mount(&fs, DiskPath, 0);    /* Mount a logical drive */
	if (result != FR_OK)
	{
		LOG_ERROR("[%s:%d] sdff mount failed(%s)\r\n ", __FUNCTION__, __LINE__ , FR_Table[result] );
	}

	return result;
}
FRESULT FsUmount(TCHAR *DiskPath)
{
    FRESULT result;

	result = f_mount(NULL, DiskPath, 0);    /* Mount a logical drive */
	if (result != FR_OK)
	{
		LOG_ERROR("[%s:%d] sdff umount failed(%s)\r\n ", __FUNCTION__, __LINE__ , FR_Table[result] );
	}
	return result;
}

FRESULT FsReadFile( FIL *file , uint8_t* FsReadBuf , uint32_t ReadLen , uint32_t *bw )
{
    FRESULT result;

    /* 璇诲彇鏂囦欢 */
    result = f_read(file, FsReadBuf, ReadLen, bw);
    if (result != FR_OK)
    {
        LOG_ERROR("[%s:%d] sd file read failed(%s)\r\n ", __FUNCTION__, __LINE__ , FR_Table[result] );
    }
	return result;

}

FRESULT FsWriteFile( FIL *file , const uint8_t* FsWriteBuf , uint32_t WriteLen , uint32_t *bw )
{
    FRESULT result;
	XTime tick4,tick5 ;

	XTime_GetTime(&tick4);
	tick4 = tick4 * 1000000 / COUNTS_PER_SECOND ;

	result = f_write(file, FsWriteBuf, WriteLen, bw);
	if (result != FR_OK)
	{
		LOG_ERROR("[%s:%d] sd file write failed(%s)\r\n ", __FUNCTION__, __LINE__ , FR_Table[result] );
	}

	XTime_GetTime(&tick5);
	tick5 = tick5 * 1000000 / COUNTS_PER_SECOND ;

	result = f_sync(file);
	if (result != FR_OK)
	{
		LOG_ERROR("[%s:%d] sd file sync failed(%s)\r\n ", __FUNCTION__, __LINE__ , FR_Table[result] );
	}
	return result;
}

FRESULT FsUnlinkFile( TCHAR *DiskPath , char* file_name )
{
    FRESULT result;
    char path[32] = {0x00};

    sprintf(path, "%s%s", DiskPath , file_name );
	result = f_unlink(path);
    if (result != FR_OK)
    {
        LOG_ERROR("[%s:%d] %s unlink failed(%s)\r\n ", __FUNCTION__, __LINE__ , file_name , FR_Table[result] );
    }
	return result;
}

FRESULT FsSeekFile( FIL *file , FSIZE_t ofs )
{
    FRESULT result;
	result = f_lseek( file , 0x00 );
    if (result != FR_OK)
    {
        LOG_ERROR("[%s:%d] file seek failed(%s)\r\n ", __FUNCTION__, __LINE__ , FR_Table[result] );
    }
	return result;
}

FRESULT FsOpenFile( FIL *file ,  TCHAR *DiskPath , char* file_name , BYTE mode )
{
    FRESULT result;
    char path[35] = {0x00};

    sprintf(path, "%s%s", DiskPath , file_name );
    result = f_open(file, path, mode );
    if (result != FR_OK)
    {
        LOG_ERROR("[%s:%d] sd file open failed(%s)\r\n ", __FUNCTION__, __LINE__ , FR_Table[result] );
    }
	return result;
}

FRESULT FsRenameFile(TCHAR *DiskPath , char* old_name , char* new_name )
{
    FRESULT result;
    char old_name_path[35] = {0x00}, new_name_path[35] = {0x00};

    sprintf(old_name_path, "%s%s", DiskPath , old_name );
    sprintf(new_name_path, "%s%s", DiskPath , new_name );
    result = f_rename(old_name_path, new_name_path);
    if (result != FR_OK)
    {
        LOG_ERROR("[%s:%d] sd file f_rename failed(%s)\r\n ", __FUNCTION__, __LINE__ , FR_Table[result] );
    }
	return result;
}

FRESULT FsClossFile( FIL *file )
{
    FRESULT result;

    result = f_close(file);
    if (result != FR_OK)
    {
        LOG_ERROR("[%s:%d] sd file close failed(%s)\r\n ", __FUNCTION__, __LINE__ , FR_Table[result] );
    }
	return result;
}

FRESULT FsOpenDir( DIR *DirInf , char *DiskPath )
{
    FRESULT result;

	result = f_opendir(DirInf, DiskPath);
	if (result != FR_OK)
	{
		LOG_ERROR("[%s:%d] sd dir open failed(%s)\r\n ", __FUNCTION__, __LINE__ , FR_Table[result] );
	}
	return result;

}

FRESULT FsClossDir( DIR *DirInf )
{
    FRESULT result;

    result = f_closedir(DirInf ); /* 濡傛灉涓嶅甫鍙傛暟锛屽垯浠庡綋鍓嶇洰褰曞紑濮� */
    if (result != FR_OK)
    {
        LOG_ERROR("[%s:%d] sd dir closs failed(%s)\r\n ", __FUNCTION__, __LINE__ , FR_Table[result] );
    }
	return result;
}

FRESULT FsReadDir( DIR *DirInf, FILINFO *FileInf )
{
    FRESULT result;

    result = f_readdir(DirInf, FileInf);
    if (result != FR_OK)
    {
        LOG_ERROR("[%s:%d] sd dir read failed(%s)\r\n ", __FUNCTION__, __LINE__ , FR_Table[result] );
    }
	return result;
}


FRESULT FsCreateFs( TCHAR *DiskPath )
{
    FRESULT result;

    result = f_mkfs(DiskPath, FM_FAT32, 0, work, sizeof(work) );
    if (result != FR_OK)
    {
        LOG_ERROR("[%s:%d] sdff create failed(%s)\r\n ", __FUNCTION__, __LINE__ , FR_Table[result] );
    }
	return result;
}

