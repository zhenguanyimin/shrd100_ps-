/*
 * pl_config.c
 *
 *  Created on: 2023骞�5鏈�8鏃�
 *      Author: A21001
 */
/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include <timers.h>
#include "semphr.h"
#include "queue.h"

#include "pl_config.h"
#include "../data_path/data_path.h"
#include "../../hal/hal.h"
#include "../../app/sys_status_data/sys_status.h"
#include "../../app/process_task/detect_interface.h"
/* waiting time for some time to AD9361 work well, 50ms */
#define RF_PL_CONFIG_TIMER 50

static TimerHandle_t TimerStartAd_Handle = NULL;

void RF_StopPLAlg(void)
{
	axi_write_data(PL_AD_START, 0);
}

void RF_StartPLAlgDirectly(void)
{

	ClearDetDmaBuf();
	axi_write_data(PL_AD_START, 1);
}

/* waiting for some time to AD9361 work well */
void RF_StartPLAlgByTimer(void)
{
	xTimerStart(TimerStartAd_Handle, 10);
}

static void TimerStartAd_Callback(void)
{
//	SYS_SystemStatusMode_t lSysStaus = SYS_IDLE_STATUS;
//	DET_OutsideInterface_t *pOutsideInterface = NULL;
//	pOutsideInterface = DET_GetOutsideInterface();

	ClearDetDmaBuf();
//	pOutsideInterface->pGetSysStatus(&lSysStaus);
//	if (lSysStaus == SYS_DETECTION_TARGET_STATUS)
	{
		axi_write_data(PL_AD_START, 1);
	}
}

int32_t Rf_Pl_Config_Init(void)
{
	int32_t retVal = 0;

	TimerStartAd_Handle = xTimerCreate((const char *)"Timer",
									 (TickType_t)RF_PL_CONFIG_TIMER,
									 (UBaseType_t)pdFALSE,
									 (void *)6,
									 (TimerCallbackFunction_t)TimerStartAd_Callback);


	if (TimerStartAd_Handle == NULL)
	{
		retVal = -1;
	}

	return retVal;
}
