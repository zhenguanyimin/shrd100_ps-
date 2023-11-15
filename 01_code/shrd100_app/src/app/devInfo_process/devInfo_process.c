#include "devInfo_process.h"

#include "sleep.h"
#include "timers.h"
#include <math.h>
#include <xuartlite.h>

#include "../../drv/axiuart/sys_intr.h"
#include "../app_init.h"
#include "../../drv/JY901/JY901.h"
#include "../../srv/flash_nv/flash_nv.h"
#include "../../srv/heartbeat_packet/heartbeat_packet.h"
#include "../../cfg/shrd_config.h"
#include "../sys_status_data/detection_param.h"

static TimerHandle_t g_timer_check_uart_no_datat_handle = NULL;
static QueueHandle_t DevInfoProcessQueue;
static uint8_t g_5s_count_cyc_flag = 0;
//lsm303ah
SemaphoreHandle_t mag_cali_start_cmd_Sem = NULL;
SemaphoreHandle_t acc_cali_start_cmd_Sem = NULL;
SemaphoreHandle_t gyro_cali_start_cmd_Sem = NULL;
SemaphoreHandle_t imu_timer_Sem = NULL;

TimerHandle_t g_timer ;

imu_info_t imu;

char GpsDataStr[1024];

static uint32_t GyroFrameNo = 0;


uint8_t  GetGyroStatus(void)
{
	uint8_t GyroStatus = 0;
	static uint32_t LastGyroFrameNo = 0;

	if( LastGyroFrameNo < GyroFrameNo)
	{
		GyroStatus = 1;
		LastGyroFrameNo = GyroFrameNo;
	}

	return GyroStatus;
}

static int32_t DevInfoProcessTask_Pend(uint8_t *flag, uint32_t timeout)
{
	int32_t result = -1;

	if (DevInfoProcessQueue != NULL)
	{
		if (xQueueReceive(DevInfoProcessQueue, flag, timeout))
		{
			result = 0;
		}
	}

	return result;
}

void DevInfoProcessTask_Post(uint8_t flag)
{
	BaseType_t xHigherPriorityTaskWoken;
	if (DevInfoProcessQueue != NULL)
	{
		if (xQueueIsQueueFullFromISR(DevInfoProcessQueue) == 0)
		{
			xQueueSendFromISR(DevInfoProcessQueue, &flag, &xHigherPriorityTaskWoken);
		}
	}
}

void GetAngleInfo(float *roll, float *pitch, float *yaw)
{
	roll[0] = imu.roll ;
	pitch[0] = imu.pitch ;
	yaw[0] = imu.yaw ;
}

static uint8_t PrintGpsInfoFlag = 0;

void SetPrintGpsInfoFlag(uint8_t value)
{
	PrintGpsInfoFlag = value;
}

void ImuProc(uint8_t cRxedChar)
{
	static unsigned char ucRxBuffer[44];
	static unsigned char ucRxCnt = 0;
	uint8_t sum = 0x00;
	float l_roll , l_pitch , l_yaw ;

	ucRxBuffer[ucRxCnt++]=cRxedChar;
	if (ucRxBuffer[0]!=0x55)
	{
		ucRxCnt=0;
	}
	if (ucRxCnt>=11)
	{
		ucRxCnt=0;
		for( uint32_t j = 0x00 ; j < 10 ; j++)
		{
			sum += ucRxBuffer[j];
		}
		if( sum != ucRxBuffer[10] )
		{
			return;
		}
		switch(ucRxBuffer[1])
		{
		case 0x53:
		{
			memcpy(&g_stcAngle,&ucRxBuffer[2],8);

			//转换为角度
			l_roll = g_stcAngle.Angle[0];
			l_roll = l_roll / 32768 * 180 ;
			l_pitch = g_stcAngle.Angle[1];
			l_pitch = l_pitch / 32768 * 180 ;
			l_yaw = g_stcAngle.Angle[2];
			l_yaw = l_yaw / 32768 * 180 ;

//			//转换为枪坐标系
//			imu.roll = l_pitch ;
//			imu.pitch = l_roll ;
//			imu.yaw = l_yaw ;
//
//			if (imu.yaw <= 0)
//			{
//				imu.yaw = -imu.yaw;
//			}
//			else
//			{
//				imu.yaw = 360 - imu.yaw;
//			}

			//转换为TRACER坐标系
			imu.roll = l_pitch ;
			imu.pitch = 90-l_roll ;
			imu.yaw = 180-l_yaw ;

			UpdataGyroInfo( (int32_t)(imu.pitch * 100000 ) , (int32_t)(imu.yaw * 100000 ) );
			GyroFrameNo++;
			break;
		}
		default:
			break;
		}
	}
}

void imu_jy901_task()
{
	uint8_t flag;
	xTimerStart(g_timer_check_uart_no_datat_handle, 0);
	while(1)
	{
		if (DevInfoProcessTask_Pend(&flag, 1000) == 0)
		{
//			switch (flag)
//			{
//			case DEV_INFO_PRINTF_GPS:
//			{
//				PrintGpsInfo();
//				break;
//			}
//			case DEV_INFO_PRINTF_GPS_STR:
//			{
//				PrintGpsInfoStr();
//				break;
//			}
//			default:
//				break;
//			}
		}

//		if (GetUsartDataUpdateFlag() == UART_DATA_UPDATE_FLAG_RESET && (g_5s_count_cyc_flag == 1))
//		{
//			JY901_Setup_Intr_System( &xInterruptController );
//			g_5s_count_cyc_flag = 0;
//		}


	}

}

static void TimerUartNoDataCheckCallback(void)
{
	SetUartDataUpdateFlag(UART_DATA_UPDATE_FLAG_RESET);
	g_5s_count_cyc_flag = 1;
}

int32_t InitImuJy901Process_Task()
{
	int32_t ret = RET_OK;
	int32_t *pvTimerID = NULL;
	xTaskHandle xCreatedTask;
	int Status;

	g_timer_check_uart_no_datat_handle = xTimerCreate((const char *)"Timer",
								(TickType_t)3000,
								(UBaseType_t)pdTRUE,
								(void *)pvTimerID,
								(TimerCallbackFunction_t)TimerUartNoDataCheckCallback);
	if (g_timer_check_uart_no_datat_handle == NULL)
	{
		LOG_ERROR("create Timer error\r\n");
		ret = -1;
		return ret;
	}

	DevInfoProcessQueue = xQueueCreate( 5, sizeof(uint8_t));
	configASSERT( DevInfoProcessQueue );
	JY901_Setup_Intr_System( &xInterruptController );
	if (pdPASS != xTaskCreate( imu_jy901_task, "Imu901Tsk", TASK_STACK_SIZE_IMU, (void *)0, TASK_PRI_IMU, &xCreatedTask))
	{
		ret = RET_SYSTEM_ERR;
		goto out;
	}

	UNUSED(xCreatedTask);

out:
	return ret;
}

void init_device_info()
{
	int32_t ret = 0 ;
	size_t retLent;
	char sn_data[DEVICE_SN_CODE_LEN+1] = {0};
//	FlashNV_Del(FLASH_NV_ID_SN_BOARD);
//	ret = FlashNV_Set(FLASH_NV_ID_SN_BOARD, &sn_data, sizeof(sn_data));
	ret = FlashNV_Get(FLASH_NV_ID_SN_BOARD, sn_data, DEVICE_SN_CODE_LEN+1 , &retLent );
//	FlashNV_Del(FLASH_NV_ID_SN_BOARD);
	if (ret > 0)
	{
		set_device_sn_code(&sn_data[1]);
	}
	else
	{
	}

	SYS_SetWorkMode(WORKMODE);
	SYS_RefreshDetectionParam();
}
