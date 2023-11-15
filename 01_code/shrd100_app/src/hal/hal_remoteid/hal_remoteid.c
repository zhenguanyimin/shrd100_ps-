#include "../hal_remoteid/hal_remoteid.h"

#include "FreeRTOS.h"
#include "task.h"
#include <string.h>
#include <stdint.h>

#include "../../drv/uart_init/uartps_intr.h"
#include "../../drv/uart_init/uart_init.h"

static RemoteID_DataInfo_t remotetd_recvinfo = {0};
extern void UartPsMsgProcTask_Post(RemoteID_DataInfo_t* remotetd_recvinfo);


static uint8_t UartRecvTimeCount = 0;
static uint16_t UartRecvDataCount = 0;
static uint8_t UartProcRecvBuff[5 * 1024];  // 处理 数据
int UartPsRevProcess(uint8_t *databuff, uint32_t len)
{
	memcpy(UartProcRecvBuff + UartRecvDataCount, databuff, len);
	UartRecvDataCount += len;
	UartRecvTimeCount++;
	if (UartRecvTimeCount >= 3)
	{
		remotetd_recvinfo.databuff = UartProcRecvBuff;
		remotetd_recvinfo.datalen = UartRecvDataCount;
		UartRecvTimeCount = 0;
		UartRecvDataCount = 0;
		UartPsMsgProcTask_Post(&remotetd_recvinfo);
	}

	return 0;
}

