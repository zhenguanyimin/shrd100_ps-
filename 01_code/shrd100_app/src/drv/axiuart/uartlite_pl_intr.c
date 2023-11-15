/********************MILIANKE**************************

 *Company:Liyang Milian Electronic Technology Co., Ltd
 *Technical forum:www.uisrc.com
 *Taobao: https://milianke.taobao.com
 *Create Date: 2020/12/01
 *Module Name:uartlite_pl_intr
 *Copyright: Copyright (c) milianke
 *Revision: 1.1
 *Description:

****************************************************/

#include "uartlite_pl_intr.h"
#include "xuartps_hw.h"

// volatile int TotalSentCount;
// volatile int TotalRecvCount;

static void UartLiteSendHandler(void *CallBackRef, unsigned int EventData);

static void UartLiteSendHandler(void *CallBackRef, unsigned int EventData)
{
	// TotalSentCount = EventData;
}

void UartLiteDisableIntrSystem(XScuGic *IntcInstancePtr, u16 UartLiteIntrId)
{
	XScuGic_Disable(IntcInstancePtr, UartLiteIntrId);
	XScuGic_Disconnect(IntcInstancePtr, UartLiteIntrId);
}

int UartLite_setup_IntrSystem(XScuGic *IntcInstancePtr, XUartLite *UartLiteInstPtr, u16 UartLiteIntrId)
{
	int Status;

	XScuGic_SetPriorityTriggerType(IntcInstancePtr, UartLiteIntrId, 0xA0, 0x3);

	Status = XScuGic_Connect(IntcInstancePtr, UartLiteIntrId, (Xil_ExceptionHandler)XUartLite_InterruptHandler, UartLiteInstPtr);
	if (Status != XST_SUCCESS)
		return Status;

	XUartLite_EnableInterrupt(UartLiteInstPtr);

	XScuGic_Enable(IntcInstancePtr, UartLiteIntrId);

	return XST_SUCCESS;
}

int UartLite_Init(XUartLite *UartLiteInstPtr, u16 UartLiteDeviceId, XUartLite_Handler FuncPtr)
{
	int Status;

	Status = XUartLite_Initialize(UartLiteInstPtr, UartLiteDeviceId);
	if (Status != XST_SUCCESS)
		return XST_FAILURE;

	XUartLite_SetSendHandler(UartLiteInstPtr, UartLiteSendHandler, UartLiteInstPtr);
	XUartLite_SetRecvHandler(UartLiteInstPtr, FuncPtr, UartLiteInstPtr);

	return XST_SUCCESS;
}
