
#include "xil_types.h"
#include "uartps_drv.h"

extern XScuGic xInterruptController;
static XUartPs xUARTInstance;

static int muc_dir_key_cnt = 0 ;

int get_muc_dir_key_cnt()
{
	int ret = 0;

	ret = muc_dir_key_cnt;
	muc_dir_key_cnt = 0 ;

	return ret;
}

void MCU_UART_Handler(void *pvNotUsed)
{
	extern unsigned int XUartPs_SendBuffer(XUartPs * InstancePtr);
	uint32_t ulActiveInterrupts, ulChannelStatusRegister;
	char cChar;

	configASSERT(pvNotUsed == &xUARTInstance);

	/* Remove compile warnings if configASSERT() is not defined. */
	(void)pvNotUsed;

	/* Read the interrupt ID register to see which interrupt is active. */
	ulActiveInterrupts = XUartPs_ReadReg(XPAR_PSU_UART_1_BASEADDR, XUARTPS_IMR_OFFSET);
	ulActiveInterrupts &= XUartPs_ReadReg(XPAR_PSU_UART_1_BASEADDR, XUARTPS_ISR_OFFSET);

	/* Are any receive events of interest active? */
	if ((ulActiveInterrupts & UARTPS_INTERRUPT_MASK) != 0)
	{
		/* Read the Channel Status Register to determine if there is any data in
		the RX FIFO. */
		ulChannelStatusRegister = XUartPs_ReadReg(XPAR_PSU_UART_1_BASEADDR, XUARTPS_SR_OFFSET);

		/* Move data from the Rx FIFO to the Rx queue.  NOTE THE COMMENTS AT THE
		TOP OF THIS FILE ABOUT USING QUEUES FOR THIS PURPOSE. */
		while ((ulChannelStatusRegister & XUARTPS_SR_RXEMPTY) == 0)
		{
			cChar = XUartPs_ReadReg(XPAR_PSU_UART_1_BASEADDR, XUARTPS_FIFO_OFFSET);

			if( cChar == 0x5a )
			{
				//short press of mcu key
				muc_dir_key_cnt = 1;
			}
			if( cChar == 0xa5 )
			{
				//long press of mcu key
			}
			/* If writing to the queue unblocks a task, and the unblocked task
			has a priority above the currently running task (the task that this
			interrupt interrupted), then xHigherPriorityTaskWoken will be set
			to pdTRUE inside the xQueueSendFromISR() function.
			xHigherPriorityTaskWoken is then passed to portYIELD_FROM_ISR() at
			the end of this interrupt handler to request a context switch so the
			interrupt returns directly to the (higher priority) unblocked
			task. */
			ulChannelStatusRegister = XUartPs_ReadReg(XPAR_PSU_UART_1_BASEADDR, XUARTPS_SR_OFFSET);
		}
	}

	/* Clear the interrupt status. */
	XUartPs_WriteReg(XPAR_PSU_UART_1_BASEADDR, XUARTPS_ISR_OFFSET, ulActiveInterrupts);
}

void mcu_Setup_Intr_System( )
{
	BaseType_t xStatus;
	XUartPs_Config *pxConfig;

	/* Look up the UART configuration then initialise the dirver. */
	pxConfig = XUartPs_LookupConfig(XPAR_XUARTPS_1_DEVICE_ID);

	/* Initialise the driver. */
	xStatus = XUartPs_CfgInitialize(&xUARTInstance, pxConfig, XPAR_PSU_UART_1_BASEADDR);
	configASSERT(xStatus == XST_SUCCESS);
	(void)xStatus; /* Remove compiler warning if configASSERT() is not defined. */

	/* Misc. parameter configuration. */
	XUartPs_SetBaudRate(&xUARTInstance, 115200);
	XUartPs_SetOperMode(&xUARTInstance, XUARTPS_OPER_MODE_NORMAL);

	/* Install the interrupt service routine that is defined within this
	file. */
	xStatus = XScuGic_Connect(&xInterruptController, XPAR_XUARTPS_1_INTR, (Xil_ExceptionHandler)MCU_UART_Handler, (void *)&xUARTInstance);
	configASSERT(xStatus == XST_SUCCESS);
	(void)xStatus; /* Remove compiler warning if configASSERT() is not defined. */

	/* Ensure interrupts start clear. */
	XUartPs_WriteReg(XPAR_PSU_UART_1_BASEADDR, XUARTPS_ISR_OFFSET, XUARTPS_IXR_MASK);

	/* Enable the UART interrupt within the GIC. */
	XScuGic_Enable(&xInterruptController, XPAR_XUARTPS_1_INTR);

	/* Enable the interrupts of interest in the UART. */
	XUartPs_SetInterruptMask(&xUARTInstance, XUARTPS_IXR_RXFULL | XUARTPS_IXR_RXOVR | XUARTPS_IXR_TOUT | XUARTPS_IXR_TXEMPTY);

	/* Set the receive timeout. */
	XUartPs_SetRecvTimeout(&xUARTInstance, 8);

}

