
/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include <timers.h>
#include "semphr.h"
#include "queue.h"

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xil_printf.h>

#include "netif/xadapter.h"
#include "../data_path/data_path.h"

//#include "../../srv/alink/xusb_cdc_acm_api.h"
#include "../../drv/usb/xusb_cdc_acm_api.h"
#include "../../hal/hal.h"
#include "../../hal/hal_ad9361/ad9361_config.h"
#include "../../srv/cli/cli_if.h"
#include "../../srv/heartbeat_packet/heartbeat_packet.h"
#include "../../srv/protocol/protocol_dbgdat.h"
#include "../../app/process_task/orientation.h"
#include "../rpc_master_api/rpc_master_api.h"
#include "../alg/alg_init.h"
#include "../app_init.h"
#include "../process_task/detect_process_task.h"
#include "../process_task/orientation.h"
#include "../rf_config/rf_config.h"

#include "../../app/process_task/detect_interface.h"
#include "../rf_config/pl_config.h"
#include "../rpc_master_api/rpc_master_api.h"

#include "../sys_status_data/detection_target.h"

#include "../devInfo_process/devInfo_process.h"

uint8_t buffer_ADC_[RAW_ADC_DATA_LEN_PER_FRAME] __attribute__((section(".dmaSection")));
uint8_t buffer_ADC1_[RAW_ADC_DATA_LEN_PER_FRAME] __attribute__((section(".dmaSection")));
// uint64_t buffer_aeag_[RAW_ADC_DATA_LEN_PER_FRAME / sizeof(uint64_t)] __attribute__((section(".dmaSection")));
// uint64_t buffer_aeag1_[RAW_ADC_DATA_LEN_PER_FRAME / sizeof(uint64_t)] __attribute__((section(".dmaSection")));
uint32_t buffer_aeag5_[POINT_PACK_NUM][LINE_NUM][COLUMN_NUM];
uint32_t buffer_aeag_[POINT_PACK_NUM][LINE_NUM][COLUMN_NUM];
uint32_t buffer_aeag2_[POINT_PACK_NUM][LINE_NUM][COLUMN_NUM];
uint32_t buffer_aeag1_[NET_MAX_NUM * 2][LINE_NUM][COLUMN_NUM];
uint32_t RecordCenFreq[NET_MAX_NUM];
// uint32_t detalgdata[1][LINE_NUM][COLUMN_NUM] = {0};
uint32_t detalgdata[POINT_PACK_NUM][LINE_NUM][COLUMN_NUM] = {0};
#define REV_DET_ALGDATA_LEN (32 * 1024)
//#define MaxUAV 20

uint8_t buffer_fft_data[16384];

extern SemaphoreHandle_t DetectionSemphrHandleCore0;
extern SemaphoreHandle_t DetectionSemphrHandleCore1;

static QueueHandle_t xDataPathQueue = NULL;
static s32 sDataPathOutEn = 0;

static QueueHandle_t sQueueHandle;

// for ADC
static DataPingPong_t sAdcBuf = {0};
// for rdm
static DataPingPong_t sRdmBuf = {0};
static DataPingPong_t sFftBuf = {0};

uint8_t uOutputSpecMat = 0;

static uint8_t uSysWorkingMode = eWORK_MODE_DETECT;

droneResult_t WifiMiniSeOut = {
		.ID = 167,
		.name = {"DJI MiniSE           "},
		.psbID = {3},
		.nPsbID = 1,
		.flag = 0,
		.freq = {5745, 5765, 5785, 5805, 5825},
};


void SetSysWorkingMode(uint8_t mode)
{
	uSysWorkingMode = mode;
}

uint8_t GetSysWorkingMode(void)
{
	return uSysWorkingMode;
}

void SetOutputSpecMat(uint8_t value)
{
	uOutputSpecMat = value;
}

uint8_t GetOutputSpecMat(void)
{
	return uOutputSpecMat;
}

void set_buffer_empty(s32 type, void *data)
{
	buffer_t *pBuff = NULL;

	if (DIR_DATA == type)
		pBuff = &sAdcBuf.buff1;

	pBuff = (pBuff->pData == (u8 *)data) ? (&sAdcBuf.buff1) : (&sAdcBuf.buff2);

	pBuff->flag = BUFF_EMPTY;
}

void DataPathTask_Post(DataPingPong_t *msg)
{
	BaseType_t xHigherPriorityTaskWoken;
	if (xDataPathQueue != NULL)
	{
		if (xQueueIsQueueFullFromISR(xDataPathQueue) == 0)
		{
			xQueueSendFromISR(xDataPathQueue, msg, &xHigherPriorityTaskWoken);
		}
	}
}

static int32_t DataPathTask_Pend(DataPingPong_t *msg, uint32_t timeout)
{

	int32_t result = -1;

	if (xDataPathQueue != NULL)
	{
		if (xQueueReceive(xDataPathQueue, msg, timeout))
		{
			result = 0;
		}
	}

	return result;
}

void NetDataOutput(uint32_t framenum, uint8_t nDrone, uint8_t FreqItem)
{
	static uint8_t OutputChl, usbprintf, cntNet = 0;
	if (GetOutputSpecMat() != 0)
	{
		if ((framenum >= 2) && (((nDrone >= 1) && (GetOutputSpecMat() == 2)) || (GetOutputSpecMat() == 1) || (GetOutputSpecMat() == 3) || (GetOutputSpecMat() == 4)))
		{
			memcpy(&buffer_aeag1_[cntNet * 2][0][0], &buffer_aeag_[0][0][0], REV_DET_PACK_LEN / 2);
			memcpy(&buffer_aeag1_[cntNet * 2 + 1][0][0], &buffer_aeag_[1][0][0], REV_DET_PACK_LEN / 2);
			LOG_DEBUG("-------------- cnt: %d-------\r\n", cntNet);
			RecordCenFreq[cntNet] = SYS_GetCenterFreq(FreqItem);
			cntNet++;
			if (cntNet >= NET_MAX_NUM)
			{
				data_trans_CacheInvalidate((u8*)buffer_aeag1_, sizeof(buffer_aeag1_));
				if (GetOutputSpecMat() == 3)
				{
					protocol_tcp_send_data("----------------", 16);
					protocol_tcp_send_data(RecordCenFreq, sizeof(RecordCenFreq));
					protocol_tcp_send_data("################", 16);
					protocol_tcp_send_data(buffer_aeag1_, sizeof(buffer_aeag1_));
					protocol_tcp_send_data("&&&&&&&&&&&&&&&&", 16);
					SetOutputSpecMat(0);
				}
				else if ((GetOutputSpecMat() == 4) || (GetOutputSpecMat() == 2))
				{
					usbprintf = GetUsbDataPrintf();
					SetUsbDataPrintf(0x10);
					OutputChl = GetSwitchOutputChl();
					if (OutputChl == 4)
					{
						SetSwitchOutputChl(0);
					}
					Usb_Send_data_func((uint8_t*)"----------------", 16);
					Usb_Send_data_func((uint8_t*)RecordCenFreq, sizeof(RecordCenFreq));
					Usb_Send_data_func((uint8_t*)"################", 16);
//					Usb_Send_data_func((uint8_t*)buffer_aeag1_, sizeof(buffer_aeag1_));
					uint32_t len = 128 ;
					uint32_t loop = sizeof(buffer_aeag1_) / len ;
					uint8_t * buf = (uint8_t*)buffer_aeag1_;
					for( int32_t i = 0x00 ; i < loop ; i++ )
					{
						Usb_Send_data_func((uint8_t*)&buf[len*i], len);
					}
					Usb_Send_data_func((uint8_t*)"&&&&&&&&&&&&&&&&", 16);
					SetUsbDataPrintf(usbprintf);
					if (OutputChl == 4)
					{
						SetSwitchOutputChl(4);
					}
					SetOutputSpecMat(0);
				}
				else
				{
					protocol_udp_send_data2("----------------", 16);
					protocol_udp_send_data2(RecordCenFreq, sizeof(RecordCenFreq));
					protocol_udp_send_data2((uint8_t*)"################", 16);
					protocol_udp_send_data2(buffer_aeag1_, sizeof(buffer_aeag1_));
					protocol_udp_send_data2((uint8_t*)"&&&&&&&&&&&&&&&&", 16);
					SetOutputSpecMat(0);
				}
				cntNet = 0;
				memset(buffer_aeag1_, 0, sizeof(buffer_aeag1_));
			}
		}
	}
}

void Data_FFT_GetFromPl(float* fftoutput)
{
	buffer_t BufMsg = {0};
	BaseType_t ret = pdFALSE;

	ret = xQueueReceive(sQueueHandle, &BufMsg, portMAX_DELAY);
	if (pdTRUE == ret)
	{
//		if ((FFT_DATA == BufMsg.type) && (FFT_DATA == sDataPathOutEn))
		if (FFT_DATA == BufMsg.type)
		{
			memcpy((uint8_t *)fftoutput, BufMsg.pData, FFT_DMA_PACKET_LEN);
		}
	}
}

/**
 * 1、监控pl状态是否正常
 * 2、处理pl过来的数据，上报或者调用后处理算法
 * 3、pl异常后，使其恢复。并上报状态
 */
void DataPath_Task(void *pvParameters)
{
	int nDrone = 0;
	uint32_t framenum = 0;
	DataPingPong_t BufMsg = {0};
	static struct droneResult droneInfo[MaxUAV];
	unsigned short ***specMat = NULL;
	unsigned short ***upMat = NULL;
	float cenFreq = 0.f; // MHz
	float fs = 51.2f;	 // M
	float gain = 10.0f;
	float bw = 48.0f;//M
	float amp = 0;
	int useRow = 3000;
	float tempAmp = 0;
	float roll; float pitch; float yaw;
	unsigned short ***specMatCore1 = createUSMat(1,LINE_NUM,COLUMN_NUM);
	unsigned short ***upMatCore1 = createUSMat(1,LINE_NUM,COLUMN_NUM);
	uint8_t DetFreqItemCurr = 0xFF;
	uint32_t FirstTagetUpdate = 0;
	uint32_t CoreID = 1;
	TickType_t tick,tick1,tick2,tick3,tick4;
	uint8_t DetectLedOnOff = 0;
	uint32_t (*pBuf)[LINE_NUM][COLUMN_NUM] = NULL;
	DroneReportResult_t* pDroneReportResult;
	SYS_SystemStatusMode_t lSysStaus = SYS_IDLE_STATUS;

	DET_OutsideInterface_t *pOutsideInterface = NULL;
	pOutsideInterface = DET_GetOutsideInterface();

	while (1)
	{
		if (DataPathTask_Pend(&BufMsg, 1500) == 0)
		{

			KeyScanf();

			pBuf = (uint32_t*)BufMsg.buff1.pData;
			tick = xTaskGetTickCount();

			specMat = specMatCore1;
			upMat = upMatCore1;

			for (int z = 0; z < POINT_PACK_NUM; z++)
			{
				for (int i = 0; i < LINE_NUM; i++)
				{
					for (int j = 0; j < COLUMN_NUM; j++)
					{
						if (z == 0)
							{
								specMat[0][i][j] = (unsigned short)((pBuf[z][i][j] & 0xFFFF0000) >> 16); // 原图传数据，现全向数据
							}
							else
							{
								upMat[0][i][j] = (unsigned short)((pBuf[z][i][j] & 0xFFFF0000) >> 16); // 原飞控数据  ，现定线数据
							}
					}
				}
			}

			pOutsideInterface->pGetSysStatus(&lSysStaus);
			if (lSysStaus == SYS_DETECTION_TARGET_STATUS)
			{
				if ((specMat != NULL) && (upMat != NULL))
				{

					//对接收的一帧数据进行处理
					DetFreqItemCurr = pOutsideInterface->pGetDetectFreqItem();
					cenFreq = SYS_GetCenterFreq(DetFreqItemCurr);
					RF_StopPLAlg();
					DET_CfgNextDetection();
					RF_StartPLAlgByTimer();
					memset((uint8_t*)&droneInfo, 0xFF, sizeof(droneInfo));
					tempAmp = 0;
					if (CoreID == 1)
					{
						rpc_droneSniffer_noFilt(droneInfo, &nDrone, specMat, upMat, cenFreq, fs, bw, gain, useRow, &tempAmp);
					}

					if (GetOutputSpecMat() != 0)
					{
						NetDataOutput(framenum, nDrone, DetFreqItemCurr);
					}

					if (nDrone >= MaxUAV)
					{
						nDrone = 0;
					}
					printResult2(droneInfo, nDrone);

					amp = 0;
					for(uint8_t i = 0; i < nDrone; i++)
					{
						if (i == 0)
						{
							amp = droneInfo[0].amp;
						}
						if (amp < droneInfo[i].amp)
						{
							amp = droneInfo[i].amp;
						}

					}

					/* when system status is not in detection, not update detection result,
					 * if updated, the hit strategy maybe not according with the drone show
					 * on screen
					 */

					/* only update target list in eWORK_MODE_DETECT mode of SYS_DETECTION_TARGET_STATUS */
					if ((GetSysWorkingMode() == eWORK_MODE_DETECT)
						|| (GetSysWorkingMode() == eWORK_MODE_ELIGIBLE_ENTRY_DIR)
						|| (GetSysWorkingMode() == eWORK_MODE_NO_ELIGIBLE_ENTRY_DIR))
					{
						FirstTagetUpdate = 1;
					}
					else
					{
						FirstTagetUpdate = 0;
					}
					pOutsideInterface->pSetDetectionResult(droneInfo, nDrone, DetFreqItemCurr, FirstTagetUpdate);

					/* when no target been detected, must restore the work mode */
					if (pOutsideInterface->pGetDroneTargetCnt() == 0)
					{
						SetSysWorkingMode(eWORK_MODE_DETECT);
						SetLastDetFreqItem(0xFF);
					}


					if ((FirstTagetUpdate == 0) && (GetJudgeIfDirMiniSe() == 1))
					{//已经进入定向，且是定向MiniSE的，不再进入

					}
					else
					{
						pDroneReportResult = SYS_GetDroneInfo();

						GetAngleInfo(&roll, &pitch, &yaw);
						OrientateProcess(framenum, yaw, pitch, tempAmp, GetLastDetFreqItem(), specMat, upMat, DetFreqItemCurr, fs, pDroneReportResult);
					}

				}


			framenum ++;
			}
		}
		else
		{

#if( WORKMODE == SPECTRUM)
			LOG_DEBUG("data_path timeout!\r\n");
#endif
		}

		DetectLedOnOff = !DetectLedOnOff;
	}


}

void data_path_buff_init(s32 type)
{
	if (DIR_DATA == type)
	{
	}
	else if (DET_DATA == type)
	{
		sRdmBuf.revLen = 0;
		sRdmBuf.buff1.flag = BUFF_EMPTY;
		sRdmBuf.buff1.length = REV_DET_PACK_LEN;
		sRdmBuf.buff1.type = DET_DATA;
		sRdmBuf.buff1.ts = 0;
		sRdmBuf.buff1.pData = (u8 *)buffer_aeag_;
	}
	else if (FFT_DATA == type)
	{
		sFftBuf.revLen = 0;
		sFftBuf.buff1.flag = BUFF_EMPTY;
		sFftBuf.buff1.length = REV_DET_PACK_LEN;
		sFftBuf.buff1.type = FFT_DATA;
		sFftBuf.buff1.ts = 0;
		sFftBuf.buff1.pData = (u8 *)buffer_aeag5_;
	}
}

buffer_t *data_path_getUsedBuf(s32 type)
{
	DataPingPong_t *pPing = NULL;
	buffer_t *pBuf = NULL;

	if (DIR_DATA == type)
	{
		pPing = &sAdcBuf;
	}
	else if (DET_DATA == type)
	{
		pPing = &sRdmBuf;
	}
	else if (FFT_DATA == type)
	{
		pPing = &sFftBuf;
	}
	else
	{
		;
	}

	if (pPing != NULL)
	{
		//	if ((BUFF_EMPTY == pPing->buff2.flag) &&
		//		(BUFF_EMPTY == pPing->buff1.flag))
		{
			pPing->usedId = 1;
		}
	}

	pBuf = (1 == pPing->usedId) ? (&pPing->buff1) : (&pPing->buff2);

	return pBuf;
}

buffer_t *data_path_getFullBuf(s32 type)
{
	DataPingPong_t *pPing = NULL;
	buffer_t *pBuf = NULL;

	if (DIR_DATA == type)
	{
		pPing = &sAdcBuf;
	}
	else if (DET_DATA == type)
	{
		pPing = &sRdmBuf;
	}
	pBuf = (1 == pPing->usedId) ? (&pPing->buff2) : (&pPing->buff1);

	if (BUFF_FULL != pBuf->flag)
		pBuf = NULL;

	return pBuf;
}

void RDMAPDataCallback(s32 type, s32 len)
{
	buffer_t *pBuf = &sRdmBuf.buff1;
	DataPingPong_t *pDataPingPong = &sRdmBuf;
	uint8_t *pDetalgData = (uint8_t *)detalgdata;

#if( WORKMODE == SPECTRUM)
	{
		data_trans_CacheInvalidate(pDetalgData + sRdmBuf.revLen, DET_DMA_PACKET_LEN);

		if (sDataPathOutEn != NO_DATA)
		{
			if (pBuf->flag == BUFF_FULL)
			{
				sRdmBuf.revLen = 0;
			}
			else
			{
				sRdmBuf.revLen += (u32)len;
				if (REV_DET_PACK_LEN <= sRdmBuf.revLen)
				{
					sRdmBuf.revLen = 0;
					pBuf->flag = BUFF_FULL;
					for (uint8_t i = 0; i < DMA_PACK_NUM; i++)
					{
						memcpy(pBuf->pData + i * MAT_PACKET_LEN, pDetalgData + i * DET_DMA_PACKET_LEN, MAT_PACKET_LEN);
						memcpy(pBuf->pData + REV_DET_PACK_LEN / 2 + i * MAT_PACKET_LEN, pDetalgData + i * DET_DMA_PACKET_LEN + MAT_PACKET_LEN, MAT_PACKET_LEN);
					}
					DataPathTask_Post(pDataPingPong);
				}
			}
		}
		data_trans_start(DET_DATA, pDetalgData + sRdmBuf.revLen, DET_DMA_PACKET_LEN);
	}

#endif
}

void FFTDataCallback(s32 type, s32 len)
{
	portBASE_TYPE taskWoken = pdFALSE;
	buffer_t *pBuf = &sFftBuf.buff1;

	data_trans_CacheInvalidate(pBuf->pData, FFT_DMA_PACKET_LEN);
//	if (sDataPathOutEn == FFT_DATA)
	{
//		memcpy(buffer_fft_data, buffer_aeag5_, FFT_DMA_PACKET_LEN);
		xQueueSendFromISR(sQueueHandle, pBuf, &taskWoken);
		if (pdTRUE == taskWoken)
		{
			portYIELD_FROM_ISR(taskWoken);
		}
	}
	data_trans_start(FFT_DATA, pBuf->pData, FFT_DMA_PACKET_LEN);
}

s32 data_path_init(void)
{
	s32 status = 0;
	buffer_t *pBuf = NULL;

	data_path_buff_init(DET_DATA);
	data_path_buff_init(FFT_DATA);

	if (!status)
		status = data_trans_init(DET_DATA);
	if (!status)
		status = data_trans_set_callback(DET_DATA, RDMAPDataCallback);

	if (!status)
		status = data_trans_init(FFT_DATA);
	if (!status)
		status = data_trans_set_callback(FFT_DATA, FFTDataCallback);

	status = data_trans_start(DET_DATA, (u8*)detalgdata, DET_DMA_PACKET_LEN);
	pBuf = data_path_getUsedBuf(FFT_DATA);
	if (pBuf)
	{
		status = data_trans_start(FFT_DATA, pBuf->pData, FFT_DMA_PACKET_LEN);
	}

	xDataPathQueue = xQueueCreate(1, sizeof(DataPingPong_t));
	if (NULL == xDataPathQueue)
	{
		status = -1;
	}

	sQueueHandle = xQueueCreate(10, sizeof(buffer_t));
	if (NULL == sQueueHandle)
	{
		status = -1;
	}

	if (xTaskCreate(DataPath_Task, "DataPath_Task", TASK_STACK_SIZE_DATA_PATH, NULL, TASK_PRI_DATA_PATH, NULL) != pdPASS)
	{
		status = -1;
	}

	return status;
}

void data_path_out_en(s32 type)
{
	sDataPathOutEn = type;
}

s32 GetDataPathOutEn(void)
{
	return sDataPathOutEn;
}

void ClearDetDmaBuf(void)
{
	sRdmBuf.revLen = 0;
	sRdmBuf.buff1.flag = BUFF_EMPTY;
}

s32 data_path_start(s32 type)
{
	s32 status = 0;

	return status;
}

void InitDataPath_Task()
{

	SpectrumSeamphoreCreate();

	data_path_init();

	data_path_out_en(DET_DATA);
}
