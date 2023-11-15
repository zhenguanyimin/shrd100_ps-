#include <string.h>
#include "remoteid_app.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "portmacro.h"
#include "../../cfg/shrd_config.h"
#include "../../drv/uart_init/uart_init.h"
#include "../../hal/hal_remoteid/hal_remoteid.h"
#include "../../srv/alink/alink_msg_proc.h"
#include "../../srv/alink/checksum.h"
#include "../../srv/log/log.h"
#include "remoteid_parse.h"
#include "xil_printf.h"

static QueueHandle_t xRemoteIdQueue;


#define UART_RECV_DATA_BUFF (3 * 1024)
static uint8_t RemoteIdSendBuff[UART_RECV_DATA_BUFF]; // 找出的有用的beacon信息
static RemoteID_DataInfo_t RemoteidSendDataInfo = {0};

static SemaphoreHandle_t  xRemoteidSeamphore;

void UartPsMsgProcTask_Post(RemoteID_DataInfo_t* bufaddr)
{
	if (xRemoteIdQueue != NULL)
	{
		BaseType_t ret = 0;
		BaseType_t xHigherPriorityTaskWoken;
		ret = xQueueIsQueueFullFromISR(xRemoteIdQueue);
		if (pdTRUE != ret)
			xQueueSendFromISR(xRemoteIdQueue, bufaddr, &xHigherPriorityTaskWoken);
	}
}


static int32_t UartPsMsgProcTask_Pend(RemoteID_DataInfo_t* bufaddr, uint32_t timeout)
{
	int32_t result = -1;

	if (xRemoteIdQueue != NULL)
	{
		if (xQueueReceive(xRemoteIdQueue, bufaddr, timeout))
		{
			result = 0;
		}
	}

	return result;
}

void RemoteIdSeamphoreTake(void)
{
	if (xRemoteidSeamphore != NULL)
	{
		xSemaphoreTake(xRemoteidSeamphore, 10000);
	}
}

void RemoteIdSeamphoreGive(void)
{
	if (xRemoteidSeamphore != NULL)
	{
		xSemaphoreGive(xRemoteidSeamphore);
	}
}
//static int RemoteId_PackageUartRecvData(RemoteID_DataInfo_t sourceAddrInfo, RemoteID_DataInfo_t* PackResult)
//{
//	uint16_t i;
//	uint8_t data[10] = {0};
//	uint16_t RemotePrcoessDataLen = 0; // 此时需要全部处理的长度
//	memset(PackResult, 0, sizeof(RemoteID_DataInfo_t));
//
//	for (i = 0; i < sourceAddrInfo.datalen; i++)
//	{
//		xil_printf("%x  ",sourceAddrInfo.databuff[i]);
//	}
//	xil_printf("\r\n");
//	memset(UartProcRecvBuff, 0, sizeof(UartProcRecvBuff));
//	if (RecvPackDataCnt == 0) // 开始查找 80 00 00 00 FF FF FF FF FF FF
//	{
//		memcpy(UartProcRecvBuff, UartRecvRemainBuff, RecvPackRemainLen);
//		memcpy(UartProcRecvBuff + RecvPackRemainLen, sourceAddrInfo.databuff, sourceAddrInfo.datalen);
//		RemotePrcoessDataLen = RecvPackRemainLen + sourceAddrInfo.datalen;
//		memset(UartRecvRemainBuff, 0, sizeof(UartRecvRemainBuff));
//		RecvPackRemainLen = 0;
//		for (i = 0; i < RemotePrcoessDataLen; i++)
//		{
//			if (UartProcRecvBuff[i] == 0x80)
//			{
//				memcpy(data, &UartProcRecvBuff[i], 10);
//				if ((data[0] == 0x80) && (data[1] == 0x0) && (data[2] == 0x0) && (data[3] == 0x0) && (data[4] == 0xFF) && (data[5] == 0xFF) \
//						&& (data[6] == 0xFF) && (data[7] == 0xFF))
//				{
//					RemoteIdPackLength = data[8] << 8 | data[9];
//
//					if (RemoteIdPackLength <= (RemotePrcoessDataLen - i))
//					{
//						memcpy(RemoteIdSendBuff, &UartProcRecvBuff[i], RemoteIdPackLength);
//						RecvPackRemainLen += (RemotePrcoessDataLen - i) - RemoteIdPackLength;
//						memcpy(UartRecvRemainBuff, &UartProcRecvBuff[i] + RemoteIdPackLength, RecvPackRemainLen);
//						PackResult->databuff = RemoteIdSendBuff;
//						PackResult->datalen = RemoteIdPackLength;
//						RemoteIdPackLength = 0;
//						RecvPackDataCnt = 0;
//						return 1;
//					}
//					else if (RemoteIdPackLength > (RemotePrcoessDataLen - i))
//					{
//						memcpy(RemoteIdSendBuff, &UartProcRecvBuff[i], RemotePrcoessDataLen - i);
//						RecvPackDataCnt = RemotePrcoessDataLen - i;
////						RecvPackRemainLen = 0;
//						break;
//					}
//				}
//			}
//		}
//	}
//	else // 上次接收的数据beacon 不完整
//	{
//		memcpy(UartProcRecvBuff, sourceAddrInfo.databuff, sourceAddrInfo.datalen);
//		RemotePrcoessDataLen = sourceAddrInfo.datalen;
//		if (RemotePrcoessDataLen >= (RemoteIdPackLength - RecvPackDataCnt))
//		{
//			memcpy(RemoteIdSendBuff + RecvPackDataCnt, UartProcRecvBuff, RemoteIdPackLength - RecvPackDataCnt);
//			memcpy(UartRecvRemainBuff, UartProcRecvBuff + (RemoteIdPackLength - RecvPackDataCnt), RemotePrcoessDataLen - (RemoteIdPackLength - RecvPackDataCnt));
//			RecvPackRemainLen += RemotePrcoessDataLen - (RemoteIdPackLength - RecvPackDataCnt);
//			PackResult->databuff = RemoteIdSendBuff;
//			PackResult->datalen = RemoteIdPackLength;
//			RemoteIdPackLength = 0;
//			RecvPackDataCnt = 0;
//			return 1;
//		}
//		else
//		{
//			memcpy(UartRecvRemainBuff + RecvPackRemainLen, UartProcRecvBuff, RemotePrcoessDataLen);
//			RecvPackRemainLen += RemotePrcoessDataLen;
//		}
//	}
//
//	return 0;
//}


//static uint8_t UartRecvTimeCount = 0;
//static uint16_t UartRecvDataCount = 0;

static int RemoteId_PackageUartRecvData(RemoteID_DataInfo_t sourceAddrInfo, RemoteID_DataInfo_t* PackResult)
{
	int i;
	uint8_t data[10] = {0};
	uint16_t RemoteIdPackLength = 0;
	uint8_t* databuff = NULL;

//	xil_printf("len: %d \r\n",sourceAddrInfo.datalen);
//	for (i = 0; i < sourceAddrInfo.datalen; i++)
//	{
//		xil_printf("%02x  ",sourceAddrInfo.databuff[i]);
//	}
//	xil_printf("\r\n\r\n\r\n");


	databuff = sourceAddrInfo.databuff;
	for (i = 0; i < sourceAddrInfo.datalen; i++)
	{
		if (databuff[i] == 0x80)
		{
			memcpy(data, &databuff[i], 10);
			if ((data[0] == 0x80) && (data[1] == 0x0) && (data[2] == 0x0) && (data[3] == 0x0) && (data[4] == 0xFF) && (data[5] == 0xFF) \
					&& (data[6] == 0xFF) && (data[7] == 0xFF))
			{
				RemoteIdPackLength = data[8] << 8 | data[9];
				memset(RemoteIdSendBuff, 0, sizeof(RemoteIdSendBuff));
				memcpy(RemoteIdSendBuff, &databuff[i], RemoteIdPackLength);
				PackResult->databuff = RemoteIdSendBuff;
				PackResult->datalen = RemoteIdPackLength;
				return 1;
			}
		}
	}

	return 0;
}


void RemoteIdHandle_Task()
{
	RemoteID_DataInfo_t sourceAddrInfo = {0};
	remotid_msg_head_t *pRemoteIDRecv_Head = NULL;;

	while(1)
	{
		if (UartPsMsgProcTask_Pend(&sourceAddrInfo, 50) == 0)
		{
			if (RemoteId_PackageUartRecvData(sourceAddrInfo, &RemoteidSendDataInfo))
			{
				pRemoteIDRecv_Head = (remotid_msg_head_t *)RemoteidSendDataInfo.databuff;
				if (RemoteIDMsgProc_IsHeaderValid(pRemoteIDRecv_Head))
				{
					RemoteID_BeaconFreamProcess(RemoteidSendDataInfo.databuff, RemoteidSendDataInfo.datalen);
				}
			}
		}
		RemoteId_UpdateOutputList();
	}
}


int remoteid_init()
{
	int32_t retVal = -1;

	xRemoteIdQueue = xQueueCreate(2, sizeof(RemoteID_DataInfo_t));
	uart_init();
	if (xTaskCreate(RemoteIdHandle_Task, "RemoteIdHandle_Task", TASK_STACK_SIZE_REMOTEHANDLE, NULL, TASK_PRI_REMOTEHANDLE, NULL) == 1)
	{
		retVal = 0;
	}

	return retVal;
}



