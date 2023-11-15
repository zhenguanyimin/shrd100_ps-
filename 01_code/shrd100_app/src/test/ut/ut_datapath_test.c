/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "ff.h"
#include "timers.h"

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../app/data_path/data_path.h"
/* FreeRTOS+CLI includes. */
#include "../../srv/cli/cli_if.h"
#include "ut_dataPath_test.h"
#include "../../app/sd_app/sd_app.h"
#include "../../app/data_path/data_path_droneid.h"
#include "../../app/process_task/detect_interface.h"
#include "../../app/sys_status_data/detection_param.h"
#include "../../app/process_task/orientation.h"
#include "../../hal/sd/sd_ff_hal.h"
#include "../../hal/hal.h"
#include "../../app/process_task/detect_process_task.h"
#include "../../app/switch_filter/switch_filter.h"

s32 datapath_output_en_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	s32 type = -1;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if (len1 > 0)
	{
		type = strtoul(param1, NULL, 0);
		if (1 == type)
		{
			data_path_start(DIR_DATA); // adc
			data_path_out_en(DIR_DATA);
			axi_write_data(0x01, 1);
		}
		else if (2 == type)
		{
			data_path_start(DET_DATA); // rdm
			data_path_out_en(DET_DATA);
		}
		else if (3==type)
		{
			data_trans_init(DIR_DATA);
		}

		if ((1 == type) || (2 == type))
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "OK");
		}
		else
			snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}
	else
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");

	return pdFALSE;
}
BaseType_t AdcDataOutput_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{

	BaseType_t len1 = 0;
	const char *name = 0;
	FIL file;
    uint32_t bw  ;
    uint8_t *buf = GetAdcBufDroneid();
    uint32_t len, index,loop;

	name = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	FsOpenFile( &file, DiskPath , name , FA_CREATE_ALWAYS | FA_WRITE | FA_READ );

	len = 16*1024;
	loop = ADC_MAX_LEN_DRONEID/len;
	for (uint16_t i = 0; i < loop; i++)
	{
		index = i * len;
		FsWriteFile(&file, buf + index, len , &bw );
	}
	FsClossFile(&file);

	return pdFALSE;
}

BaseType_t SetDetectionMode_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{

	uint32_t mode = 0 , DefinedFreqIndex = 0 ;
	SYS_DetectionParam_t LocalDetectionParam = {0};
	BaseType_t len1 = 0, len2 = 0;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
	const char *param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &len2);

	DET_OutsideInterface_t *pOutsideInterface = NULL;
	pOutsideInterface = DET_GetOutsideInterface();

	pOutsideInterface->pGetDetectionParam(&LocalDetectionParam);

	if (len1 <= 0)
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
		return pdFALSE;
	}

	mode = strtoul(param1, NULL, 0);
	if( mode >= AUTO_SWEEP_FREQ_MODE )
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
		return pdFALSE;
	}

	SYS_SetDetectionMode( mode );

	if( mode == DEFINED_FREQ_MODE )
	{

		DefinedFreqIndex = strtoul(param2, NULL, 0);
		if (len2 <= 0)
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
			return pdFALSE;
		}

		if( DefinedFreqIndex >= LocalDetectionParam.DefinedSweepParam.DefinedFreqValideNo )
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
			return pdFALSE;
		}
		SYS_SetDefinedFreqParamByItem( DefinedFreqIndex );
	}

	snprintf(pcWriteBuffer, xWriteBufferLen, "OK");

	return pdFALSE;
}
BaseType_t SetFrqrang_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{

	uint32_t mode = 0 ;
	BaseType_t len1 = 0;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if (len1 <= 0)
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
		return pdFALSE;
	}

	mode = strtoul(param1, NULL, 0);
#if RF_T3 == 1
	if( mode <= RF_5650M_5850M )
#else
	if( mode <= FRQ_RANG_5650M_6000M )
#endif
	{
		SYS_SetFrqRang( mode );
#if RF_T3 == 1
		RF_TEST(mode);
#endif
		snprintf(pcWriteBuffer, xWriteBufferLen, "OK");
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}


	return pdFALSE;
}
BaseType_t SetWorkMode_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{

	BaseType_t len1 = 0;
	SYS_WorkMode_t mode ;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if (len1 <= 0)
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
		return pdFALSE;
	}

	mode = (SYS_WorkMode_t)strtoul(param1, NULL, 0);

	if( mode <= SPECTRUM_DX )
	{

		SYS_SwitchWorkMode(mode);

		snprintf(pcWriteBuffer, xWriteBufferLen, "OK");
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}


	return pdFALSE;
}

BaseType_t SetDirMode_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{

	BaseType_t len1 = 0;
	SYS_WorkMode_t mode ;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if (len1 <= 0)
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
		return pdFALSE;
	}

	mode = (SYS_WorkMode_t)strtoul(param1, NULL, 0);

	if( mode <= 1 )
	{

		SetEligibleEntryOrientationFlag(mode);

		snprintf(pcWriteBuffer, xWriteBufferLen, "OK");
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}


	return pdFALSE;
}
static const CLI_Command_Definition_t datapath_out_enable =
	{
		"DatapathOutEn",
		"\r\nDatapathOutEn <type>:\r\n type:0 for disable ,1 for adc ,2 for rdmap\r\n",
		datapath_output_en_handler,
		1};

static const CLI_Command_Definition_t AdcDataOutput =
	{
		"AdcDataOutput",
		"\r\nAdcDataOutput <name>\r\n",
		AdcDataOutput_handler,
		1};

static const CLI_Command_Definition_t SetDetectionMode =
	{
		"SetDetectionMode",
		"\r\nSetDetectionMode <type>:\r\n 0:DEFINED_FREQ ,1:SWEEP_FREQ \r\n",
		SetDetectionMode_handler,
		-1};

static const CLI_Command_Definition_t SetFrqrang =
	{
		"SetFrqrang",
		"\r\nSetFrqrang <type>:\r\n 0:400M~6000M,1:2400M~2460M,2:2450M~2510M,3:5150M~5250M,4:5650M~5850M\, \r\n",
		SetFrqrang_handler,
		1};

static const CLI_Command_Definition_t SetWorkMode =
	{
		"SetWorkMode",
		"\r\nSetWorkMode <type>:\r\n 0:DEFINED ,1:FREQ_DETECT_1,2:FREQ_DETECT_2 \r\n",
		SetWorkMode_handler,
		1};

static const CLI_Command_Definition_t SetDirMode =
	{
		"SetDirMode",
		"\r\nSetDirMode <type>:\r\n 0:disable ,1:enable \r\n",
		SetDirMode_handler,
		1};

void ut_datapath_init(void)
{
	FreeRTOS_CLIRegisterCommand(&datapath_out_enable);
	FreeRTOS_CLIRegisterCommand(&AdcDataOutput);
	FreeRTOS_CLIRegisterCommand(&SetDetectionMode);
	FreeRTOS_CLIRegisterCommand(&SetFrqrang);
	FreeRTOS_CLIRegisterCommand(&SetWorkMode);
	FreeRTOS_CLIRegisterCommand(&SetDirMode);
}
