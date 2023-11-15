
/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include <queue.h>
#include <timers.h>

#include "alink_msg_common.h"
#include "alink_msg_proc.h"
#include "../log/log.h"
//#include "../watch_srv/watch_srv.h"
#include "checksum.h"
#include "../../drv/axiuart/sys_intr.h"
//#include "../watch_srv/watch_srv.h"
#include "../../drv/axiuart/sys_intr.h"
#include "../../drv/uartlite/uartlite_drv.h"
//#include "../../app/alg/detection/detection_interface.h"
//#include "../../srv/watch_srv/servers/service_setheartbeatinfo.h"
#include "../../drv/fc41d/bt_wifi.h"
#include "../../inc/common.h"

#include "../link_srv/link_srv.h"
#include "../../drv/usb/xusb_cdc_acm_api.h"
#include "../../drv/uartlite/uartlite_drv.h"
#include "../../app/btAndWifi/app_bt_wifi.h"
#include "../../drv/fc41d/bt_uart.h"
static TimerHandle_t Timer_Handle = NULL;

#define UART_BUFFER_SIZE 1024
alink_msg_t sAlinkSend_msg;
alink_msg_t sAlinkRecv_msg;
uint8_t RecvBuffer[UART_BUFFER_SIZE];

SemaphoreHandle_t  aLinkMsgSeamphore;

static QueueHandle_t xAlinkQueue;
static QueueHandle_t xAlinkSendRecvQueue;
static uint8_t uRecvHeadCnt = 0;
static uint8_t uRecvBufCnt = 0;
static uint16_t uLen = 0;
extern uint8_t AT_QBLEGATTSNTFY[512];


static uint8_t ualinkinterface = TYPE_C;
void SetAlinkInterface(uint8_t value)
{
	ualinkinterface = value;
}

uint8_t GetAlinkInterface(void)
{
    return ualinkinterface;
}

alink_msg_t *GetAlinkSend_msg(void)
{
	return &sAlinkSend_msg;
}

void AlinkMsgProcTask_Post(uint32_t bufaddr)
{
	if (xAlinkQueue != NULL)
	{
		BaseType_t ret = 0;
		BaseType_t xHigherPriorityTaskWoken;
		ret = xQueueIsQueueFullFromISR(xAlinkQueue);
		if (pdTRUE != ret)
			xQueueSendFromISR(xAlinkQueue, &bufaddr, &xHigherPriorityTaskWoken);
	}
}

static int32_t AlinkMsgProcTask_Pend(uint32_t *bufaddr, uint32_t timeout)
{
	int32_t result = -1;

	if (xAlinkQueue != NULL)
	{
		if (xQueueReceive(xAlinkQueue, (void *)bufaddr, timeout))
		{
			result = 0;
		}
	}

	return result;
}

void AlinkMsgRecv_Post(alink_ans_t *msg)
{
	if (xAlinkSendRecvQueue != NULL)
	{
		xQueueSend(xAlinkSendRecvQueue, msg, 10);
	}
}

static int32_t AlinkMsgRecv_Pend(alink_ans_t *msg, uint32_t timeout)
{
	int32_t result = -1;

	if (xAlinkSendRecvQueue != NULL)
	{
		if (xQueueReceive(xAlinkSendRecvQueue, (void *)msg, timeout))
		{
			result = 0;
		}
	}

	return result;
}

static uint8_t AlinkMsgProc_GetHeaderChecksum(alink_msg_head_t *header)
{
	uint8_t *pkt = (uint8_t *)header;
	uint8_t checksum = 0;
	uint8_t i = 0;

	for (i = 0; i < ALINK_CORE_HEADER_LEN; i++)
	{
		checksum += pkt[i];
	}

	return checksum;
}

bool AlinkMsgProc_IsHeaderValid(alink_msg_head_t *header)
{
	bool ret = true;
	uint32_t len_msg_payload = 0;
	uint8_t checksum = 0;

	// check the magic
	if (header->magic != ALINK_MSG_MAGIC_VAL)
	{
		LOG_VERBOSE("0x%x!=0x%x\r\n", header->magic, ALINK_MSG_MAGIC_VAL);
		ret = false;
		goto out;
	}

	// check the length
	len_msg_payload = (header->len_hi << 8) + header->len_lo;
	if (len_msg_payload > ALINK_MAX_PAYLOAD_LEN)
	{
		LOG_VERBOSE("%u > %u\r\n", len_msg_payload, ALINK_MAX_PAYLOAD_LEN);
		ret = false;
		goto out;
	}

	// check the destid
//	if ((header->destid & 0xf) != DEV_DRONEID)
//	{
////		if (g_alg_debug_printf_en != PRINT_PARA_ESTIMATE_RESULT)
//		LOG_VERBOSE("0x%x!=0x%x\r\n", header->destid, DEV_DRONEID);
//		ret = false;
//		goto out;
//	}

	// check the checksum
	checksum = AlinkMsgProc_GetHeaderChecksum(header);
	if (header->checksum != checksum)
	{
//		if (g_alg_debug_printf_en != PRINT_PARA_ESTIMATE_RESULT)
			//		LOG_DEBUG("0x%x!=0x%x\r\n", header->checksum, checksum);
			ret = false;
		goto out;
	}

out:
	return ret;
}

int8_t AlinkMsgSeamphoreTake(SemaphoreHandle_t seamphore, TickType_t tick)
{
	if (seamphore == NULL)
	{
		return -1;
	}
	xSemaphoreTake(aLinkMsgSeamphore, tick);
	return 0;
}

int8_t AlinkMsgSeamphoreGive(SemaphoreHandle_t seamphore)
{
	if (seamphore == NULL)
	{
		return -1;
	}
	xSemaphoreGive(aLinkMsgSeamphore);
	return 0;
}


static void UartLiteRecvHandler(void *CallBackRef, uint32_t EventData)
// static void UartLiteRecvHandler(uint32_t EventData)
{
	uint16_t crc;
	//	XUartLite * Uartpl = (XUartLite *) CallBackRef;
	// todo  加互斥锁
	if (EventData > 0)
	{
		//		Dis_UartliteRecvData(RecvBuffer, EventData);
		if (uRecvHeadCnt < sizeof(alink_msg_head_t))
		{
			if ((EventData + uRecvHeadCnt) >= sizeof(alink_msg_head_t))
			{
				uint8_t len = sizeof(alink_msg_head_t) - uRecvHeadCnt;
				uint8_t *buf = (uint8_t *)&sAlinkRecv_msg.msg_head;
				memcpy(buf + uRecvHeadCnt, RecvBuffer, len);
				uRecvHeadCnt = sizeof(alink_msg_head_t);

				if (AlinkMsgProc_IsHeaderValid(&sAlinkRecv_msg.msg_head))
				{
					uLen = ((sAlinkRecv_msg.msg_head.len_hi << 8) + sAlinkRecv_msg.msg_head.len_lo);
					if ((EventData - len) > 0)
					{
						xTimerStartFromISR(Timer_Handle, 0);
						uRecvBufCnt = EventData - len;
						memcpy(sAlinkRecv_msg.buffer, RecvBuffer + len, uRecvBufCnt);
						if (uRecvBufCnt >= uLen)
						{
							crc = (sAlinkRecv_msg.buffer[uLen - 1] << 8) + sAlinkRecv_msg.buffer[uLen - 2];
							if (AlinkMsg_IsCrcValid((uint8_t *)&sAlinkRecv_msg, uLen + uRecvHeadCnt + 2, crc))
							{
								AlinkMsgProcTask_Post((uint32_t)&sAlinkRecv_msg);
								SetAlinkInterface(UART);
							}
							uRecvHeadCnt = 0;
							uRecvBufCnt = 0;
						}
					}
				}
				else
				{
					uRecvHeadCnt = 0;
					uRecvBufCnt = 0;
				}
			}
			else
			{
				uint8_t *buf = (uint8_t *)&sAlinkRecv_msg.msg_head;
				memcpy(buf + uRecvHeadCnt, RecvBuffer, EventData);
				uRecvHeadCnt += EventData;
				xTimerStartFromISR(Timer_Handle, 0);
			}
		}
		else if (uRecvBufCnt < (uLen + ALINK_NUM_CHECKSUM_BYTES))
		{
			uint8_t *buf = (uint8_t *)&sAlinkRecv_msg.buffer;
			memcpy(buf + uRecvBufCnt, RecvBuffer, EventData);
			uRecvBufCnt += EventData;
			xTimerStartFromISR(Timer_Handle, 0);
			if (uRecvBufCnt >= (uLen + ALINK_NUM_CHECKSUM_BYTES))
			{
				//				crc = (sAlinkRecv_msg.buffer[uLen - 1] << 8) + sAlinkRecv_msg.buffer[uLen - 2];
				crc = (sAlinkRecv_msg.buffer[uLen + 1] << 8) + sAlinkRecv_msg.buffer[uLen];
				if (AlinkMsg_IsCrcValid((uint8_t *)&sAlinkRecv_msg, uLen + uRecvHeadCnt + 2, crc))
				{
					AlinkMsgProcTask_Post((uint32_t)&sAlinkRecv_msg);
					SetAlinkInterface(UART);
				}
				uRecvHeadCnt = 0;
				uRecvBufCnt = 0;
			}
		}
	}
	UartLite_RevData(WATCH_UARTLITE_NUM, RecvBuffer, UART_BUFFER_SIZE);
}
/*
 *  send data
 * return
 */
int32_t AlinkMsgSend(void *pUartpl, alink_msg_t *pMsg)
{
	int32_t ret = -1;
	uint16_t uSendLen = 0;
	uint16_t uPayloadLen = 0;
	uint16_t crc = 0;

	if (pMsg != NULL)
	{
		uPayloadLen = (pMsg->msg_head.len_hi << 8) + pMsg->msg_head.len_lo;
		uSendLen = (pMsg->msg_head.len_hi << 8) + pMsg->msg_head.len_lo + sizeof(alink_msg_head_t);
		pMsg->msg_head.checksum = AlinkMsgProc_GetHeaderChecksum(&pMsg->msg_head);
		crc = crc_calculate((const uint8_t *)((&pMsg->msg_head.magic) + 1), uPayloadLen + 8); // 计算校验值不包含帧头，校验从字节1至字节N+8
		pMsg->buffer[uPayloadLen] = crc & 0xFF;
		pMsg->buffer[uPayloadLen + 1] = (crc & 0xFF00) >> 8;

		if (uSendLen > 0)
		{
			if(GetAlinkInterface() == WIFI)
			{
				if((pMsg->msg_head.msgid == 0xEF)&&(get_bt_wifi_connect_status()==0))
				{
					return ret;
				}
				WifiSendDataFunc((char*)pMsg, uSendLen + 2 );

			}
			if(GetAlinkInterface() == TYPE_C)
			{
				if(GetSwitchOutputChl() == LOG_PS_USB)
				{
					return ret;
				}
				UsbSendDataFunc((uint8_t *)pMsg, uSendLen + 2);
			}
		}
	}

	return ret;
}

/*
 *  send data
 * return
 */
int32_t AlinkMsgBtSend(void *pUartpl, alink_msg_t *pMsg)
{
	int32_t ret = -1;
	uint16_t uSendLen = 0;
	uint16_t uPayloadLen = 0;
	uint16_t crc = 0;
	uint8_t dataSendBuf[sizeof(alink_msg_head_t) + RECV_MAX_BUFF] = {0};

	if (pMsg != NULL)
	{
		uPayloadLen = (pMsg->msg_head.len_hi << 8) + pMsg->msg_head.len_lo;
		uSendLen = (pMsg->msg_head.len_hi << 8) + pMsg->msg_head.len_lo + sizeof(alink_msg_head_t);
		pMsg->msg_head.checksum = AlinkMsgProc_GetHeaderChecksum(&pMsg->msg_head);
		crc = crc_calculate((const uint8_t *)((&pMsg->msg_head.magic) + 1), uPayloadLen + 8);  // 计算校验值不包含帧头，校验从字节1至字节N+8
		pMsg->buffer[uPayloadLen] = crc & 0xFF;
		pMsg->buffer[uPayloadLen + 1] = (crc & 0xFF00) >> 8;

		if (uSendLen > 0)
		{

			hex_to_asciistring((uint8_t*)pMsg , uSendLen + 2, dataSendBuf);
//			BTSendStrData(dataSendBuf, strlen((char *)dataSendBuf), RETURN_DATA_CKECK);
			LOG_DEBUG("bt send:%s\r\n", dataSendBuf);
//			ret = UartLite_SendData(WATCH_UARTLITE_NUM, (uint8_t*)pMsg, uSendLen + 2);
		}
	}

	return ret;
}

/*
 *  send data
 * return
 */
int32_t AlinkMsgResponseBtSend(void *pUartpl, alink_msg_t *pMsg)
{
	int32_t ret = -1;
	uint16_t uSendLen = 0;
	uint16_t uPayloadLen = 0;
	uint16_t crc = 0;
	uint8_t dataSendBuf[sizeof(alink_msg_head_t) + RECV_MAX_BUFF] = {0};
	btWifiErrorState_e rett;

	if (pMsg != NULL)
	{
		uPayloadLen = (pMsg->msg_head.len_hi << 8) + pMsg->msg_head.len_lo;
		uSendLen = (pMsg->msg_head.len_hi << 8) + pMsg->msg_head.len_lo + sizeof(alink_msg_head_t);
		pMsg->msg_head.checksum = AlinkMsgProc_GetHeaderChecksum(&pMsg->msg_head);
		crc = crc_calculate((const uint8_t *)((&pMsg->msg_head.magic) + 1), uPayloadLen + 8);  // 计算校验值不包含帧头，校验从字节1至字节N+8
		pMsg->buffer[uPayloadLen] = crc & 0xFF;
		pMsg->buffer[uPayloadLen + 1] = (crc & 0xFF00) >> 8;

		if (uPayloadLen > 0)
		{
			hex_to_asciistring((uint8_t*)pMsg , uSendLen + 2, dataSendBuf);
			for (uint8_t i = 0; i < 3; i++)
			{
//				rett = BTSendStrData(dataSendBuf, strlen((char *)dataSendBuf), RETURN_DATA_CKECK);
				if (rett == BT_WIFI_OK)
				{
					LOG_DEBUG("bt send str data success i=%d\r\n", i + 1);
					break;
				}
				xil_printf("bt send str data fail i=%d\r\n", i + 1);
				vTaskDelay(100);
			}

			LOG_DEBUG("bt send:%s\r\n", dataSendBuf);
			LOG_DEBUG("bt send original cmd:%s\r\n", AT_QBLEGATTSNTFY);

//			hex_to_asciistring((uint8_t*)pMsg , uSendLen + 2, dataSendBuf);
//			BTSendStrData(dataSendBuf, strlen((char *)dataSendBuf), RETURN_DATA_CKECK);
//			xil_printf("bt send:%s\r\n", dataSendBuf);
////			ret = UartLite_SendData(WATCH_UARTLITE_NUM, (uint8_t*)pMsg, uSendLen + 2);
//			ret = UartLite_SendData(WATCH_UARTLITE_NUM, (uint8_t *)pMsg, uSendLen + 2);
		}
	}

	return ret;
}

/*
 *  send-receive data
 * return
 */
int32_t AlinkMsgSendRecv(void *pUartpl, alink_msg_t *pMsg, alink_ans_t *pAns_msg, uint32_t timeout)
{
	int ret = -1;

//	AlinkMsgSeamphoreTake(aLinkMsgSeamphore,ALINK_MSG_SEAMPHORE_DELY);
	if (AlinkMsgSend(pUartpl, pMsg))
	{
		ret = 0;
		if ((timeout) && (pAns_msg))
		{
			ret = -2;
			if (AlinkMsgRecv_Pend(pAns_msg, timeout) == 0)
			{
				ret = 0;
			}
		}
	}
//	AlinkMsgSeamphoreGive(aLinkMsgSeamphore);

	return ret;
}


/*
 *  send-receive data
 * return
 */
int32_t AlinkMsgBtSendRecv(void *pUartpl, alink_msg_t *pMsg, alink_ans_t *pAns_msg, uint32_t timeout)
{
	int ret = -1;

//	AlinkMsgSeamphoreTake(aLinkMsgSeamphore,ALINK_MSG_SEAMPHORE_DELY);
	if(AlinkMsgBtSend(pUartpl, pMsg))
	{
		ret = 0;
		if ((timeout) && (pAns_msg))
		{
			ret = -2;
			if (AlinkMsgRecv_Pend(pAns_msg, timeout) == 0)
			{
				ret = -0;
			}
		}
	}
//	AlinkMsgSeamphoreGive(aLinkMsgSeamphore);
	return ret;
}

static void Timer_Callback(void)
{
	uRecvHeadCnt = 0;
	uRecvBufCnt = 0;
	uLen = 0;
}

static void AlinkMsgProc_Task(void *params)
{
	UNUSED(params);
	uint32_t sourceAddr = 0;
	alink_msg_t *pAlinkRecv_msg = NULL;
	alink_msg_t *pAlinkSend_msg = NULL;
	alink_ans_t ans_msg;
	uint8_t isNeed2InPassthrough = 0;

	MAV_COMPONENT enumSourceDevice = DEV_NONE;

	pAlinkSend_msg = GetAlinkSend_msg();
	while (1)
	{
		if (AlinkMsgProcTask_Pend(&sourceAddr, 500) == 0)
		{
			if (sourceAddr != 0)
			{
				pAlinkRecv_msg = (alink_msg_t *)sourceAddr;
				if (AlinkMsgProc_IsHeaderValid(&pAlinkRecv_msg->msg_head))
				{
					enumSourceDevice = (MAV_COMPONENT)pAlinkRecv_msg->msg_head.sourceid;
					switch (enumSourceDevice)
					{
					case DEV_PC:
					case DEV_TYPEC:
					{
						if ((pAlinkRecv_msg->msg_head.msgid & 0xF0) == 0xE0) /*Watch response message*/ //枪发起的
						{
							ans_msg.msg_len = (pAlinkRecv_msg->msg_head.len_hi << 8) + pAlinkRecv_msg->msg_head.len_lo;
							if (ans_msg.msg_len < ANS_RECV_MAX_BUFF)
							{
								memcpy(ans_msg.buffer, pAlinkRecv_msg->buffer, ans_msg.msg_len);
								AlinkMsgRecv_Post(&ans_msg);
							}
						}
						else
						{
							//服务应用层功能   由手表发起的
							if (pAlinkSend_msg)
							{
								memset((void *)pAlinkSend_msg, 0, sizeof(alink_msg_t));
								Alink_Server_Process(pAlinkRecv_msg, pAlinkSend_msg);
//								AlinkMsgSeamphoreTake(aLinkMsgSeamphore,ALINK_MSG_SEAMPHORE_DELY);
								AlinkMsgSend(params, pAlinkSend_msg);
//								AlinkMsgSeamphoreGive(aLinkMsgSeamphore);
//								AlinkMsgBtSend(params, pAlinkSend_msg);
							}
						}
					}
					break;

					case DEV_C2_BLE:
					{
//						if ( ((pAlinkRecv_msg->msg_head.msgid & 0x0F) == C2_CMD_UPLOAD)  || ((pAlinkRecv_msg->msg_head.msgid & 0xF0) == C2_CMD_UPLOAD_IP_PORT_AND_GET_CHANNE) ) /*C2 BLE/WIFI response message*/  //枪发起的
						if ((pAlinkRecv_msg->msg_head.msgid & 0xF0) == CMD_TCP_WIFI_INFO) /*C2 BLE response message*/  //枪发起的
						{
							ans_msg.msg_len = (pAlinkRecv_msg->msg_head.len_hi << 8) + pAlinkRecv_msg->msg_head.len_lo;
							if (ans_msg.msg_len < ANS_RECV_MAX_BUFF)
							{
								memcpy(ans_msg.buffer, pAlinkRecv_msg->buffer, ans_msg.msg_len);
								AlinkMsgRecv_Post(&ans_msg);
							}
						}
						else
						{
							if (pAlinkSend_msg)
							{
								memset((void *)pAlinkSend_msg, 0, sizeof(alink_msg_t));
								Alink_Server_Process(pAlinkRecv_msg, pAlinkSend_msg);
//								Alink_C2_Server_Process(pAlinkRecv_msg, pAlinkSend_msg, &isNeed2InPassthrough);
//								AlinkMsgSeamphoreTake(aLinkMsgSeamphore,ALINK_MSG_SEAMPHORE_DELY);
								//AlinkMsgResponseBtSend(params, pAlinkSend_msg);
								AlinkMsgSend(params, pAlinkSend_msg);
//								AlinkMsgSeamphoreGive(aLinkMsgSeamphore);
//								if (isNeed2InPassthrough)
//								{
//									//BTSendInPassthrough();
//									isNeed2InPassthrough = 0;
//								}
							}
						}

						break;
					}

					case DEV_C2:
					{
						if ( (pAlinkRecv_msg->msg_head.msgid & 0xF0) == C2_CMD_UPLOAD_IP_PORT_AND_GET_CHANNE ) /*C2 WIFI response message*/  //枪发起的
						{
							ans_msg.msg_len = (pAlinkRecv_msg->msg_head.len_hi << 8) + pAlinkRecv_msg->msg_head.len_lo;
							if (ans_msg.msg_len < ANS_RECV_MAX_BUFF)
							{
								memcpy(ans_msg.buffer, pAlinkRecv_msg->buffer, ans_msg.msg_len);
								AlinkMsgRecv_Post(&ans_msg);
							}
						}
						else
						{
							if (pAlinkSend_msg)
							{
								memset((void *)pAlinkSend_msg, 0, sizeof(alink_msg_t));
								Alink_Server_Process(pAlinkRecv_msg, pAlinkSend_msg);
//								AlinkMsgSeamphoreTake(aLinkMsgSeamphore,ALINK_MSG_SEAMPHORE_DELY);
								AlinkMsgSend(params, pAlinkSend_msg);
//								AlinkMsgSeamphoreGive(aLinkMsgSeamphore);
							}
						}

						break;
					}

					case DEV_RADAR_C2:
					{
					}
					break;
					default:
						break;
					}
				}
			}
		}
	}
}




/**
 *  @b Description
 *  @n
 *      Init alink message process module.
 *
 *  @retval
 *      Success    - 0
 *      Fail       - others
 */
int32_t AlinkMsgProc_Init(void)
{
	int32_t ret = RET_OK;
	TaskHandle_t task_hdl = NULL;
	int32_t *pvTimerID = NULL;

	xAlinkQueue = xQueueCreate(2, sizeof(uint32_t));

	xAlinkSendRecvQueue = xQueueCreate(2, sizeof(alink_ans_t));

	pvTimerID = (int32_t *)1;
	Timer_Handle = xTimerCreate((const char *)"Timer",
								(TickType_t)10,
								(UBaseType_t)pdFALSE,
								(void *)pvTimerID,
								(TimerCallbackFunction_t)Timer_Callback);
	if (Timer_Handle == NULL)
	{
		LOG_ERROR("create Timer error\r\n");
		ret = -1;
		return ret;
	}


	aLinkMsgSeamphore = xSemaphoreCreateMutex();
	if (aLinkMsgSeamphore == NULL)
	{
		LOG_ERROR("create seamphore error\r\n");
		ret = -1;
		return ret;
	}

//	ret = Axi_UartLite_Init(WATCH_UARTLITE_NUM, (XUartLite_Handler)AlinkMsg_UartLiteRecvHandler);
//	if (ret != RET_OK)
//	{
//		ret = RET_SYSTEM_ERR;
//		return ret;
//	}

	//	ret = Axi_UartLite_Init(WATCH_UARTLITE_NUM, (XUartLite_Handler)AlinkMsg_UartLiteRecvHandler);
	//	if (ret != RET_OK)
	//	{
	//		ret = RET_SYSTEM_ERR;
	//		return ret;
	//	}
	//	UartLite_RevData(WATCH_UARTLITE_NUM, Get_RevBuffPoint(), 1024);
	//    ret = UartAdp_Init();
	//    if (ret != RET_OK)
	//    {
	//        goto out;
	//    }
//	test();
	//	if (pdPASS != xTaskCreate(AlinkMsgProc_Task, "alinkTsk", TASK_STACK_SIZE_ALINKMSGPROC, (void *)1, TASK_PRI_ALINKMSGPROC, &task_hdl))
	if (pdPASS != xTaskCreate(AlinkMsgProc_Task, "alinkTsk", TASK_STACK_SIZE_ALINKMSGPROC, (void *)0, 7, &task_hdl))
	//	if (pdPASS != xTaskCreate(test, "alinkTsk", TASK_STACK_SIZE_ALINKMSGPROC, (void *)0, 7, &task_hdl))
	{
		ret = RET_SYSTEM_ERR;
		goto out;
	}


//	HeartBeatHandle_Task();

	UNUSED(task_hdl);

out:
	return ret;
}

void test()
{
//	uint16_t crc = 0;
//	uint8_t checksum = 0;
//	uint8_t buf[31] = {0xFD, 0x14, 0x00, 0x00, 0x01, 0x02, 0xC3, 0x01, 0xD7, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0x11, 0x22, 0x33,
//					   0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0x88, 0x77, 0x6A, 0x07};
//
//	uint8_t setmodeheader[9] = {0xFD, 0x01, 0x00, 0x00, 0x01, 0x02, 0x60, 0x01, 0x61};
//	uint8_t setmodecmd[12] = {0xFD, 0x01, 0x00, 0x00, 0x01, 0x02, 0x60, 0x01, 0x61, 0x01, 0x7a, 0xbf};
//
//	uint8_t getmodeheader[9] = {0xFD, 0x00, 0x00, 0x00, 0x01, 0x02, 0x61, 0x01, 0x61};
//	uint8_t getmodecmd[11] = {0xFD, 0x00, 0x00, 0x00, 0x01, 0x02, 0x61, 0x01, 0x61, 0x78, 0x7a};
//
//	uint8_t getswitchheader[9] = {0xFD, 0x00, 0x00, 0x00, 0x01, 0x02, 0x62, 0x01, 0x62};
//	uint8_t getswitchcmd[11] = {0xFD, 0x00, 0x00, 0x00, 0x01, 0x02, 0x62, 0x01, 0x62, 0x87, 0xa7};
//
//	uint8_t ctrgnssheader[9] = {0xFD, 0x01, 0x00, 0x00, 0x01, 0x02, 0x63, 0x01, 0x64};
//	uint8_t ctrgnsscmd[12] = {0xFD, 0x01, 0x00, 0x00, 0x01, 0x02, 0x63, 0x01, 0x64, 0x01, 0x0f, 0xe4};
//
//	uint8_t sethitfreqheader[9] = {0xFD, 0x01, 0x00, 0x00, 0x01, 0x02, 0x64, 0x01, 0x65};
//	uint8_t sethitfreqcmd[12] = {0xFD, 0x01, 0x00, 0x00, 0x01, 0x02, 0x64, 0x01, 0x65, 0x01, 0xf6, 0xaa};
//
//	uint8_t setchanvoltheader[9] = {0xFD, 0x06, 0x00, 0x00, 0x01, 0x02, 0x65, 0x01, 0x6b};
//	uint8_t setchanvoltcmd[17] = {0xFD, 0x06, 0x00, 0x00, 0x01, 0x02, 0x65, 0x01, 0x6b, 0x01, 0x00, 0x06, 0x04, 0xda, 0x07, 0xf9, 0x9f};
//
//	uint8_t getchanvoltheader[9] = {0xFD, 0x00, 0x00, 0x00, 0x01, 0x02, 0x66, 0x01, 0x66};
//	uint8_t getchanvoltcmd[11] = {0xFD, 0x00, 0x00, 0x00, 0x01, 0x02, 0x66, 0x01, 0x66, 0xc2, 0x82};
//
//	// set board SN cmd, (SN = GB122241080032)
//	uint8_t seBoardSNHeader[9] = {0xFD, 0x0E, 0x00, 0x00, 0x01, 0x04, 0x80, 0x01, 0x90};
//	uint8_t setBoardSNcmd[25] = {0xFD, 0x0E, 0x00, 0x00, 0x01, 0x04, 0x80, 0x01, 0x90, 'G', 'B', '1', '2', '2', '2', '4', '1', '0', '8', '0', '0', '3', '2', 0xf4, 0x4e};
//
//	// set board SN cmd, (SN = GB122241080032)
//	uint8_t seBoardSNHeader2[9] = {0xFD, 0x0E, 0x00, 0x00, 0x01, 0x04, 0x80, 0x01, 0x90};
//	uint8_t setBoardSNcmd2[25] = {0xFD, 0x0E, 0x00, 0x00, 0x01, 0x04, 0x80, 0x01, 0x90, 0x47, 0x42, 0x31, 0x32, 0x32, 0x32, 0x34, 0x31, 0x30, 0x38, 0x30, 0x30, 0x33, 0x32, 0xf4, 0x4e};
//
//	// get board SN cmd
//	uint8_t getBoardSNHeader[9] = {0xFD, 0x00, 0x00, 0x00, 0x01, 0x04, 0x81, 0x01, 0x83};
//	uint8_t getBoardSNcmd[11] = {0xFD, 0x00, 0x00, 0x00, 0x01, 0x04, 0x81, 0x01, 0x83, 0x5f, 0xfc};
//
//	// get software SN cmd
//	uint8_t getSoftwareSNHeader[9] = {0xFD, 0x00, 0x00, 0x00, 0x01, 0x04, 0x82, 0x01, 0x84};
//	uint8_t getSoftwareSNcmd[11] = {0xFD, 0x00, 0x00, 0x00, 0x01, 0x04, 0x82, 0x01, 0x84, 0x84, 0x67};
//
//	// get gnss state cmd
//	uint8_t getGNSSStateHeader[9] = {0xFD, 0x00, 0x00, 0x00, 0x01, 0x04, 0x69, 0x01, 0x6B};
//	uint8_t getGNSSStatecmd[11] = {0xFD, 0x00, 0x00, 0x00, 0x01, 0x04, 0x69, 0x01, 0x6B, 0x7A, 0x58};
//
//	// get hit freq cmd
//	uint8_t getHitFreqHeader[9] = {0xFD, 0x00, 0x00, 0x00, 0x01, 0x04, 0x68, 0x01, 0x6A};
//	uint8_t getHitFreqCmd[11] = {0xFD, 0x00, 0x00, 0x00, 0x01, 0x04, 0x68, 0x01, 0x6A, 0x2F, 0x13};

//	uint8_t setBluetStatusHeader[9] = {0xFD, 0x01, 0x00, 0x00, 0x01, 0x02, 0xc5, 0x01, 0xc6};
//	uint8_t setBluetStatusCmd[12] = {0xFD, 0x01, 0x00, 0x00, 0x01, 0x02, 0xc5, 0x01, 0xc6, 0x01, 0xe7, 0x91};
//
//	uint8_t getDeviceInfoHeader[9] = {0xFD, 0x00, 0x00, 0x00, 0x01, 0x02, 0xc6, 0x01, 0xc6};
//	uint8_t getDeviceInfoCmd[11] = {0xFD, 0x00, 0x00, 0x00, 0x01, 0x02, 0xc6, 0x01, 0xc6, 0x1f, 0x28};
//
//	uint8_t getC2HitFreqHeader[9] = {0xFD, 0x01, 0x00, 0x00, 0x01, 0x02, 0xc7, 0x01, 0xc8};
//	uint8_t getC2HitFreqCmd[12] = {0xFD, 0x01, 0x00, 0x00, 0x01, 0x02, 0xc7, 0x01, 0xc8, 0x01, 0x81, 0x32};

//		uint8_t setSourceIdHeader[9] = {0xFD, 0x01, 0x00, 0x00, 0x01, 0x05, 0x04, 0x01, 0x08};
//		uint8_t setSourceIdCmd[12] = {0xFD, 0x01, 0x00, 0x00, 0x01, 0x05, 0x04, 0x01, 0x08, 0x21, 0xe1, 0xf7};


//	//(AlinkMsg_IsCrcValid(buf, 31, 0));
//	while (1)
//	{
//		// for (uint8_t i = 0; i < 6; i++)
//		//{
//		//	RecvBuffer[0] = buf[i];
//		//	memcpy(RecvBuffer, buf + 5 * i, 5);
//		//	UartLiteRecvHandler(5);
//		// }
//		// memcpy(RecvBuffer, buf + 30, 1);
//		// UartLiteRecvHandler(1);
//
//		//		 for (uint8_t i = 0; i < 31; i++)
//		//		{
//		//			memcpy(RecvBuffer, buf + 1 * i, 1);
//		//			UartLiteRecvHandler(1);
//		//		 }
//
//		// for (uint8_t i = 0; i < 3; i++)
//		//{
//		//	memcpy(RecvBuffer, buf + 10 * i, 10);
//		//	UartLiteRecvHandler(10);
//		// }
//		// memcpy(RecvBuffer, buf + 30, 1);
//		// UartLiteRecvHandler(1);
//
//		//		for (uint8_t i = 0; i < 1; i++)
//		//		{
//		//			memcpy(RecvBuffer, buf, 31);
//		//			UartLiteRecvHandler(31);
//		//		}
//
//		checksum = AlinkMsgProc_GetHeaderChecksum((alink_msg_head_t *)setSourceIdHeader);
//		LOG_INFO("checksum = %x\r\n", checksum);
//		AlinkMsg_IsCrcValid((uint8_t *)setSourceIdCmd, 12, crc);
//		LOG_INFO("crc = %x\r\n", crc);
//		vTaskDelay(1000);
//		//		Timer_Callback();
//	}
}

void AlinkMsg_UartLiteRecvHandler(void *CallBackRef, uint32_t EventData)
{
	UartLiteRecvHandler(CallBackRef, EventData);
}

uint8_t *Get_RevBuffPoint()
{
	return RecvBuffer;
}
