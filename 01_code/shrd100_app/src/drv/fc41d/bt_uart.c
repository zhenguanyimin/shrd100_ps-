
#include "FreeRTOS.h"
#include "task.h"
#include <queue.h>
#include "semphr.h"
#include "xscugic.h"
#include "xil_types.h"
#include "bt_uart.h"
#include "../../srv/ring/ring.h"
//portmacro.h

#define BUFF_LEN	1024

static uint8_t	s_buffer[BUFF_LEN];
static ring_t	s_Ring = { 0 };

extern XScuGic xInterruptController;

SemaphoreHandle_t  WifiUartSeamphore;

uint32_t wifi_uart_buff_empty()
{
	uint32_t ret ;

	if( s_Ring.pvInpt == s_Ring.pvOupt )
	{
		ret = TRUE;
	}
	else
	{
		ret = FALSE;
	}

	return ret;
}

uint32_t get_wifi_uart_data( uint32_t uPos, uint8_t *buff , uint32_t len )
{
	uint32_t ret ;

	ret = ring_get_data( &s_Ring, uPos, buff , len );

	return ret;
}
uint32_t output_wifi_uart_data( uint8_t * buff, uint32_t uSize )
{
	uint32_t ret ;

	ret = ring_oupt_data( &s_Ring, buff, uSize );

	return ret;
}

void uart_wifi_isr(UART_TYPEDEF *uart_devp)
{
	char rec_data;
	static uint32_t cnt = 0x00 ;

	while (TRUE == uart_rx_fifo_no_empty(uart_devp))
	{
		uart_receive_no_block(uart_devp, &rec_data, 1);

		ring_inpt_byte( &s_Ring, rec_data );
		cnt++;

		if( cnt >= 1024 )
		{
			cnt = 0x00 ;
		}

	}
}

void wifi_Setup_Intr_System(XScuGic *GicInstancePtr )
{

    XScuGic_SetPriorityTriggerType(GicInstancePtr, UARTLITE_IRPT_WIFI, 0xA1, 0x3);

	XScuGic_Connect(GicInstancePtr, UARTLITE_IRPT_WIFI,	(Xil_ExceptionHandler)uart_wifi_isr,	(void *)UARTLITE_WIFI);

	uart_tx_fifo_clear( UARTLITE_WIFI );
	uart_rx_fifo_clear( UARTLITE_WIFI );

	XScuGic_Enable(GicInstancePtr, UARTLITE_IRPT_WIFI);		// enable the interrupt for GPIO at GIC

    uart_interrupt_control( UARTLITE_WIFI, XUL_CR_ENABLE_INTR, UART_ENABLE);

}

void init_fc41d_uart()
{
	WifiUartSeamphore = xSemaphoreCreateMutex();

	ring_init( &s_Ring, s_buffer, sizeof(s_buffer) );

	wifi_Setup_Intr_System( &xInterruptController );
}

void WifiSendDataFunc(char *data, uint32_t len)
{

	if (WifiUartSeamphore != NULL)
	{
		xSemaphoreTake(WifiUartSeamphore,portMAX_DELAY);
	}

	uart_send_block(UARTLITE_WIFI,(char*)data, len , 0x10000);

	if (WifiUartSeamphore != NULL)
	{
		xSemaphoreGive(WifiUartSeamphore);
	}

}
