/*
 * uartps_intr.h
 *
 *  Created on: 2019��5��3��
 *      Author: Administrator
 */

#ifndef SRC_UARTPS_INTR_H_
#define SRC_UARTPS_INTR_H_

#include "xparameters.h"
#include "xplatform_info.h"
#include "xuartps.h"
#include "xil_exception.h"
#include "xil_printf.h"
#include "xscugic.h"

int Init_UartPsIntr(XUartPs *UartInstPtr,u16 DeviceId);

int UartPs_Setup_IntrSystem(XScuGic *IntcInstancePtr,XUartPs *UartInstancePtr,u16 UartIntrId);

void UartPs_Intr_Handler(void *CallBackRef, u32 Event, unsigned int EventData);

void test_uart_send();

//#define UART_DEVICE_ID		XPAR_XUARTPS_1_DEVICE_ID
//#define INTC_DEVICE_ID		XPAR_SCUGIC_SINGLE_DEVICE_ID
//#define UART_INT_IRQ_ID		XPAR_XUARTPS_1_INTR
#define UART_DEVICE_ID		XPAR_XUARTPS_0_DEVICE_ID
#define INTC_DEVICE_ID		XPAR_SCUGIC_SINGLE_DEVICE_ID
#define UART_INT_IRQ_ID		XPAR_XUARTPS_0_INTR

#define TEST_BUFFER_SIZE	 (3 *1024)

extern XUartPs UartPs;//uart
extern volatile int TotalReceivedCount;
extern volatile int TotalSentCount;

uint8_t* Get_BufferPointer(void);

#endif /* SRC_UARTPS_INTR_H_ */
