/*
 * detection_param.c
 *
 *  Created on: 2023年1月29日
 *      Author: A21001
 */
#include "FreeRTOS.h"
#include <string.h>
#include "semphr.h"

#include "../../srv/log/log.h"

#include "xil_types.h"
#include "detection_param.h"

#include "../../hal/hal_ad9361/ad9361_config.h"
#include "../data_path/data_path_droneid.h"
#include "../rf_config/rf_config.h"

extern SemaphoreHandle_t DetectionSemphrHandleCore0;
extern SemaphoreHandle_t DetectionSemphrHandleCore1;

static SYS_DetectionParam_t DetectionParam = {
		DEFINED_SWEEP_FREQ_MODE,
		842,
		0,
		{0, 842},
		{400, 400, 6000, 40},
		{12, 40, 0, {842, 915, 2420, 2460, 5160, 5200, 5240, 5745, 5765, 5785, 5805, 5825, 0, 0}},
		{40, 34, 0}
};
//static SYS_DetectionParam_t DetectionParam = {
//		DEFINED_SWEEP_FREQ_MODE,
//		2422,
//		0,
//		{0, 2422},
//		{400, 2422, 6000, 40},
//		{4, 16, 0, {2422, 2452, 5766.5, 5806.5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
//		{40, 34, 0}
//};


static SYS_DetectionParam_t DronidDetectionParam = {
		DEFINED_SWEEP_FREQ_MODE,
		2422,
		0,
		{0, 2422},
		{400, 2422, 6000, 40},
		{4, 40, 0, {2422, 2452, 5766.5, 5806.5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
		{40, 34, 0}
};


static SYS_DetectionParam_t SpectrumDetectionParam = {
		DEFINED_SWEEP_FREQ_MODE,
		842,
		0,
		{0, 842},
		{400, 400, 6000, 40},
		{30, 40, 0, {842,915,1080,1120,1160,1200,1240,1280,1320,1360,1437,2422,2437,2462,5160,5180,5200,5220,5240,5655,5695,5725,5745,5765,5785,5805,5825,5850,5890,5930}},
		{40, 34, 0}
};

/* 0: alg not running, 1: detect alg is hitting */
static uint32_t DetecionAlgStausCore0 = 0;
static uint32_t DetecionAlgStausCore1 = 0;

static uint32_t work_mode = DRONEID_MODE ;

void SYS_SetWorkMode( SYS_WorkMode_t mode )
{
	work_mode = mode ;
}

SYS_WorkMode_t SYS_GetWorkMode()
{
	return work_mode ;
}

void SYS_SwitchWorkMode( SYS_WorkMode_t mode )
{

	if( mode > SPECTRUM_DX )
	{
		return;
	}
	if( SYS_GetWorkMode() == mode )
	{
		return;
	}

	SYS_SetWorkMode(mode);
	SYS_RefreshDetectionParam(mode);

	if( mode == DRONEID_MODE )
	{
//		RfConfig_droneid_Init();
//		RfConfig_Init();
		xTimerStart( droneid_timer , 10 );
	}
	else
	{
//		RfConfig_Init();
		xTimerStop( droneid_timer , 10 );
	}

}

void SYS_SetDetectionMode(SYS_DetectionMode_t SetDetecionMode)
{
	DetectionParam.DetectionMode = SetDetecionMode;
}

SYS_DetectionMode_t SYS_GetDetectionMode(void)
{
	return DetectionParam.DetectionMode;
}

uint8_t SYS_GetDetectFreqItem(void)
{
	return DetectionParam.CenterFreqItem;
}
uint32_t SYS_GetDetectFreq(void)
{
	return DetectionParam.CenterFreq;
}
/* DetRunCoreID: 0 Core0, 1 Core1 */
void SYS_SetDetectionAlgStatus(uint32_t DetRunCoreID, uint32_t SetDetecionAlgStaus)
{
	SemaphoreHandle_t *pLocalSemphrHandle = NULL;

	if (DetRunCoreID == 0)
	{
		DetecionAlgStausCore0 = SetDetecionAlgStaus;
		pLocalSemphrHandle = &DetectionSemphrHandleCore0;
	}
	else if(DetRunCoreID == 1)
	{
		DetecionAlgStausCore1 = SetDetecionAlgStaus;
		pLocalSemphrHandle = &DetectionSemphrHandleCore1;
	}
	else
	{
		while(1);
	}

	if (SetDetecionAlgStaus == 0)
	{
		while( xSemaphoreGive(*pLocalSemphrHandle) != pdTRUE)
		{
			vTaskDelay(5);
//			LOG_INFO("DetectionSemphrHandle error!\r\n");
		}
	}
}

uint32_t SYS_GetDetectionAlgStatus(uint32_t DetRunCoreID)
{
	uint32_t LocalDetecionAlgStaus = 0;

	if (DetRunCoreID == 0)
	{
		LocalDetecionAlgStaus = DetecionAlgStausCore0;
	}
	else if(DetRunCoreID == 1)
	{
		LocalDetecionAlgStaus = DetecionAlgStausCore1;
	}
	else
	{
		while(1);
	}

	return LocalDetecionAlgStaus;
}

int32_t SYS_SetDetectionParam(const SYS_DetectionParam_t *pDetParam)
{
	int32_t Result = 0;

	if (pDetParam == NULL)
	{
		Result = 1;
	}
	else
	{
		memcpy(&DetectionParam, pDetParam, sizeof(SYS_DetectionParam_t));
	}

	return Result;
}

int32_t SYS_GetDetectionParam(SYS_DetectionParam_t *pDetParam)
{
	int32_t Result = 0;

	if (pDetParam == NULL)
	{
		Result = 1;
	}
	else
	{
		memcpy(pDetParam, &DetectionParam, sizeof(SYS_DetectionParam_t));
	}

	return Result;
}

int32_t SYS_SetDefinedFreqParam(const SYS_DefinedFreqParam_t *pDefinedFreqParam)
{
	int32_t Result = 0;

	if (pDefinedFreqParam == NULL)
	{
		Result = 1;
	}
	else
	{
		memcpy(&DetectionParam.DefinedFreqParam, pDefinedFreqParam, sizeof(SYS_DefinedFreqParam_t));
	}

	return Result;
}

int32_t SYS_SetDefinedFreqParamByItem(uint8_t FreqItem)
{
	int32_t Result = 0;

	if (FreqItem > Freq_MAX_CNT - 1)
	{
		FreqItem = 0;
	}
	else
	{

	}

	DetectionParam.DefinedFreqParam.DefinedFreqIndex = FreqItem;
	DetectionParam.DefinedFreqParam.DefinedFreq = SYS_GetCenterFreq(FreqItem);

	return Result;
}

int32_t SYS_GetDefinedFreqParam(SYS_DefinedFreqParam_t *pDefinedFreqParam)
{
	int32_t Result = 0;

	if (pDefinedFreqParam == NULL)
	{
		Result = 1;
	}
	else
	{
		memcpy(pDefinedFreqParam, &DetectionParam.DefinedFreqParam, sizeof(SYS_DefinedFreqParam_t));
	}

	return Result;
}

int32_t SYS_SetAutoSweepParam(const SYS_AutoSweepParam_t *pAutoSweepParam)
{
	int32_t Result = 0;

	if (pAutoSweepParam == NULL)
	{
		Result = 1;
	}
	else
	{
		memcpy(&DetectionParam.AutoSweepParam, pAutoSweepParam, sizeof(SYS_AutoSweepParam_t));;
	}

	return Result;
}

int32_t SYS_GetAutoSweepParam(SYS_AutoSweepParam_t *pAutoSweepParam)
{
	int32_t Result = 0;

	if (pAutoSweepParam == NULL)
	{
		Result = 1;
	}
	else
	{
		memcpy(pAutoSweepParam, &DetectionParam.AutoSweepParam, sizeof(SYS_AutoSweepParam_t));
	}

	return Result;
}

int32_t SYS_SetDefinedSweepParam(const SYS_DefinedSweepParam_t *pDefinedSweepParam)
{
	int32_t Result = 0;

	if (pDefinedSweepParam == NULL)
	{
		Result = 1;
	}
	else
	{
		memcpy(&DetectionParam.DefinedSweepParam, pDefinedSweepParam, sizeof(SYS_DefinedSweepParam_t));
	}

	return Result;
}

int32_t SYS_GetDefinedSweepParam(SYS_DefinedSweepParam_t *pDefinedSweepParam)
{
	int32_t Result = 0;

	if (pDefinedSweepParam == NULL)
	{
		Result = 1;
	}
	else
	{
		memcpy(pDefinedSweepParam, &DetectionParam.DefinedSweepParam, sizeof(SYS_DefinedSweepParam_t));
	}

	return Result;
}

float SYS_GetCenterFreq(uint32_t CurFreqIndex)
{
	return DetectionParam.DefinedSweepParam.DefinedFreq[CurFreqIndex];
}


uint8_t SYS_GetDirFreqItem(float UavFreq)
{
	uint8_t DirFreqItem;
	float DirFreq_up;
	float DirFreq_down;

	if( UavFreq < DetectionParam.DefinedSweepParam.DefinedFreq[0] )
	{
		DirFreqItem = 0;
	}
	else if(UavFreq > DetectionParam.DefinedSweepParam.DefinedFreq[DetectionParam.DefinedSweepParam.DefinedFreqValideNo-1])
	{
		DirFreqItem = DetectionParam.DefinedSweepParam.DefinedFreqValideNo-1;
	}
	else
	{
		for( uint32_t i = 0 ; i < DetectionParam.DefinedSweepParam.DefinedFreqValideNo ; i++ )
		{
			DirFreq_down = DetectionParam.DefinedSweepParam.DefinedFreq[i];
			DirFreq_up = DetectionParam.DefinedSweepParam.DefinedFreq[i+1];
			if( ( UavFreq >= DirFreq_down ) && ( UavFreq <= DirFreq_up ) )
			{
				if( ((DirFreq_up-DirFreq_down)/2) > UavFreq )
				{
					DirFreqItem = (uint8_t)i;
				}
				else
				{
					DirFreqItem = (uint8_t)(i+1);
				}
				break;
			}
		}
	}

	return DirFreqItem;
}

void SYS_RefreshDetectionParam()
{
#if( WORKMODE == DRONEID)
	memcpy( &DetectionParam , &DronidDetectionParam ,sizeof(DetectionParam) );
#else
	memcpy( &DetectionParam , &SpectrumDetectionParam ,sizeof(DetectionParam) );
#endif

}
