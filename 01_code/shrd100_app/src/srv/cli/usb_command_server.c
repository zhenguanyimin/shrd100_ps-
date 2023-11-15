/*
 * usb_command_server.c
 *
 *  Created on: 2023骞�3鏈�16鏃�
 *      Author: A19199
 */

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include <queue.h>

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../cfg/shrd_config.h"
#include "../../drv/usb/xusb_cdc_acm_api.h"

#include "cli_if.h"
#include "FreeRTOS_CLI.h"

static QueueHandle_t xUsbCommandServerQueue;

/* Dimensions the buffer into which input characters are placed. */
#define cmdMAX_INPUT_SIZE 100

/* Dimensions the buffer into which string outputs can be placed. */
#define cmdMAX_OUTPUT_SIZE 1024

static char cOutputString[cmdMAX_OUTPUT_SIZE];

void UsbCommandServer_Post(sUsbCommand_msg_t *msg)
{
	if (xUsbCommandServerQueue != NULL)
	{
		xQueueSend(xUsbCommandServerQueue, msg, 10);
	}
}

static int32_t UsbCommandServer_Pend(sUsbCommand_msg_t *msg, uint32_t timeout)
{

	int32_t result = -1;

	if (xUsbCommandServerQueue != NULL)
	{
		if (xQueueReceive(xUsbCommandServerQueue, msg, timeout))
		{
			result = 0;
		}
	}

	return result;
}

void UsbCommandServer_Task(void *p_arg)
{
	sUsbCommand_msg_t msg;

	while (1)
	{
		memset((uint8_t*)&msg, 0, sizeof(sUsbCommand_msg_t));
		if (UsbCommandServer_Pend(&msg, (uint32_t)portMAX_DELAY) == 0)
		{
			memset(cOutputString, 0, cmdMAX_OUTPUT_SIZE);
			FreeRTOS_CLIProcessCommand((char*)msg.buf, cOutputString, cmdMAX_OUTPUT_SIZE);
			UsbSendDataFunc((uint8_t*)cOutputString, strlen((const char *)cOutputString));
			UsbSendDataFunc((uint8_t*)"\r\n>", strlen("\r\n>"));
		}
	}
}

int32_t InitUsbCommandServer_Task()
{
	int32_t retVal = -1;

	xUsbCommandServerQueue = xQueueCreate(1, sizeof(sUsbCommand_msg_t));

	if (xTaskCreate(UsbCommandServer_Task, "UsbCommandServer", TASK_STACK_SIZE_USB_CMD, NULL, TASK_PRI_USB_CMD, NULL) == 1)
	{
		retVal = 0;
	}

	return retVal;
}
