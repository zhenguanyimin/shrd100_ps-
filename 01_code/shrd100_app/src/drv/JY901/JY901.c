#include "JY901.h"
#include "string.h"
#include "../uartlite/uartlite_drv.h"
#include "../../app/devInfo_process/devInfo_process.h"

QueueHandle_t ImuRxQueue = NULL;

SAcc_t 		g_stcAcc;
SGyro_t 	g_stcGyro;
SAngle_t 	g_stcAngle;
SMag_t 		g_stcMag;

static uint8_t g_uartDataUpdateFlag = 0; // 1: update 0:not update

void SetUartDataUpdateFlag(uint8_t flag)
{
	g_uartDataUpdateFlag = flag;
}

uint8_t GetUsartDataUpdateFlag(void)
{
	return g_uartDataUpdateFlag;
}

void uart_imu_isr(UART_TYPEDEF *uart_devp)
{
	char rec_data;
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	g_uartDataUpdateFlag = UART_DATA_UPDATE_FLAG_SET;

	while (FALSE == uart_rx_fifo_empty(uart_devp))
	{
		uart_receive_no_block(uart_devp, &rec_data, 1);

		ImuProc(rec_data);
	}

}

void JY901_Setup_Intr_System(XScuGic *GicInstancePtr )
{

    XScuGic_SetPriorityTriggerType(GicInstancePtr, UARTLITE_IRPT_IMU, 0xA2, 0x3);

	XScuGic_Connect(GicInstancePtr, UARTLITE_IRPT_IMU,	(Xil_ExceptionHandler)uart_imu_isr,	(void *)UARTLITE_IMU);

	uart_tx_fifo_clear( UARTLITE_IMU );
	uart_rx_fifo_clear( UARTLITE_IMU );

	XScuGic_Enable(GicInstancePtr, UARTLITE_IRPT_IMU);		// enable the interrupt for GPIO at GIC

    uart_interrupt_control( UARTLITE_IMU, XUL_CR_ENABLE_INTR, UART_ENABLE);

}

