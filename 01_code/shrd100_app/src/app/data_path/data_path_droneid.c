
#include <stdio.h>

#include "data_path_droneid.h"

#include "../../hal/hal.h"
#include "../../srv/heartbeat_packet/heartbeat_packet.h"
#include "../../srv/protocol/protocol_dbgdat.h"
#include "../alg/alg_init.h"
#include "../alg/droneID/droneID_main.h"
#include "../alg/droneID/droneID_utils.h"
#include "../rf_config/rf_config.h"
#include "../../app/key_handle/interrupt_handle.h"
#include "../sys_status_data/detection_param.h"
#include "../../drv/usb/xusb_cdc_acm_api.h"

static uint32_t BurstCheckEn = 0 ;
static uint32_t bufferAdc[VALID_ADC_LEN];
static uint64_t bufferAdc_64bit[VALID_ADC_LEN];
static DroneID_ParseAllInfo_t g_ParseAllInfo[MAX_NUM_DRONE_ID];

static eDATA_PATH_INTR_FLAG gDataPathFlag;

static QueueHandle_t xDataPathQueueDroneid = NULL;

static uint8_t bufAdcDroneid[ADC_MAX_LEN_DRONEID] = {0};

TimerHandle_t droneid_timer ;

void SetBurstCheckEn( uint32_t para)
{
	BurstCheckEn = para ;
}

uint32_t GetBurstCheckEn()
{
	return BurstCheckEn ;
}

uint8_t *GetAdcBufDroneid(void)
{
	return bufAdcDroneid;
}

void DataPath_Post(uint32_t bufaddr)
{
	if (xDataPathQueueDroneid != NULL)
	{
//		xQueueSend(xDataPathQueue, &bufaddr, 10);
		BaseType_t xHigherPriorityTaskWoken;
		if (xDataPathQueueDroneid != NULL)
		{
			if (xQueueIsQueueFullFromISR(xDataPathQueueDroneid) == 0)
			{
				xQueueSendFromISR(xDataPathQueueDroneid, &bufaddr, &xHigherPriorityTaskWoken);
			}
		}
	}
}

static int32_t DataPath_Pend(uint32_t *bufaddr, uint64_t timeout)
{
	int32_t result = -1;

	if (xDataPathQueueDroneid != NULL)
	{
		if (xQueueReceive(xDataPathQueueDroneid, (void *)bufaddr, timeout))
		{
			result = 0;
		}
	}

	return result;
}

void BurstDataCheck( uint64_t *BurstBuff )
{
	static uint32_t lBurstCnt = 0x00 ;
	uint64_t lBurstSum = 0x00 ;
	uint64_t lBurstLen = 0x00 ;

	if( BurstBuff[19980-1] > BurstBuff[0] )
	{
		lBurstLen =BurstBuff[19980-1] - BurstBuff[0];
	}
	else
	{
		lBurstLen =0xffffffffffffffff - BurstBuff[19980-1] + BurstBuff[0] + 1 ;
	}

	if( lBurstLen != 19960 )
	{
		LOG_ERROR("BurstLen = %d \r\n", lBurstLen );
		return;
	}

	lBurstCnt++;
	if( lBurstCnt != (BurstBuff[2]>>32) )
	{
		LOG_ERROR("lBurstCnt = %d \r\n", BurstBuff[2] );
		lBurstCnt = (BurstBuff[2]>>32);
		return;
	}


	for( uint32_t i = 0x00 ; i < 19960 ; i++ )
	{
		lBurstSum += BurstBuff[3+i];
	}
	if( lBurstSum != BurstBuff[19980-2] )
	{
		LOG_ERROR("BurstSum = 0x%x \r\n", lBurstSum );
		return;
	}

	return;

}
void DataPathDroneid_Task(void *pvParameters)
{
	XTime tCur1 = 0, tCur2 = 0, tEnd1 = 0, tEnd2 = 0;
	int32_t tUsed1 = 0, tUsed2 = 0;
	uint64_t sourceAddr = 0;
	uint32_t uDataFlag;
	int32_t numDroneID = 0;
	uint8_t *buf = GetAdcBufDroneid();
	uint8_t flag = 0;

	while (1)
	{
		if (DataPath_Pend(&uDataFlag, portMAX_DELAY) == 0)
		{
			eDATA_PATH_INTR_FLAG eDataFlag = (eDATA_PATH_INTR_FLAG)uDataFlag;
			gDataPathFlag = eDataFlag;

			switch (eDataFlag)
			{
				case eDATA_PATH_DDR_0_256M:
				case eDATA_PATH_DDR_256_512M:
				{
					if (eDataFlag == eDATA_PATH_DDR_0_256M)
					{
						sourceAddr = READ_DDR_0_256M_ADDR;
					}
					else
					{
						sourceAddr = READ_DDR_256_512M_ADDR;
					}
					if (GetAlgorithmRunFlag() == 1)
					{
//						LOG_PRINTF("sourceAddr:%X\r\n", sourceAddr);
//						numDroneID = algorithm_Run((uint64_t *)sourceAddr, READ_DDR_LEN, g_DroneID_ParseInfo_t);
//						LOG_PRINTF("numDroneID:%d\r\n", numDroneID);
					}

					if (GetAlgorithmRunFlag() == 2)
					{
						if (buf)
						{
							if ((sourceAddr == READ_DDR_0_256M_ADDR) && (flag == 0))
							{
								flag = 1;
								memcpy(buf, (uint64_t *)sourceAddr, ADC_MAX_LEN_DRONEID / 2);
							}
							else if ((sourceAddr == READ_DDR_256_512M_ADDR) && (flag == 1))
							{
								flag = 2;
								memcpy(buf + ADC_MAX_LEN_DRONEID / 2, (uint64_t *)sourceAddr, ADC_MAX_LEN_DRONEID / 2);
								LOG_DEBUG("copy finish!!!!!!!!! buf addr:%X\r\n", (uint32_t)&buf);
							}
						}
					}
					else
					{
						flag = 0;
					}

					break;
				}

				case eDATA_PATH_DDR_BURST:
				case eDATA_PATH_TIMER:
				{
					XTime_GetTime(&tCur1);
					GetFreqRegValue();
					sourceAddr = READ_DDR_BURST_ADDR;
					Xil_DCacheInvalidateRange(sourceAddr, VALID_ADC_LEN * 8);
					memcpy(bufferAdc_64bit, (uint64_t *)sourceAddr, VALID_ADC_LEN * 8);

					if (eDataFlag == eDATA_PATH_DDR_BURST)
					{
						if( GetBurstCheckEn() != 0x00 )
						{
							BurstDataCheck( bufferAdc_64bit );
						}
					}

					XTime_GetTime(&tEnd1);
					tUsed1 = (tEnd1 - tCur1) * 1000000 / (COUNTS_PER_SECOND);

					XTime_GetTime(&tCur2);
					SYS_SetWorkMode(DRONEID_MODE);
					numDroneID = algorithm_Run(eDataFlag, bufferAdc_64bit, VALID_ADC_LEN, g_ParseAllInfo);

					if ((GetAlgorithmRunFlag() == 2) && (eDataFlag == eDATA_PATH_DDR_BURST))
					{
						LOG_DEBUG("\n");
						Usb_Send_data_func((uint8_t*)"################", 16);
						Usb_Send_data_func((uint8_t*)bufferAdc_64bit, VALID_ADC_LEN*8);
						Usb_Send_data_func((uint8_t*)"&&&&&&&&&&&&&&&&", 16);
						LOG_DEBUG("\n");
					}

					XTime_GetTime(&tEnd2);
					tUsed2 = (tEnd2 - tCur2) * 1000000 / (COUNTS_PER_SECOND);

					if (PRINT_BURST_PROCESS_TIME_COST == GetAlgDebugPrintMode())
					{
						LOG_DEBUG("*** get burst data from ddr time: %dus,  alg run time: %dus\r\n", tUsed1, tUsed2);
					}
					break;
				}
				default:
					break;
			}
		}
	}
}

eDATA_PATH_INTR_FLAG GetDataPathIntrFlag()
{
	return gDataPathFlag;
}

int32_t InitDataPathDroneID_Task()
{
	int32_t retVal = -1;

	IntcInitFunction(INTC_DEVICE_ID);

	xDataPathQueueDroneid = xQueueCreate(1, sizeof(uint32_t));

	droneid_timer = xTimerCreate( "droneid_timer" , 650, pdTRUE, &Droneid_timer_intr_Handler , Droneid_timer_intr_Handler );

#if( WORKMODE == DRONEID)
	xTimerStart( droneid_timer , 10 );
#endif


	if (xTaskCreate(DataPathDroneid_Task, "DataPath_Task", TASK_STACK_SIZE_DATA_PATH, NULL, TASK_PRI_DATA_PATH, NULL) == 1)
	{
		retVal = 0;
	}

	return retVal;
}
