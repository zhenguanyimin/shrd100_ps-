
#include "protocol_common.h"

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

/* lwIP core includes */
#include "lwip/opt.h"
#include "lwip/sockets.h"
#include "lwip/err.h"
#include "lwip/sys.h"

#include <sys/select.h>

/* Utils includes */
#include "sleep.h"

#include "protocol_parameter.h"
#include "protocol_ack.h"

#define PROTOCOL_COMMOM_MSGQ_LEN (20)

typedef enum protocol_tcp_proc_msg_ret
{
	TCP_MSG_PROC_RET_CONTINUE = 0,
	TCP_MSG_PROC_RET_FINISH = 1,
	TCP_MSG_PROC_RET_ERROR = 2
} protocol_tcp_proc_msg_ret_t;

typedef struct protocol_common_msg
{
	void *data;
	uint32_t len;
	send_data_cb_fp cb_fp;
} protocol_common_msg_t;

typedef struct protocol_udp_mcb
{
	int32_t sockFd;
	sys_thread_t sendTskHdl; // TaskHandle_t
	sys_thread_t recvTskHdl; // TaskHandle_t
	SemaphoreHandle_t lock;
	QueueHandle_t msgQ;
	protocol_packet_head_t packetHead;
} protocol_udp_mcb_t;

typedef struct protocol_tcp_mcb
{
	sys_thread_t tskHdl; // TaskHandle_t
	union
	{
		protocol_cfg_param_t cfg_param;
	} buf;
	uint32_t rcvIdx;
} protocol_tcp_mcb_t;

typedef struct protocol_common_mcb
{
	bool inited;
	protocol_udp_mcb_t udpMcb;
	protocol_tcp_mcb_t tcpMcb;
} protocol_common_mcb_t;

STATIC protocol_common_mcb_t g_protocolCommonMcb;

PRIVATE ret_code_t protocol_udp_send_data(void *data, uint32_t len)
{
	ret_code_t ret = RET_OK;
	int32_t sockFd = -1;
	struct sockaddr_in remoteAddr = {0};
	int32_t sendLen = 0;
	int32_t totalLen = 0;
	uint16_t currLen = 0;
	uint8_t buf[UDP_PACKET_LEN_MAX] = {0};
	protocol_packet_head_t pktHead = {0};
	protocol_info_head_t *infoHead = (protocol_info_head_t *)data;
	uint8_t *posIdx = (uint8_t *)data;
	uint16_t infoPacketNum = 0;
	uint16_t curInfoPacketOrder = 0;
	uint16_t cnt = 0;

	sockFd = g_protocolCommonMcb.udpMcb.sockFd;
	if ((!g_protocolCommonMcb.inited) || (sockFd < 0))
	{
		ret = RET_SYSTEM_ERR;
		goto out;
	}

	if (sizeof(pktHead) != UDP_PACKET_HEAD_LEN)
	{
		ret = RET_SYSTEM_ERR;
		goto out;
	}

	remoteAddr.sin_family = AF_INET;
	remoteAddr.sin_port = htons(UDP_COM_PORT_REMOTE);
	inet_aton(IP_ADDR_REMOTE, &remoteAddr.sin_addr);

	if (xSemaphoreTake(g_protocolCommonMcb.udpMcb.lock, portMAX_DELAY))
	{
		memcpy(&pktHead, &g_protocolCommonMcb.udpMcb.packetHead, sizeof(pktHead));
		pktHead.packetFlag = htonl(PACKET_HEAD_FLAG);
		pktHead.terminalType = infoHead->terminalType;
		pktHead.subTerminalType = infoHead->subTerminalType;
		pktHead.infoType = infoHead->infoType;
		infoPacketNum = (len % UDP_INFO_LEN_MAX == 0) ? (len / UDP_INFO_LEN_MAX) : (len / UDP_INFO_LEN_MAX) + 1;
		pktHead.infoPacketNum = htons(infoPacketNum);

		totalLen = len;
		while (totalLen > 0)
		{
			currLen = totalLen > UDP_INFO_LEN_MAX ? UDP_INFO_LEN_MAX : totalLen;
			pktHead.totalPacketNum = htons(g_protocolCommonMcb.udpMcb.packetHead.totalPacketNum);
			pktHead.packetLength = htons(sizeof(pktHead) + currLen);
			pktHead.sendInfoCount = htons(g_protocolCommonMcb.udpMcb.packetHead.sendInfoCount);
			pktHead.curInfoPacketOrder = htons(curInfoPacketOrder);
			memset(buf, 0, sizeof(buf));
			memcpy(buf, &pktHead, sizeof(pktHead));
			memcpy(buf + sizeof(pktHead), posIdx, currLen);
			sendLen = lwip_sendto(sockFd, buf, sizeof(pktHead) + currLen, 0, (struct sockaddr *)&remoteAddr, sizeof(remoteAddr));
			if (sendLen != (sizeof(pktHead) + currLen))
			{
				LOG_VERBOSE("[%s:%d] sendLen=%d not same with currLen=%d\r\n", __FUNCTION__, __LINE__, sendLen, sizeof(pktHead) + currLen);
			}
			totalLen -= currLen;
			posIdx += currLen;
			g_protocolCommonMcb.udpMcb.packetHead.totalPacketNum++;
			curInfoPacketOrder++;
			// [Begin] This is an avoidance method to let DMA IRQ work properly
			// vTaskDelay(2);
			cnt++;
			if (cnt >= 10)
			{
				vTaskDelay(1);
				cnt = 0;
			}
			else
			{
				usleep(100);
			}
			// [End] This is an avoidance method to let DMA IRQ work properly
		}

		g_protocolCommonMcb.udpMcb.packetHead.sendInfoCount++;
		xSemaphoreGive(g_protocolCommonMcb.udpMcb.lock);
	}

out:
	return ret;
}

ret_code_t protocol_udp_send_data2(void *data, uint32_t len)
{
	ret_code_t ret = RET_OK;
	int32_t sockFd = -1;
	struct sockaddr_in remoteAddr = {0};
	int32_t sendLen = 0;
	int32_t totalLen = 0;
	uint16_t currLen = 0;
	uint8_t buf[UDP_PACKET_LEN_MAX] = {0};
	uint8_t *posIdx = (uint8_t *)data;
	uint16_t curInfoPacketOrder = 0;
	uint16_t cnt = 0;

	sockFd = g_protocolCommonMcb.udpMcb.sockFd;
	if ((!g_protocolCommonMcb.inited) || (sockFd < 0))
	{
		ret = RET_SYSTEM_ERR;
		goto out;
	}

	remoteAddr.sin_family = AF_INET;
	remoteAddr.sin_port = htons(UDP_COM_PORT_REMOTE);
	inet_aton(IP_ADDR_REMOTE, &remoteAddr.sin_addr);

	if (xSemaphoreTake(g_protocolCommonMcb.udpMcb.lock, portMAX_DELAY))
	{
		totalLen = len;
		while (totalLen > 0)
		{
			currLen = totalLen > UDP_PACKET_LEN_MAX ? UDP_PACKET_LEN_MAX : totalLen;
			memset(buf, 0, sizeof(buf));
			memcpy(buf, posIdx, currLen);
			sendLen = lwip_sendto(sockFd, buf, currLen, 0, (struct sockaddr *)&remoteAddr, sizeof(remoteAddr));
			if (sendLen != currLen)
			{
				LOG_VERBOSE("[%s:%d] sendLen=%d not same with currLen=%d\r\n", __FUNCTION__, __LINE__, sendLen, currLen);
			}
			totalLen -= currLen;
			posIdx += currLen;
			g_protocolCommonMcb.udpMcb.packetHead.totalPacketNum++;
			curInfoPacketOrder++;
			// [Begin] This is an avoidance method to let DMA IRQ work properly
			// vTaskDelay(2);
			cnt++;
			if (cnt >= 1)
			{
				vTaskDelay(1);
//				usleep(10);
				cnt = 0;
			}
			else
			{
//				usleep(100);
			}
			// [End] This is an avoidance method to let DMA IRQ work properly
		}

		xSemaphoreGive(g_protocolCommonMcb.udpMcb.lock);
	}

out:
	return ret;
}

PRIVATE void protocol_udp_send_thread(void *p_arg)
{
	BaseType_t osRet = pdTRUE;
	protocol_common_msg_t msg = {0};
	ret_code_t ret = RET_OK;

	while (!g_protocolCommonMcb.inited)
	{
		vTaskDelay(1);
	}

	while (1)
	{
		memset(&msg, 0, sizeof(msg));
		osRet = xQueueReceive(g_protocolCommonMcb.udpMcb.msgQ, &msg, portMAX_DELAY);
		if (osRet)
		{
			ret = protocol_udp_send_data(msg.data, msg.len);
			if (msg.cb_fp != NULL)
			{
				msg.cb_fp(msg.data, msg.len, (uint32_t)ret);
			}
		}
		else
		{
			LOG_ERROR("[%s:%d] Unexpected error", __FUNCTION__, __LINE__);
		}
	}
}

PRIVATE void protocol_udp_recv_thread(void *p_arg)
{
	struct sockaddr_in remoteAddr = {0};
	socklen_t addrLen = 0;
	int32_t recvLen = 0;
	int32_t sockFd = 0;
	uint8_t buf[UDP_PACKET_LEN_MAX] = {0};

	while (!g_protocolCommonMcb.inited)
	{
		vTaskDelay(1);
	}
	sockFd = g_protocolCommonMcb.udpMcb.sockFd;

	while (1)
	{
		// Note: the udp server is not needed anymore
		vTaskDelay(1);

		memset(buf, 0, sizeof(buf));
		addrLen = sizeof(remoteAddr);
		recvLen = lwip_recvfrom(sockFd, buf, sizeof(buf), 0, (struct sockaddr *)&remoteAddr, &addrLen);
		if (recvLen > 0)
		{
			LOG_VERBOSE("[%s:%d] recvLen=%lu from %s %u\r\n",
						__FUNCTION__, __LINE__, recvLen, inet_ntoa(remoteAddr.sin_addr.s_addr), ntohs(remoteAddr.sin_port));
		}
	}
}

PRIVATE void protocol_udp_deinit(void)
{
	if (g_protocolCommonMcb.udpMcb.msgQ != NULL)
	{
		vQueueDelete(g_protocolCommonMcb.udpMcb.msgQ);
		g_protocolCommonMcb.udpMcb.msgQ = NULL;
	}

	if (g_protocolCommonMcb.udpMcb.lock != NULL)
	{
		vSemaphoreDelete(g_protocolCommonMcb.udpMcb.lock);
		g_protocolCommonMcb.udpMcb.lock = NULL;
	}

	if (g_protocolCommonMcb.udpMcb.recvTskHdl != NULL)
	{
		vTaskDelete(g_protocolCommonMcb.udpMcb.recvTskHdl);
		g_protocolCommonMcb.udpMcb.recvTskHdl = NULL;
	}

	if (g_protocolCommonMcb.udpMcb.sendTskHdl != NULL)
	{
		vTaskDelete(g_protocolCommonMcb.udpMcb.sendTskHdl);
		g_protocolCommonMcb.udpMcb.sendTskHdl = NULL;
	}

	if (g_protocolCommonMcb.udpMcb.sockFd >= 0)
	{
		lwip_close(g_protocolCommonMcb.udpMcb.sockFd);
		g_protocolCommonMcb.udpMcb.sockFd = -1;
	}

	return;
}

PRIVATE ret_code_t protocol_udp_init(void)
{
	ret_code_t ret = RET_OK;
	int32_t sockFd = -1;
	int32_t sockRet = 0;
	struct sockaddr_in svrAddr = {0};
	SemaphoreHandle_t lock = NULL;
	QueueHandle_t msgQ = NULL;
	sys_thread_t sendTskHdl = NULL;
	sys_thread_t recvTskHdl = NULL;

	sockFd = lwip_socket(AF_INET, SOCK_DGRAM, 0);
	if (sockFd < 0)
	{
		ret = RET_NO_RESOURCE;
		goto out;
	}

	svrAddr.sin_family = AF_INET;
	svrAddr.sin_port = htons(UDP_COM_PORT_LOCAL);
	inet_aton(IP_ADDR_LOCAL, &svrAddr.sin_addr);

	sockRet = lwip_bind(sockFd, (struct sockaddr *)&svrAddr, sizeof(svrAddr));
	if (sockRet < 0)
	{
		LOG_ERROR("cannot bind!\r\n");
		ret = RET_SYSTEM_ERR;
		goto out;
	}

	sendTskHdl = sys_thread_new("PROT_T_UDP", protocol_udp_send_thread, NULL, TASK_STACK_SIZE_PTOTOCOL_UDP, TASK_PRI_PTOTOCOL_UDP);
	if (sendTskHdl == NULL)
	{
		ret = RET_NO_RESOURCE;
		goto out;
	}

	recvTskHdl = sys_thread_new("PROT_R_UDP", protocol_udp_recv_thread, NULL, TASK_STACK_SIZE_PTOTOCOL_UDP, TASK_PRI_PTOTOCOL_UDP);
	if (recvTskHdl == NULL)
	{
		ret = RET_NO_RESOURCE;
		goto out;
	}

	lock = xSemaphoreCreateMutex();
	if (lock == NULL)
	{
		ret = RET_NO_RESOURCE;
		goto out;
	}

	msgQ = xQueueCreate(PROTOCOL_COMMOM_MSGQ_LEN, sizeof(protocol_common_msg_t));
	if (msgQ == NULL)
	{
		ret = RET_NO_RESOURCE;
		goto out;
	}

	g_protocolCommonMcb.udpMcb.msgQ = msgQ;
	g_protocolCommonMcb.udpMcb.lock = lock;
	g_protocolCommonMcb.udpMcb.recvTskHdl = recvTskHdl;
	g_protocolCommonMcb.udpMcb.sendTskHdl = sendTskHdl;
	g_protocolCommonMcb.udpMcb.sockFd = sockFd;

out:
	if (ret != RET_OK)
	{
		if (msgQ != NULL)
		{
			vQueueDelete(msgQ);
			msgQ = NULL;
		}

		if (lock != NULL)
		{
			vSemaphoreDelete(lock);
			lock = NULL;
		}

		if (recvTskHdl != NULL)
		{
			vTaskDelete(recvTskHdl);
			recvTskHdl = NULL;
		}

		if (sendTskHdl != NULL)
		{
			vTaskDelete(sendTskHdl);
			sendTskHdl = NULL;
		}

		if (sockFd >= 0)
		{
			lwip_close(sockFd);
			sockFd = -1;
		}
	}

	LOG_DEBUG("[%s:%d] ret=%d\r\n", __FUNCTION__, __LINE__, ret);
	return ret;
}

PRIVATE int32_t protocol_tcp_proc_msg(void)
{
	int32_t ret = TCP_MSG_PROC_RET_CONTINUE;
	uint16_t crcCalc = 0;
	uint8_t *buf = (uint8_t *)&g_protocolCommonMcb.tcpMcb.buf;
	protocol_info_head_t *head = (protocol_info_head_t *)&g_protocolCommonMcb.tcpMcb.buf;
//	protocol_info_tail_t *tail = NULL;
	protocol_info_tail_t mtail;
	uint16_t uIndex = 0;
	ret_code_t funcRet = RET_OK;

	/* check head of the message */
	if (g_protocolCommonMcb.tcpMcb.rcvIdx < sizeof(protocol_info_head_t))
	{
		ret = TCP_MSG_PROC_RET_CONTINUE;
		goto out;
	}

	if (head->infoSync != ntohl(INFO_HEAD_FLAG))
	{
		ret = TCP_MSG_PROC_RET_ERROR;
		goto out;
	}

	if (ntohl(head->infoLength) <= (TCP_INFO_HEAD_LEN + TCP_INFO_TAIL_LEN))
	{
		ret = TCP_MSG_PROC_RET_CONTINUE;
		goto out;
	}

	if (g_protocolCommonMcb.tcpMcb.rcvIdx < ntohl(head->infoLength))
	{
		ret = TCP_MSG_PROC_RET_CONTINUE;
		goto out;
	}

	/* check tail of the message */
//	tail = (protocol_info_tail_t *)(buf + ntohl(head->infoLength) - TCP_INFO_TAIL_LEN);
	uIndex = ntohl(head->infoLength) - TCP_INFO_TAIL_LEN;
	mtail.reserved = buf[uIndex] + (buf[uIndex + 1] << 8);
	mtail.crc = buf[uIndex + 2] + (buf[uIndex + 3] << 8);
	crcCalc = crc_16bits_compute(buf, ntohl(head->infoLength) - TCP_INFO_TAIL_LEN);
	if (crcCalc != ntohs(mtail.crc))
	{
		LOG_DEBUG("[%s:%d] crcCalc=0x%x isn't same as crcRecv=0x%x\r\n",
				 __FUNCTION__, __LINE__, crcCalc, ntohs(mtail.crc));
		/* whether the crc is used is decided in the specific info processor */
	}

	/* check body of the message */
	if (ntohs(head->infoType) == PIT_CFG_PARAM)
	{
		funcRet = protocol_cfg_param_process_data(buf, ntohl(head->infoLength));
		ret = (funcRet == RET_OK) ? (TCP_MSG_PROC_RET_FINISH) : (TCP_MSG_PROC_RET_ERROR);
	}

out:
	return ret;
}

PRIVATE ret_code_t protocol_tcp_send_ack(int32_t sockFd, int32_t procRet)
{
	ret_code_t ret = RET_OK;
	protocol_ack_t ackMsg = {0};
	uint32_t result = 0;
	uint16_t crcCalc = 0;

	/* fill head */
	memcpy((uint8_t *)&ackMsg.stInfoHeader, (uint8_t *)(&g_protocolCommonMcb.tcpMcb.buf), sizeof(ackMsg.stInfoHeader));
	ackMsg.stInfoHeader.infoLength = htonl(sizeof(ackMsg));
	ackMsg.stInfoHeader.infoType = htons(PIT_ACK_INFO);
	ackMsg.stInfoHeader.infoVersion = PROTOCOL_ACK_VER;

	/* fill body */
	result = (procRet == TCP_MSG_PROC_RET_FINISH) ? 0 : 1;
	ackMsg.revResult = htonl(result);

	/* fill tail */
	crcCalc = crc_16bits_compute((uint8_t *)&ackMsg, sizeof(ackMsg) - TCP_INFO_TAIL_LEN);
	ackMsg.stInfoTail.crc = htons(crcCalc);

	ret = protocol_send_ack(sockFd, &ackMsg);

	return ret;
}

#define CREATE_TCP_SOCKET(sockFd)                      \
	if (sockFd >= 0)                                   \
	{                                                  \
		lwip_close(sockFd);                            \
		sockFd = -1;                                   \
	}                                                  \
	do                                                 \
	{                                                  \
		sockFd = lwip_socket(AF_INET, SOCK_STREAM, 0); \
		vTaskDelay(1000);                              \
	} while (sockFd < 0)

PRIVATE void protocol_tcp_thread(void *p_arg)
{
	struct sockaddr_in remoteAddr = {0};
	int32_t leftSpace = 0;
	int32_t recvLen = 0;
	int32_t sockFd = -1;
	int32_t sockRet = 0;
	fd_set readSet;
	struct timeval timeout;
	uint8_t *buf = (uint8_t *)&g_protocolCommonMcb.tcpMcb.buf;
	int32_t procRet = TCP_MSG_PROC_RET_CONTINUE;
	ret_code_t ackRet = RET_OK;

	while (!g_protocolCommonMcb.inited)
	{
		vTaskDelay(1);
	}
	CREATE_TCP_SOCKET(sockFd);

	remoteAddr.sin_family = AF_INET;
	remoteAddr.sin_port = htons(TCP_COM_PORT_REMOTE);
	remoteAddr.sin_addr.s_addr = inet_addr(IP_ADDR_REMOTE);
	// inet_aton(IP_ADDR_REMOTE, &remoteAddr.sin_addr);

	timeout.tv_sec = 60;
	timeout.tv_usec = 0;

	while (1)
	{
		// connect to the server
		sockRet = lwip_connect(sockFd, (struct sockaddr *)&remoteAddr, sizeof(remoteAddr));
		if (sockRet < 0)
		{
			// re-create the socket when failed
			CREATE_TCP_SOCKET(sockFd);
			continue;
		}

		FD_ZERO(&readSet);
		FD_SET(sockFd, &readSet);
		memset(&g_protocolCommonMcb.tcpMcb.buf, 0, sizeof(g_protocolCommonMcb.tcpMcb.buf));
		g_protocolCommonMcb.tcpMcb.rcvIdx = 0;
		buf = (uint8_t *)&g_protocolCommonMcb.tcpMcb.buf;

		while (1)
		{
			/* waiting for data */
			sockRet = lwip_select(sockFd + 1, &readSet, NULL, NULL, &timeout);
			if (sockRet == 0)
			{
				// timeout, try again
				FD_ZERO(&readSet);
				FD_SET(sockFd, &readSet);
				continue;
			}
			else if (sockRet < 0)
			{
				// error, reconnect
				LOG_ERROR("[%s:%d] sockRet=%d\r\n", __FUNCTION__, __LINE__, sockRet);
				break;
			}
			else
			{
				leftSpace = sizeof(g_protocolCommonMcb.tcpMcb.buf) - g_protocolCommonMcb.tcpMcb.rcvIdx;
				if (leftSpace < 1)
				{
					// no valid message, disconnect, reconnect
					LOG_DEBUG("[%s:%d] no more space\r\n", __FUNCTION__, __LINE__);
					break;
				}

				// read the data
				recvLen = lwip_recv(sockFd, buf, leftSpace, 0);
				if (recvLen > 0)
				{
					g_protocolCommonMcb.tcpMcb.rcvIdx += (uint32_t)recvLen;
					buf += recvLen;

					// check and process the data
					procRet = protocol_tcp_proc_msg();
					LOG_VERBOSE("[%s:%d] recvLen=%d, procRet=%d\r\n", __FUNCTION__, __LINE__, recvLen, procRet);
					if (procRet != TCP_MSG_PROC_RET_CONTINUE)
					{
						/* send response to the host */
						ackRet = protocol_tcp_send_ack(sockFd, procRet);
						if (ackRet == RET_OK)
						{
							memset(&g_protocolCommonMcb.tcpMcb.buf, 0, sizeof(g_protocolCommonMcb.tcpMcb.buf));
							g_protocolCommonMcb.tcpMcb.rcvIdx = 0;
							buf = (uint8_t *)&g_protocolCommonMcb.tcpMcb.buf;
							continue;
						}
						else
						{
							LOG_ERROR("[%s:%d] ackRet=%d\r\n", __FUNCTION__, __LINE__, ackRet);
							break;
						}
					}
				}
				else
				{
					// disconnect, reconnect
					LOG_DEBUG("[%s:%d] %d disconnect\r\n", __FUNCTION__, __LINE__, sockFd);
					break;
				}
			}
		}
	}
}

PRIVATE void protocol_tcp_deinit(void)
{
	if (g_protocolCommonMcb.tcpMcb.tskHdl != NULL)
	{
		vTaskDelete(g_protocolCommonMcb.tcpMcb.tskHdl);
		g_protocolCommonMcb.tcpMcb.tskHdl = NULL;
	}

	return;
}

PRIVATE ret_code_t protocol_tcp_init(void)
{
	ret_code_t ret = RET_OK;
	sys_thread_t tskHdl = NULL;

	tskHdl = sys_thread_new("PROT_TCP", protocol_tcp_thread, NULL, TASK_STACK_SIZE_PTOTOCOL_TCP, TASK_PRI_PTOTOCOL_TCP);
	if (tskHdl == NULL)
	{
		ret = RET_NO_RESOURCE;
		goto out;
	}

	g_protocolCommonMcb.tcpMcb.tskHdl = tskHdl;

out:
	if (ret != RET_OK)
	{
		if (tskHdl != NULL)
		{
			vTaskDelete(tskHdl);
			tskHdl = NULL;
		}
	}

	LOG_DEBUG("[%s:%d] ret=%d\r\n", __FUNCTION__, __LINE__, ret);
	return ret;
}

ret_code_t protocol_common_init(void)
{
	ret_code_t ret = RET_OK;

	if (g_protocolCommonMcb.inited)
	{
		ret = RET_OK;
		goto out;
	}

	memset(&g_protocolCommonMcb, 0, sizeof(g_protocolCommonMcb));
	g_protocolCommonMcb.udpMcb.sockFd = -1;

	ret = protocol_udp_init();
	if (ret != RET_OK)
	{
		goto out;
	}

	ret = protocol_tcp_init();
	if (ret != RET_OK)
	{
		goto out;
	}

	g_protocolCommonMcb.inited = true;

out:
	if (ret != RET_OK)
	{
		protocol_tcp_deinit();
		protocol_udp_deinit();
	}

	LOG_DEBUG("[%s:%d] ret=%d\r\n", __FUNCTION__, __LINE__, ret);
	return ret;
}

ret_code_t protocol_udp_send_data_blocking(void *data, uint32_t len)
{
	ret_code_t ret = RET_OK;

	//	LOG_DEBUG("[%s:%d] enter\r\n", __FUNCTION__, __LINE__);
	if ((data == NULL) || (len <= (UDP_INFO_HEAD_LEN + UDP_INFO_TAIL_LEN)))
	{
		ret = RET_INVALID_PARAM;
		goto out;
	}

	ret = protocol_udp_send_data(data, len);
	if (ret != RET_OK)
	{
		LOG_ERROR("Send data failed %d\r\n", ret);
		goto out;
	}

out:
	return ret;
}

ret_code_t protocol_udp_send_data_nonblocking(void *data, uint32_t len, send_data_cb_fp cb_fp)
{
	ret_code_t ret = RET_OK;
	BaseType_t osRet = pdTRUE;
	protocol_common_msg_t msg = {0};

	//	LOG_DEBUG("[%s:%d] enter\r\n", __FUNCTION__, __LINE__);
	if ((data == NULL) || (len <= (UDP_INFO_HEAD_LEN + UDP_INFO_TAIL_LEN)))
	{
		ret = RET_INVALID_PARAM;
		goto out;
	}

	msg.data = data;
	msg.len = len;
	msg.cb_fp = cb_fp;

	osRet = xQueueSend(g_protocolCommonMcb.udpMcb.msgQ, &msg, 100); // portMAX_DELAY
	if (osRet != pdTRUE)
	{
		LOG_ERROR("Send msg to q failed %d.\r\n", osRet);
		ret = RET_SYSTEM_ERR;
	}

out:
	return ret;
}
