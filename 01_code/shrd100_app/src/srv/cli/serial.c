/*
 * FreeRTOS V202112.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

/*
	BASIC INTERRUPT DRIVEN SERIAL PORT DRIVER.

	Note1:  This driver is used specifically to provide an interface to the
	FreeRTOS+CLI command interpreter.  It is *not* intended to be a generic
	serial port driver.  Nor is it intended to be used as an example of an
	efficient implementation.  In particular, a queue is used to buffer
	received characters, which is fine in this case as key presses arrive
	slowly, but a DMA and/or RAM buffer should be used in place of the queue in
	applications that expect higher throughput.

	Note2:  This driver does not attempt to handle UART errors.
*/

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* Demo application includes. */
#include "serial.h"

/* Xilinx includes. */
#include "xuartps.h"
#include "xscugic.h"
#include "xil_exception.h"

#include "xuartlite.h"

/* The UART interrupts of interest when receiving. */
#define serRECEIVE_INTERRUPT_MASK (XUARTPS_IXR_RXOVR | XUARTPS_IXR_RXFULL | XUARTPS_IXR_TOUT)

/* The UART interrupts of interest when transmitting. */
#define serTRANSMIT_IINTERRUPT_MASK (XUARTPS_IXR_TXEMPTY)

/*-----------------------------------------------------------*/

/* The UART being used. */
static XUartPs xUARTInstance;

/* The interrupt controller, which is configred by the hardware setup routines
defined in main(). */
extern XScuGic xInterruptController;

/* The queue into which received key presses are placed.  NOTE THE COMMENTS AT
THE TOP OF THIS FILE REGARDING THE USE OF QUEUES FOR THIS PURPOSE. */
static QueueHandle_t xRxQueue = NULL;

/* The semaphore used to indicate the end of a transmission. */
static SemaphoreHandle_t xTxCompleteSemaphore = NULL;

static XUartLite gLogUart;
static u8 gUartPort = 1;
static u8 gdata[10] = {0};
/*-----------------------------------------------------------*/

/*
 * The UART interrupt handler is defined in this file to provide more control,
 * but still uses parts of the Xilinx provided driver.
 */
void prvUART_Handler(void *pvNotUsed);

void vLogUartRev_Handler(void *CallBackRef, unsigned int ByteCount);

void vLogUartSend_Handler(void *CallBackRef, unsigned int ByteCount);

int LogUartInit(void);

/*-----------------------------------------------------------*/

/*
 * See the serial2.h header file.
 */
xComPortHandle xSerialPortInitMinimal(uint32_t ulWantedBaud, UBaseType_t uxQueueLength)
{
	BaseType_t xStatus;
	XUartPs_Config *pxConfig;
	uint8_t *pTempPointer = NULL;

	/* Create the queue used to hold received characters.  NOTE THE COMMENTS AT
	THE TOP OF THIS FILE REGARDING THE USE OF QUEUES FOR THIS PURPOSE. */
	xRxQueue = xQueueCreate(uxQueueLength, sizeof(char));
	pTempPointer = (uint8_t *)xRxQueue;
	configASSERT((int32_t)pTempPointer);

	/* Create the semaphore used to signal the end of a transmission, then take
	the semaphore so it is in the correct state the first time
	xSerialSendString() is called.  A block time of zero is used when taking
	the semaphore as it is guaranteed to be available (it was just created). */
	xTxCompleteSemaphore = xSemaphoreCreateBinary();
	pTempPointer = (uint8_t *)xTxCompleteSemaphore;
	configASSERT((int32_t)pTempPointer);
	xSemaphoreTake(xTxCompleteSemaphore, 0);

	/* Look up the UART configuration then initialise the dirver. */
	pxConfig = XUartPs_LookupConfig(XPAR_XUARTPS_0_DEVICE_ID);

	/* Initialise the driver. */
	xStatus = XUartPs_CfgInitialize(&xUARTInstance, pxConfig, XPAR_PS7_UART_BASEADDR);
	configASSERT(xStatus == XST_SUCCESS);
	(void)xStatus; /* Remove compiler warning if configASSERT() is not defined. */

	/* Misc. parameter configuration. */
	XUartPs_SetBaudRate(&xUARTInstance, ulWantedBaud);
	XUartPs_SetOperMode(&xUARTInstance, XUARTPS_OPER_MODE_NORMAL);

	/* Install the interrupt service routine that is defined within this
	file. */
	xStatus = XScuGic_Connect(&xInterruptController, XPAR_XUARTPS_0_INTR, (Xil_ExceptionHandler)prvUART_Handler, (void *)&xUARTInstance);
	configASSERT(xStatus == XST_SUCCESS);
	(void)xStatus; /* Remove compiler warning if configASSERT() is not defined. */

	/* Ensure interrupts start clear. */
	XUartPs_WriteReg(XPAR_PS7_UART_BASEADDR, XUARTPS_ISR_OFFSET, XUARTPS_IXR_MASK);

	/* Enable the UART interrupt within the GIC. */
	XScuGic_Enable(&xInterruptController, XPAR_XUARTPS_0_INTR);

	/* Enable the interrupts of interest in the UART. */
	XUartPs_SetInterruptMask(&xUARTInstance, XUARTPS_IXR_RXFULL | XUARTPS_IXR_RXOVR | XUARTPS_IXR_TOUT | XUARTPS_IXR_TXEMPTY);

	/* Set the receive timeout. */
	XUartPs_SetRecvTimeout(&xUARTInstance, 8);

	LogUartInit();

	return (xComPortHandle)0;
}
/*-----------------------------------------------------------*/

BaseType_t xSerialGetChar(xComPortHandle pxPort, signed char *pcRxedChar, TickType_t xBlockTime)
{
	BaseType_t xReturn;

	/* Only a single port is supported. */
	(void)pxPort;

	/* Obtain a received character from the queue - entering the Blocked state
	(so not consuming any processing time) to wait for a character if one is not
	already available. */
	xReturn = xQueueReceive(xRxQueue, pcRxedChar, xBlockTime);
	return xReturn;
}
/*-----------------------------------------------------------*/

void vSerialPutString(xComPortHandle pxPort, const signed char *const pcString, unsigned short usStringLength)
{
	const TickType_t xMaxWait = 200UL / portTICK_PERIOD_MS;

	/* Only a single port is supported. */
	(void)pxPort;

	/* Start the transmission.  The interrupt service routine will complete the
	transmission if necessary. */
	if (1 == gUartPort)
		XUartPs_Send(&xUARTInstance, (void *)pcString, usStringLength);
	else
		XUartLite_Send(&gLogUart, (u8 *)pcString, usStringLength);

	/* Wait until the string has been transmitted before exiting this function,
	otherwise there is a risk the calling function will overwrite the string
	pointed to by the pcString parameter while it is still being transmitted.
	The calling task will wait in the Blocked state (so not consuming any
	processing time) until the semaphore is available. */
	xSemaphoreTake(xTxCompleteSemaphore, xMaxWait);
}
/*-----------------------------------------------------------*/

signed portBASE_TYPE xSerialPutChar(xComPortHandle pxPort, signed char cOutChar, TickType_t xBlockTime)
{
	/* Only a single port is supported. */
	(void)pxPort;

	/* Send the character. */
	if (1 == gUartPort)
		XUartPs_Send(&xUARTInstance, (void *)&cOutChar, sizeof(cOutChar));
	else
		XUartLite_Send(&gLogUart, (void *)&cOutChar, sizeof(cOutChar));

	/* Wait for the transmission to be complete so the semaphore is left in the
	correct state for the next time vSerialPutString() is called. */
	xSemaphoreTake(xTxCompleteSemaphore, xBlockTime);

	return pdPASS;
}
/*-----------------------------------------------------------*/

void vSerialClose(xComPortHandle xPort)
{
	/* Not supported as not required by the demo application. */
	(void)xPort;
}
/*-----------------------------------------------------------*/

void prvUART_Handler(void *pvNotUsed)
{
	extern unsigned int XUartPs_SendBuffer(XUartPs * InstancePtr);
	uint32_t ulActiveInterrupts, ulChannelStatusRegister;
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	char cChar;

	configASSERT(pvNotUsed == &xUARTInstance);

	/* Remove compile warnings if configASSERT() is not defined. */
	(void)pvNotUsed;

	/* Read the interrupt ID register to see which interrupt is active. */
	ulActiveInterrupts = XUartPs_ReadReg(XPAR_PS7_UART_BASEADDR, XUARTPS_IMR_OFFSET);
	ulActiveInterrupts &= XUartPs_ReadReg(XPAR_PS7_UART_BASEADDR, XUARTPS_ISR_OFFSET);

	/* Are any receive events of interest active? */
	if ((ulActiveInterrupts & serRECEIVE_INTERRUPT_MASK) != 0)
	{
		/* Read the Channel Status Register to determine if there is any data in
		the RX FIFO. */
		ulChannelStatusRegister = XUartPs_ReadReg(XPAR_PS7_UART_BASEADDR, XUARTPS_SR_OFFSET);

		/* Move data from the Rx FIFO to the Rx queue.  NOTE THE COMMENTS AT THE
		TOP OF THIS FILE ABOUT USING QUEUES FOR THIS PURPOSE. */
		while ((ulChannelStatusRegister & XUARTPS_SR_RXEMPTY) == 0)
		{
			cChar = XUartPs_ReadReg(XPAR_PS7_UART_BASEADDR, XUARTPS_FIFO_OFFSET);

			/* If writing to the queue unblocks a task, and the unblocked task
			has a priority above the currently running task (the task that this
			interrupt interrupted), then xHigherPriorityTaskWoken will be set
			to pdTRUE inside the xQueueSendFromISR() function.
			xHigherPriorityTaskWoken is then passed to portYIELD_FROM_ISR() at
			the end of this interrupt handler to request a context switch so the
			interrupt returns directly to the (higher priority) unblocked
			task. */
			xQueueSendFromISR(xRxQueue, &cChar, &xHigherPriorityTaskWoken);
			ulChannelStatusRegister = XUartPs_ReadReg(XPAR_PS7_UART_BASEADDR, XUARTPS_SR_OFFSET);
			gUartPort = 1;
		}
	}

	/* Are any transmit events of interest active? */
	if ((ulActiveInterrupts & serTRANSMIT_IINTERRUPT_MASK) != 0)
	{
		if (xUARTInstance.SendBuffer.RemainingBytes == 0)
		{
			/* Give back the semaphore to indicate that the transmission is
			complete.  If giving the semaphore unblocks a task, and the
			unblocked task has a priority above the currently running task (the
			task that this interrupt interrupted), then xHigherPriorityTaskWoken
			will be set	to pdTRUE inside the xSemaphoreGiveFromISR() function.
			xHigherPriorityTaskWoken is then passed to portYIELD_FROM_ISR() at
			the end of this interrupt handler to request a context switch so the
			interrupt returns directly to the (higher priority) unblocked
			task. */
			xSemaphoreGiveFromISR(xTxCompleteSemaphore, &xHigherPriorityTaskWoken);

			/* No more data to transmit. */
			XUartPs_WriteReg(XPAR_PS7_UART_BASEADDR, XUARTPS_IDR_OFFSET, XUARTPS_IXR_TXEMPTY);
		}
		else
		{
			/* More data to send. */
			XUartPs_SendBuffer(&xUARTInstance);
		}
	}

	/* portYIELD_FROM_ISR() will request a context switch if executing this
	interrupt handler caused a task to leave the blocked state, and the task
	that left the blocked state has a higher priority than the currently running
	task (the task this interrupt interrupted).  See the comment above the calls
	to xSemaphoreGiveFromISR() and xQueueSendFromISR() within this function. */
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

	/* Clear the interrupt status. */
	XUartPs_WriteReg(XPAR_PS7_UART_BASEADDR, XUARTPS_ISR_OFFSET, ulActiveInterrupts);
}

void vLogUartRev_Handler(void *CallBackRef, unsigned int ByteCount)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	xQueueSendFromISR(xRxQueue, &gdata[0], &xHigherPriorityTaskWoken);
	XUartLite_Recv(&gLogUart, &gdata[0], 1);
	gUartPort = 2;
}

void vLogUartSend_Handler(void *CallBackRef, unsigned int ByteCount)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR(xTxCompleteSemaphore, &xHigherPriorityTaskWoken);
}

int LogUartInit(void)
{
	int Status;

//	XUartLite_Initialize(&gLogUart, XPAR_AXI_UARTLITE_1_DEVICE_ID);
//
//	XUartLite_SetSendHandler(&gLogUart, vLogUartSend_Handler, NULL);
//
//	XUartLite_SetRecvHandler(&gLogUart, vLogUartRev_Handler, NULL);
//
//	XUartLite_EnableInterrupt(&gLogUart);
//
//	XScuGic_SetPriorityTriggerType(&xInterruptController, 63, 0xB0, 0x3);
//
//	Status = XScuGic_Connect(&xInterruptController, 63, (Xil_ExceptionHandler)XUartLite_InterruptHandler, &gLogUart);
//	if (Status != XST_SUCCESS)
//		return Status;
//
//	XScuGic_Enable(&xInterruptController, 63);
//
//	XUartLite_ResetFifos(&gLogUart);
//
//	XUartLite_Recv(&gLogUart, (u8 *)&gdata[0], 1);

	return XST_SUCCESS;
}

void LogUartPrint(u8 *pSendBuf, u32 len)
{
	XUartLite_DisableInterrupt(&gLogUart);

	XUartLite_ResetFifos(&gLogUart);

	XUartLite_Send(&gLogUart, pSendBuf, len);

	while (gLogUart.SendBuffer.RemainingBytes)
	{
		vTaskDelay(10);
		XUartLite_SendBuffer(&gLogUart);
	}

	vTaskDelay(10);

	XUartLite_ResetFifos(&gLogUart);
	XUartLite_EnableInterrupt(&gLogUart);
}

void LogUartPrint2(u8 *pSendBuf, u32 len)
{
	XUartLite_Send(&gLogUart, pSendBuf, len);
}

int32_t GetLogUartLiteInstSendStatus(void)
{
	return gLogUart.SendBuffer.RemainingBytes;
}

int Usb_Cmd_Proc(uint8_t *buffdata, uint32_t EventData)
{
	int i;
	int ret = 1;

//	for (i = 0; i < EventData; i++)
//	{
//		ret = xQueueSend(xRxQueue, buffdata, 5);
//		if (errQUEUE_FULL == ret)
//		{
//			ret = 0;
//			break;
//		}
//	}

	return ret;
}
