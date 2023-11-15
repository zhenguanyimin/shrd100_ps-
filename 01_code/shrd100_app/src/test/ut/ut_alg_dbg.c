

/* FreeRTOS includes. */
#include "ut_alg_dbg.h"

#include "FreeRTOS.h"
#include "task.h"

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* FreeRTOS+CLI includes. */
#include "../../srv/cli/cli_if.h"
#include "../../app/alg/droneID/droneID_utils.h"
#include "ut_alg_dbg.h"


static BaseType_t alg_debug_printf_en_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	s32 printfEn = -1;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if ((len1 > 0))
	{
		printfEn = strtoul(param1, NULL, 0);

		if (printfEn >= 0)
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "OK");
		}
		else
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
		}
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return pdFALSE;
}

static BaseType_t alg_identify_tc_match_en_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	s32 tcMatchEn = -1;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if ((len1 > 0))
	{
		tcMatchEn = strtoul(param1, NULL, 0);

		if (tcMatchEn >= 0)
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "OK");
		}
		else
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
		}
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return pdFALSE;
}

static BaseType_t alg_param_threshold_mag_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	s32 threshold_mag = -1;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if ((len1 > 0))
	{
		threshold_mag = strtoul(param1, NULL, 0);

		if (threshold_mag >= 0)
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "OK");
		}
		else
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
		}
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return pdFALSE;
}

static BaseType_t alg_param_threshold_time_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	s32 threshold_time = -1;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if ((len1 > 0))
	{
		threshold_time = strtoul(param1, NULL, 0);

		if (threshold_time >= 0)
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "OK");
		}
		else
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
		}
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return pdFALSE;
}

static BaseType_t alg_param_threshold_freq_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	s32 threshold_freq = -1;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if ((len1 > 0))
	{
		threshold_freq = strtoul(param1, NULL, 0);

		if (threshold_freq >= 0)
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "OK");
		}
		else
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
		}
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return pdFALSE;
}

static BaseType_t alg_identify_UAV_cnt_th_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	s32 threshold_cnt = -1;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if ((len1 > 0))
	{
		threshold_cnt = strtoul(param1, NULL, 0);

		if (threshold_cnt >= 0)
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "OK");
		}
		else
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
		}
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return pdFALSE;
}

static BaseType_t alg_slide_win_len_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	s32 slide_win_len = -1;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if ((len1 > 0))
	{
		slide_win_len = strtoul(param1, NULL, 0);

		if (slide_win_len >= 0)
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "OK");
		}
		else
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
		}
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return pdFALSE;
}

static BaseType_t alg_warn_cnt_in_win_th_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	s32 warn_cnt_in_win_th = -1;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if ((len1 > 0))
	{
		warn_cnt_in_win_th = strtoul(param1, NULL, 0);

		if (warn_cnt_in_win_th >= 0)
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "OK");
		}
		else
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
		}
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return pdFALSE;
}

static BaseType_t AlgDebugPrintMode_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	s32 printMode = -1;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if ((len1 > 0))
	{
		printMode = strtoul(param1, NULL, 0);

		if ((printMode >= 0) && (printMode < ALG_PRINT_MODE_NUM))
		{
			SetAlgDebugPrintMode(printMode);
			snprintf(pcWriteBuffer, xWriteBufferLen, "OK, set algorithm debug print mode: %d", printMode);
		}
		else
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR, set algorithm debug print mode: %d, exceed!", printMode);
		}
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return pdFALSE ;
}

static BaseType_t DroneIDRemoveThreSet_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	s32 threValue = -1;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if ((len1 > 0))
	{
		threValue = strtoul(param1, NULL, 0);

		if (threValue >= 0)
		{
			SetDroneIDRemoveThre(threValue);
			snprintf(pcWriteBuffer, xWriteBufferLen, "OK, set droneID remove threshold value: %d", threValue);
		}
		else
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR, set droneID remove threshold value: %d", threValue);
		}
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return pdFALSE ;
}

static BaseType_t DroneIDRemoveThreLostDurSet_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	s32 threValue = -1;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if ((len1 > 0))
	{
		threValue = strtoul(param1, NULL, 0);

		if (threValue >= 0)
		{
			SetDroneIDRemoveThreLostDur(threValue);
			snprintf(pcWriteBuffer, xWriteBufferLen, "OK, set droneID remove threshold value, lost duration(ms): %d", threValue);
		}
		else
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR, set droneID remove threshold value: %d", threValue);
		}
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return pdFALSE ;
}

static BaseType_t DroneIDOutputSnModeSet_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	s32 snMode = -1;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if ((len1 > 0))
	{
		snMode = strtoul(param1, NULL, 0);

		if ((snMode >= 0) && (snMode < DRONEID_OUTPUT_SN_MODE_NUM))
		{
			SetDroneIDOutputSnMode(snMode);
			snprintf(pcWriteBuffer, xWriteBufferLen, "OK, set droneID output sn mode: %d", snMode);
		}
		else
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR, set droneID output sn mode: %d, exceed!", snMode);
		}
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return pdFALSE ;
}

static BaseType_t DroneIDOutputListModeSet_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	s32 listMode = -1;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if ((len1 > 0))
	{
		listMode = strtoul(param1, NULL, 0);

		if ((listMode >= 0) && (listMode < DRONEID_OUTPUT_LIST_MODE_NUM))
		{
			SetDroneIDOutputListMode(listMode);
			snprintf(pcWriteBuffer, xWriteBufferLen, "OK, set droneID output list mode: %d", listMode);
		}
		else
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR, set droneID output list mode: %d, exceed!", listMode);
		}
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return pdFALSE ;
}

static BaseType_t DroneIDTurboDecIterNumSet_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	s32 iterNum = -1;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if ((len1 > 0))
	{
		iterNum = strtoul(param1, NULL, 0);

		if ((iterNum > 0) && (iterNum < 10))
		{
			SetDroneIDTurboDecIterNum(iterNum);
			snprintf(pcWriteBuffer, xWriteBufferLen, "OK, set droneID turbo decode iteration num : %d", iterNum);
		}
		else
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR, set turbo decode iteration num: %d, exceed!", iterNum);
		}
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return pdFALSE ;
}

static BaseType_t DroneIDCrcErrCntReset_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	s32 cnt = -1;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if (len1 > 0)
	{
		cnt = strtoul(param1, NULL, 0);
		if (cnt == 0)
		{
			ResetDroneIDCrcErrorCnt();
			ResetDroneIDProcFrameCnt();
			snprintf(pcWriteBuffer, xWriteBufferLen, "OK, reset crc check error and frame count to: %d", cnt);
		}
		else
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR, cnt: %d, not valid", cnt);
		}
	}
	else
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");

	return pdFALSE;
}


static const CLI_Command_Definition_t alg_debug_printf_en =
{
	"AlgDebugPrintfEn",
	"\r\n AlgDebugPrintfEn <enableSwitch>:\r\n enable algorithm debug printf(0:not, 1:yes)\r\n",
	alg_debug_printf_en_handler,
	1
};

static const CLI_Command_Definition_t alg_identify_tc_match_en =
{
	"AlgIdentifyTcMatchEn",
	"\r\n AlgIdentifyTcMatchEn <enableSwitch>:\r\n enable algorithm identify tuchuan match(0:not, 1:yes)\r\n",
	alg_identify_tc_match_en_handler,
	1}
;

static const CLI_Command_Definition_t alg_param_threshold_mag =
{
	"AlgParamThresholdMagSet",
	"\r\n AlgParamThresholdMagSet <thresholdMag>:\r\n set algorithm parameter threshold mag(unit=0.001)\r\n",
	alg_param_threshold_mag_handler,
	1
};

static const CLI_Command_Definition_t alg_param_threshold_time =
{
	"AlgParamThresholdTimeSet",
	"\r\n AlgParamThresholdTimeSet <thresholdTime>:\r\n set algorithm parameter threshold time(unit=1)\r\n",
	alg_param_threshold_time_handler,
	1
};

static const CLI_Command_Definition_t alg_param_threshold_freq =
{
	"AlgParamThresholdFreqSet",
	"\r\n AlgParamThresholdFreqSet <thresholdFreq>:\r\n set algorithm parameter threshold freq(unit=1)\r\n",
	alg_param_threshold_freq_handler,
	1
};

static const CLI_Command_Definition_t alg_identify_UAV_cnt_th =
{
	"AlgIdentifyUavCntThresholdSet",
	"\r\n AlgIdentifyUavCntThresholdSet <thresholdCnt>:\r\n set algorithm identify UAV cnt threshold\r\n",
	alg_identify_UAV_cnt_th_handler,
	1
};

static const CLI_Command_Definition_t alg_slide_win_len =
{
	"AlgSlideWinLenSet",
	"\r\n AlgSlideWinLenSet <slideWinLen>:\r\n set algorithm slide window length\r\n",
	alg_slide_win_len_handler,
	1
};

static const CLI_Command_Definition_t alg_warn_cnt_in_win_th =
{
	"AlgWarnCntInWinThresholdSet",
	"\r\n AlgWarnCntInWinThresholdSet <thresholdWarnCnt>:\r\n set algorithm identify UAV warn count in slide window\r\n",
	alg_warn_cnt_in_win_th_handler,
	1
};

static const CLI_Command_Definition_t AlgDebugPrintMode_cmd =
{
	"AlgDebugPrintMode",
	"\r\n AlgDebugPrintMode <enableSwitch>:\r\n set algorithm debug print mode\r\n",
	AlgDebugPrintMode_handler,
	1
};

static const CLI_Command_Definition_t DroneIDRemoveThreSet_cmd =
{
	"DroneIDRemoveThreSet",
	"\r\n DroneIDRemoveThreSet <value>:\r\n set droneID remove threshold value\r\n",
	DroneIDRemoveThreSet_handler,
	1
};

static const CLI_Command_Definition_t DroneIDRemoveThreLostDurSet_cmd =
{
	"DroneIDRemoveThreLostDurSet",
	"\r\n DroneIDRemoveThreLostDurSet <value>:\r\n set droneID remove threshold value, lost duration r\n",
	DroneIDRemoveThreLostDurSet_handler,
	1
};

static const CLI_Command_Definition_t DroneIDOutputSnModeSet_cmd =
{
	"DroneIDOutputSnModeSet",
	"\r\n DroneIDOutputSnModeSet <value>:\r\n set droneID output sn mode\r\n",
	DroneIDOutputSnModeSet_handler,
	1
};

static const CLI_Command_Definition_t DroneIDOutputListModeSet_cmd =
{
	"DroneIDOutputListModeSet",
	"\r\n DroneIDOutputListModeSet <value>:\r\n set droneID output list mode\r\n",
	DroneIDOutputListModeSet_handler,
	1
};

static const CLI_Command_Definition_t DroneIDTurboDecIterNumSet_cmd =
{
	"DroneIDTurboDecIterNumSet",
	"\r\n DroneIDTurboDecIterNumSet <value>:\r\n set droneID turbo decode iteration num \r\n",
	DroneIDTurboDecIterNumSet_handler,
	1
};

static const CLI_Command_Definition_t DroneIDCrcErrCntReset_cmd =
{
	"ResetDroneIDCrcErrCnt",
	"\r\n ResetDroneIDCrcErrCnt <value>:\r\n reset crc check error and frame count to 0 \r\n",
	DroneIDCrcErrCntReset_handler,
	1
};


void ut_alg_dbg_init(void)
{
	FreeRTOS_CLIRegisterCommand(&alg_debug_printf_en);
	FreeRTOS_CLIRegisterCommand(&alg_identify_tc_match_en);
	FreeRTOS_CLIRegisterCommand(&alg_param_threshold_mag);
	FreeRTOS_CLIRegisterCommand(&alg_param_threshold_time);
	FreeRTOS_CLIRegisterCommand(&alg_param_threshold_freq);
	FreeRTOS_CLIRegisterCommand(&alg_identify_UAV_cnt_th);
	FreeRTOS_CLIRegisterCommand(&alg_slide_win_len);
	FreeRTOS_CLIRegisterCommand(&alg_warn_cnt_in_win_th);
	FreeRTOS_CLIRegisterCommand(&AlgDebugPrintMode_cmd);
	FreeRTOS_CLIRegisterCommand(&DroneIDRemoveThreSet_cmd);
	FreeRTOS_CLIRegisterCommand(&DroneIDRemoveThreLostDurSet_cmd);
	FreeRTOS_CLIRegisterCommand(&DroneIDOutputSnModeSet_cmd);
	FreeRTOS_CLIRegisterCommand(&DroneIDOutputListModeSet_cmd);
	FreeRTOS_CLIRegisterCommand(&DroneIDTurboDecIterNumSet_cmd);
	FreeRTOS_CLIRegisterCommand(&DroneIDCrcErrCntReset_cmd);
}
