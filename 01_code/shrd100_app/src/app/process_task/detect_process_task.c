/*
 * detect_process_task.c
 *
 *  Created on: 2022-08-24
 *      Author: A19199
 */
/* FreeRTOS includes. */
#include "detect_process_task.h"
#include "FreeRTOS.h"
#include "task.h"
#include <queue.h>

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <timers.h>
#include <xil_printf.h>

#include "../../cfg/shrd_config.h"
#include "../../hal/hal.h"
#include "../../hal/hal_ad9361/ad9361_config.h"
#include "../../inc/common_define.h"
#include "../../srv/log/log.h"
#include "../alg/alg_init.h"
#include "../data_path/data_path.h"
#include "../rf_config/rf_config.h"
#include "../rf_config/pl_config.h"
#include "../sd_app/sd_app.h"
#include "../sys_status_data/detection_param.h"
#include "detect_interface.h"

#include "../../srv/heartbeat_packet/heartbeat_packet.h"
#include "../rpc_master_api/rpc_master_api.h"
#include "../devInfo_process/devInfo_process.h"
#include "orientation.h"
#include "../../hal/hal_ad9361/ad9361_spi.h"
#include "../../app/switch_filter/switch_filter.h"

#define SWEEP_TIME 1200 // ms
uint8_t Cfg_f_flag = 0;
uint32_t g_algRunCnt = 0;

static int warn = 0;
static int drnum = 0;
static QueueHandle_t xDetectProcessQueueCore0 = NULL;
static QueueHandle_t xDetectProcessQueueCore1 = NULL;
#if RF_T3 == 1
static int g_frq_rang = RF_400M_6000M ;
#else
static int g_frq_rang = FRQ_RANG_400M_6000M ;
#endif

void SYS_SetFrqRang( int frq_rang )
{
	g_frq_rang = frq_rang ;
}

int SYS_GetFrqRang()
{
	return g_frq_rang ;
}

static uint8_t LastDetFreqItem = 0xFF;
void SetLastDetFreqItem(uint8_t value)
{
	LastDetFreqItem = value;
}

uint8_t GetLastDetFreqItem(void)
{
	return LastDetFreqItem;
}

#if RF_T3 == 1
uint32_t Get_CurFreqIndex(SYS_DetectionParam_t LocalDetectionParam )
{
	float l_CurFreq = 0x00 ,l_Freq_Min = 0x00 ,l_Freq_Max = 0x00 ;

	int cnt = 0;

	if( SYS_GetFrqRang() == RF_2400M_2460M )
	{
		l_Freq_Min = 2400;
		l_Freq_Max = 2460;
	}
	else if( SYS_GetFrqRang() == RF_2450M_2510M )
	{
		l_Freq_Min = 2450;
		l_Freq_Max = 2510;
	}
	else if( SYS_GetFrqRang() == RF_5150M_5250M )
	{
		l_Freq_Min = 5150;
		l_Freq_Max = 5250;
	}
	else if( SYS_GetFrqRang() == RF_5650M_5850M )
	{
		l_Freq_Min = 5650;
		l_Freq_Max = 5850;
	}
	else
	{
		l_Freq_Min = 400;
		l_Freq_Max = 6000;
	}
	do
	{
		cnt++;
		if( cnt > sizeof(LocalDetectionParam.DefinedSweepParam.DefinedFreq)/sizeof(LocalDetectionParam.DefinedSweepParam.DefinedFreq[0]))
		{
			return 0 ;
		}
		if (LocalDetectionParam.DefinedSweepParam.CurFreqIndex < LocalDetectionParam.DefinedSweepParam.DefinedFreqValideNo - 1)
		{
			LocalDetectionParam.DefinedSweepParam.CurFreqIndex ++;
		}
		else
		{
			LocalDetectionParam.DefinedSweepParam.CurFreqIndex = 0;
		}

		l_CurFreq = LocalDetectionParam.DefinedSweepParam.DefinedFreq[LocalDetectionParam.DefinedSweepParam.CurFreqIndex];
	}while( (l_CurFreq < l_Freq_Min)||(l_CurFreq>l_Freq_Max) );

	return LocalDetectionParam.DefinedSweepParam.CurFreqIndex;
}
#else
uint32_t Get_CurFreqIndex(SYS_DetectionParam_t LocalDetectionParam )
{
	float l_CurFreq = 0x00 ,l_Freq_Min = 0x00 ,l_Freq_Max = 0x00 ;

	int cnt = 0;

	if( SYS_GetFrqRang() == FRQ_RANG_400M_1000M )
	{
		l_Freq_Min = 400;
		l_Freq_Max = 1000;
	}
	else if( SYS_GetFrqRang() == FRQ_RANG_1000M_2000M )
	{
		l_Freq_Min = 1000;
		l_Freq_Max = 2000;
	}
	else if( SYS_GetFrqRang() == FRQ_RANG_2400M_2483M )
	{
		l_Freq_Min = 2400;
		l_Freq_Max = 2483;
	}
	else if( SYS_GetFrqRang() == FRQ_RANG_5100M_5300M )
	{
		l_Freq_Min = 5100;
		l_Freq_Max = 5300;
	}
	else if( SYS_GetFrqRang() == FRQ_RANG_5650M_6000M )
	{
		l_Freq_Min = 5650;
		l_Freq_Max = 6000;
	}
	else
	{
		l_Freq_Min = 400;
		l_Freq_Max = 6000;
	}
	do
	{
		cnt++;
		if( cnt > sizeof(LocalDetectionParam.DefinedSweepParam.DefinedFreq)/sizeof(LocalDetectionParam.DefinedSweepParam.DefinedFreq[0]))
		{
			return 0 ;
		}
		if (LocalDetectionParam.DefinedSweepParam.CurFreqIndex < LocalDetectionParam.DefinedSweepParam.DefinedFreqValideNo - 1)
		{
			LocalDetectionParam.DefinedSweepParam.CurFreqIndex ++;
		}
		else
		{
			LocalDetectionParam.DefinedSweepParam.CurFreqIndex = 0;
		}

		l_CurFreq = LocalDetectionParam.DefinedSweepParam.DefinedFreq[LocalDetectionParam.DefinedSweepParam.CurFreqIndex];
	}while( (l_CurFreq < l_Freq_Min)||(l_CurFreq>l_Freq_Max) );

	return LocalDetectionParam.DefinedSweepParam.CurFreqIndex;
}
#endif
int32_t DET_CfgNextDetection(void)
{
	int32_t RunResult = 0;
	SYS_DetectionParam_t LocalDetectionParam = {0};

	DET_OutsideInterface_t *pOutsideInterface = NULL;
	pOutsideInterface = DET_GetOutsideInterface();

	pOutsideInterface->pGetDetectionParam(&LocalDetectionParam);

	switch (LocalDetectionParam.DetectionMode)
	{
		case AUTO_SWEEP_FREQ_MODE:
			break;
		case DEFINED_SWEEP_FREQ_MODE:


			LocalDetectionParam.DefinedSweepParam.CurFreqIndex = Get_CurFreqIndex(LocalDetectionParam);
			LocalDetectionParam.CenterFreq = LocalDetectionParam.DefinedSweepParam.DefinedFreq[LocalDetectionParam.DefinedSweepParam.CurFreqIndex];
			LocalDetectionParam.CenterFreqItem = LocalDetectionParam.DefinedSweepParam.CurFreqIndex;

			break;
		case DEFINED_FREQ_MODE:
			LocalDetectionParam.CenterFreq = LocalDetectionParam.DefinedFreqParam.DefinedFreq;
			LocalDetectionParam.CenterFreqItem = LocalDetectionParam.DefinedFreqParam.DefinedFreqIndex;
			break;
		default:
			RunResult = 1;
			break;
	}

	if (RunResult == 0)
	{
		RunResult = RF_ConfigParamByFreqId( LocalDetectionParam.CenterFreqItem );
		pOutsideInterface->pSetDetectionParam(&LocalDetectionParam);
	}

	return RunResult;
}

void DetectProcessTaskCore0_Post(uint32_t TotalFrameNo, float ***pSpecMat, float ***pUpMat)
{
	DET_DetectionDataAddr_t lDetectionDataAddr = {0, NULL, NULL};

	lDetectionDataAddr.FrameNo = TotalFrameNo;

	lDetectionDataAddr.pSpecMat = pSpecMat;
	lDetectionDataAddr.pUpMat = pUpMat;

	if (xDetectProcessQueueCore0 != NULL)
	{
		xQueueSend(xDetectProcessQueueCore0, &lDetectionDataAddr, 10);
	}
}

static int32_t DetectProcessTaskCore0_Pend(DET_DetectionDataAddr_t *pDetectionDataAddr, uint32_t timeout)
{
	int32_t result = -1;

	if (xDetectProcessQueueCore0 != NULL)
	{
		if (xQueueReceive(xDetectProcessQueueCore0, (void *)pDetectionDataAddr, timeout))
		{
			result = 0;
		}
	}

	return result;
}

void printResult2(struct droneResult *droneInfo, int nDrone)
{
	uint32_t uDroneCnt = 0;

	for (int i = 0; i < nDrone; i++)
	{
		LOG_PRINTF("----------------UAV-----------------\r\n");
		if (droneInfo[i].flag == 0)
		{
			// continue;
			LOG_PRINTF("drone:\r\n");
			uDroneCnt++;
		}
		else
		{
			LOG_PRINTF("remote:\r\n");
		}

		droneInfo[i].amp = droneInfo[i].amp;
		LOG_INFO("spectrum info \r\n"
				"ID=%d\r\n"
				"name=%s\r\n"
				"amp=%0.1f\r\n"
				"freq=%0.1f\r\n"
				"bandWidth=%0.1f\r\n"
				"range=%0.1f\r\n",
				droneInfo[i].ID,
				droneInfo[i].name,
				droneInfo[i].amp,
				droneInfo[i].freq[0],
				droneInfo[i].bw,
				droneInfo[i].range);

//		msg.uLogNum = eLOG_DETECT_OUTSOURCE_INFO;
//		msg.sDetectOutsourceInfo_msg.uTimestamp = (uint32_t)xTaskGetTickCount();
//		msg.sDetectOutsourceInfo_msg.flag = droneInfo[i].flag;
//		memset(msg.sDetectOutsourceInfo_msg.name, 0, 48);
//		memcpy(msg.sDetectOutsourceInfo_msg.name, droneInfo[i].name, 48);
//		msg.sDetectOutsourceInfo_msg.freq = droneInfo[i].freq[0];
//		msg.sDetectOutsourceInfo_msg.bw = droneInfo[i].bw * 1000;
//		LoggingTask_Post(&msg);
	}
}


void DetectProcessTaskCore1_Post(uint32_t TotalFrameNo, float ***pSpecMat, float ***pUpMat)
{
	DET_DetectionDataAddr_t lDetectionDataAddr = {0, NULL, NULL};

	lDetectionDataAddr.FrameNo = TotalFrameNo;

	lDetectionDataAddr.pSpecMat = pSpecMat;
	lDetectionDataAddr.pUpMat = pUpMat;

	if (xDetectProcessQueueCore1 != NULL)
	{
		xQueueSend(xDetectProcessQueueCore1, &lDetectionDataAddr, 10);
	}
}

static int32_t DetectProcessTaskCore1_Pend(DET_DetectionDataAddr_t *pDetectionDataAddr, uint32_t timeout)
{
	int32_t result = -1;

	if (xDetectProcessQueueCore1 != NULL)
	{
		if (xQueueReceive(xDetectProcessQueueCore1, (void *)pDetectionDataAddr, timeout))
		{
			result = 0;
		}
	}

	return result;
}

int32_t InitDetectProcess_Task()
{
	int32_t retVal = -1;


	xDetectProcessQueueCore0 = xQueueCreate(5, sizeof(DET_DetectionDataAddr_t));
	if (xDetectProcessQueueCore0 == NULL)
	{
		while(1);
	}

	xDetectProcessQueueCore1 = xQueueCreate(5, sizeof(DET_DetectionDataAddr_t));
	if (xDetectProcessQueueCore1 == NULL)
	{
		while(1);
	}

	return retVal;
}
