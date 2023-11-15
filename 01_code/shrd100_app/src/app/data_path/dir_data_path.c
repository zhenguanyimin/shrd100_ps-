/*
 * wifi_data_path.c
 *
 *  Created on: 2023年5月5日
 *      Author: A19199
 */

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

#include "../../cfg/shrd_config.h"
#include "../../srv/log/log.h"
#include "data_path.h"
static QueueHandle_t xDirDataPathQueue;


static DataPingPong_t DirBuf = {0};
static uint8_t uRevFlag = DMA_PACK_NUM;
static uint8_t DirDataProcessing = 0;

uint32_t DmaDirbuffe[POINT_PACK_NUM][LINE_NUM][COLUMN_NUM];
uint32_t Dirbuffe[POINT_PACK_NUM][LINE_NUM][COLUMN_NUM];
uint32_t Dirbuffe1[NET_MAX_NUM * 2][LINE_NUM][COLUMN_NUM];

void DirDataPathTask_Post(buffer_t *msg)
{
	BaseType_t xHigherPriorityTaskWoken;
	if (xDirDataPathQueue != NULL)
	{
		if (xQueueIsQueueFullFromISR(xDirDataPathQueue) == 0)
		{
			xQueueSendFromISR(xDirDataPathQueue, msg, &xHigherPriorityTaskWoken);
		}
	}
}

static int32_t DirDataPathTask_Pend(buffer_t *msg, uint32_t timeout)
{

	int32_t result = -1;

	if (xDirDataPathQueue != NULL)
	{
		if (xQueueReceive(xDirDataPathQueue, msg, timeout))
		{
			result = 0;
		}
	}

	return result;
}

void DirDataPath_Task(void *pvParameters)
{
	buffer_t BufMsg = {0};
	xDirDataPathQueue = xQueueCreate(2, sizeof(buffer_t));

	while (1)
	{
		if (DirDataPathTask_Pend(&BufMsg, 1500) == 0)
		{
		}
		else
		{
			LOG_DEBUG("dir:data_path timeout!\r\n");
		}
	}
}


void DirDataPathBuff_Init(void)
{
	DirBuf.revLen = 0;
	DirBuf.buff1.flag = BUFF_EMPTY;
	DirBuf.buff1.length = 0;
	DirBuf.buff1.type = DET_DATA;
	DirBuf.buff1.ts = 0;
	DirBuf.buff1.pData = (u8 *)&Dirbuffe;
}

void DirDataCallback(s32 type, s32 len)
{
	buffer_t *pBuf = &DirBuf.buff1;
	uint8_t *pDetalgData = (uint8_t *)DmaDirbuffe;
	static uint64_t uDmaRevCount = 0;

	data_trans_CacheInvalidate(pBuf->pData + DirBuf.revLen, DET_DMA_PACKET_LEN);
	if (GetDataPathOutEn() != NO_DATA)
	{
		uDmaRevCount++;
		if ((uDmaRevCount >= DMA_PACK_NUM))
		{
			uDmaRevCount = 0;
			uRevFlag = DMA_PACK_NUM;
		}
	}
	if (DirDataProcessing == 1)
	{
		DirBuf.revLen = 0;
		len = 0;
	}
	if (uRevFlag)
	{
		DirBuf.revLen += (u32)len;
		if (REV_DET_PACK_LEN <= DirBuf.revLen)
		{
			DirBuf.revLen = 0;
			pBuf->flag = BUFF_FULL;
		}
		if (BUFF_FULL == pBuf->flag)
		{
			if (DirDataProcessing == 0)
			{
				DirDataProcessing = 1;
				for (uint8_t i = 0; i < DMA_PACK_NUM; i++)
				{
					memcpy(pBuf->pData + i * MAT_PACKET_LEN, pDetalgData + i * DET_DMA_PACKET_LEN, MAT_PACKET_LEN);
					memcpy(pBuf->pData + REV_DET_PACK_LEN / 2 + i * MAT_PACKET_LEN, pDetalgData + i * DET_DMA_PACKET_LEN + MAT_PACKET_LEN, MAT_PACKET_LEN);
				}

				DirDataPathTask_Post(pBuf);
			}
		}
		uRevFlag--;
	}
	data_trans_start(DIR_DATA, pBuf->pData + DirBuf.revLen, DET_DMA_PACKET_LEN);
}

int32_t InitDirDataPath_Task()
{
	int32_t retVal = -1;

	retVal = data_trans_init(DIR_DATA);
	if (!retVal)
		retVal = data_trans_set_callback(DIR_DATA, DirDataCallback);
	data_path_start(DIR_DATA); //

	if (xTaskCreate(DirDataPath_Task, "DirDataPath_Task", TASK_STACK_SIZE_KEYHANDLE, NULL, TASK_PRI_KEYHANDLE, NULL) == 1)
	{
		retVal = 0;
	}

	return retVal;
}

