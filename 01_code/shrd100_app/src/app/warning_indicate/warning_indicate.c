/*
 * warning_indicate.c
 *
 *  Created on: 2023年2月3日
 *      Author: A21001
 */

#include <FreeRTOS.h>
#include <timers.h>
#include <task.h>
#include "../../cfg/shrd_config.h"
#include "../../app/sys_status_data/detection_target.h"
#include "../../app/key_handle/key_handle.h"
#include "../../hal/output/output.h"
#include "../../app/alg/droneID/droneID_system_param.h"
#include "../../app/alg/droneID/droneID_main.h"
#include "../../app/sys_status_data/detection_param.h"
#define IO_DEVICE_TIMER 50u
#define LeveMax 4

typedef struct SYS_WaringPeriod
{
	/* Unit: ms, must be n*100 */
	uint32_t WarningPeriodTime;
	/* Unit: ms, must be n*100 */
	uint32_t WarningDevieOnTime;
} SYS_WaringPeriod_t;

typedef struct SYS_DeviceWaringParam
{
	int WarningLeve;
	SYS_WaringPeriod_t WarningParam[4];
} SYS_DeviceWaringParam_t;

static SYS_DeviceWaringParam_t DeviceWarningParam ={
		0,
		{{200, 100}, {400, 200}, {800, 400}, {0, 0}}
};

static int CurStatusEscapedTime=0;
static TimerHandle_t IoDeviceTimerHandle = NULL;

void SetWarmingLeve()
{
	DeviceWarningParam.WarningLeve--;
	if( DeviceWarningParam.WarningLeve == 0 )
	{
		DeviceWarningParam.WarningLeve = LeveMax-1;
	}
}

static void WI_IoDeviceTimerCallback(void)
{

	uint32_t DroneIdNum = 0 ;

	if( SYS_GetWorkMode() == DRONEID_MODE)
	{
		DroneID_OutputInfoList_t *droneidList = {0};
		droneidList = DroneID_GetOutputListInfoPredict();
		DroneIdNum = droneidList->DroneIdNum;

	}
	else
	{
		DroneReportResult_t *LocalDroneReportInfo = {0};
		LocalDroneReportInfo = SYS_GetDroneInfo();
		DroneIdNum = LocalDroneReportInfo->DroneCnt;
	}

	if (DroneIdNum > 0)
	{

		CurStatusEscapedTime += IO_DEVICE_TIMER;

		DeviceWarningParam.WarningLeve = Get4SwitchKey();
		if (CurStatusEscapedTime <= DeviceWarningParam.WarningParam[DeviceWarningParam.WarningLeve].WarningDevieOnTime)
		{
			GPIO_OutputCtrl(EMIO_SYS_LED2, IO_LOW);
			GPIO_OutputCtrl(EMIO_VBRATOR_EN, IO_LOW);
			axi_write_data(PL_BUZZER_PARA, 125*200);
			axi_write_data(PL_BUZZER_EN, 0x01);
		}
		else if (CurStatusEscapedTime <= DeviceWarningParam.WarningParam[DeviceWarningParam.WarningLeve].WarningPeriodTime)
		{
			GPIO_OutputCtrl(EMIO_SYS_LED2, IO_HIGTH);
			GPIO_OutputCtrl(EMIO_VBRATOR_EN, IO_HIGTH);
			axi_write_data(PL_BUZZER_PARA, 0x00);
			axi_write_data(PL_BUZZER_EN, 0x00);
		}
		else
		{
			CurStatusEscapedTime = 0 ;
		}
	}
	else
	{
		CurStatusEscapedTime = 0;
		GPIO_OutputCtrl(EMIO_SYS_LED2, IO_LOW);
		GPIO_OutputCtrl(EMIO_VBRATOR_EN, IO_LOW);

		axi_write_data(PL_BUZZER_PARA, 0x00);
		axi_write_data(PL_BUZZER_EN, 0x00);
	}
}

int32_t WI_Init(void)
{
	int32_t Return = 0;
	int32_t *pvTimerID = (int32_t *)1;

	IoDeviceTimerHandle = xTimerCreate((const char *)"IoDeviceTimer",
								   (TickType_t)IO_DEVICE_TIMER,
								   (UBaseType_t)pdTRUE,
								   (void *)pvTimerID,
								   (TimerCallbackFunction_t)WI_IoDeviceTimerCallback);
	if (IoDeviceTimerHandle == NULL)
	{
		Return = 1;
	}
	else
	{
		if (xTimerStart(IoDeviceTimerHandle, 10) == pdPASS)
		{
			Return = 2;
		}
		else
		{

		}
	}

	return Return;
}
