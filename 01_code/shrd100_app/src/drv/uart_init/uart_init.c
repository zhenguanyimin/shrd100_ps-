
#include "../uart_init/sys_intr.h"
#include "../uart_init/uartps_intr.h"



void init_intr_sys(void)
{
	Init_UartPsIntr(&UartPs,UART_DEVICE_ID);
//	Init_Intr_System(&Intc);
	Setup_Intr_Exception(&xInterruptController);
	UartPs_Setup_IntrSystem(&xInterruptController, &UartPs, UART_INT_IRQ_ID);
}

int uart_init(void)
{
	init_intr_sys();
	XUartPs_Recv(&UartPs, Get_BufferPointer(), TEST_BUFFER_SIZE);

	return 0;
}



