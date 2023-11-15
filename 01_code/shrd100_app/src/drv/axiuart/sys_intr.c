/********************MILIANKE**************************

 *Company:Liyang Milian Electronic Technology Co., Ltd
 *Technical forum:www.uisrc.com
 *Taobao: https://milianke.taobao.com
 *Create Date: 2020/12/01
 *Module Name:sys_intr
 *Copyright: Copyright (c) milianke
 *Revision: 1.1
 *Description: Interrupt management

****************************************************/

#include "sys_intr.h"
#include "uartlite_pl_intr.h"

extern XScuGic xInterruptController; // GIC
static XUartLite UartLiteInst0;		 /* The instance of the UartLite Device */
static XUartLite UartLiteInst1;		 /* The instance of the UartLite Device */
static XUartLite UartLiteInst2;		 /* The instance of the UartLite Device */
static XUartLite UartLiteInst3;		 /* The instance of the UartLite Device */
static XUartLite UartLiteInst4;		 /* The instance of the UartLite Device */

void Setup_Intr_Exception(XScuGic *IntcInstancePtr)
{
	/* Enable interrupts from the hardware */
	Xil_ExceptionInit();
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
								 (Xil_ExceptionHandler)XScuGic_InterruptHandler,
								 (void *)IntcInstancePtr);

	Xil_ExceptionEnable();
}

int Init_Intr_System(XScuGic *IntcInstancePtr)
{
	int Status;

	XScuGic_Config *IntcConfig;
	/*
	 * Initialize the interrupt controller driver so that it is ready to
	 * use.
	 */
	IntcConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);
	if (NULL == IntcConfig)
	{
		return XST_FAILURE;
	}

	Status = XScuGic_CfgInitialize(IntcInstancePtr, IntcConfig,
								   IntcConfig->CpuBaseAddress);
	if (Status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}
	return XST_SUCCESS;
}

int Axi_UartLite_Init(uint8_t num, XUartLite_Handler FuncPtr)
{
	int Status;

//	Status = Init_Intr_System(&xInterruptController);
//	if (Status != XST_SUCCESS)
//	{
//		return XST_FAILURE;
//	}

	if (0 == num)
	{
		Status = UartLite_Init(&UartLiteInst0, UARTLITE_DEVICE_ID_0, FuncPtr);
		if (Status != XST_SUCCESS)
		{
			return XST_FAILURE;
		}
		Status = UartLite_setup_IntrSystem(&xInterruptController, &UartLiteInst0, UARTLITE_IRPT_INTR_0);
		if (Status != XST_SUCCESS)
		{
			return XST_FAILURE;
		}
	}
	else if (1 == num)
	{
		Status = UartLite_Init(&UartLiteInst1, UARTLITE_DEVICE_ID_1, FuncPtr);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}
		Status = UartLite_setup_IntrSystem(&xInterruptController, &UartLiteInst1, UARTLITE_IRPT_INTR_1);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}
	}
//	else if (2 == num)
//	{
//		Status = UartLite_Init(&UartLiteInst2, UARTLITE_DEVICE_ID_2, FuncPtr);
//		if (Status != XST_SUCCESS) {
//			return XST_FAILURE;
//		}
//		Status = UartLite_setup_IntrSystem(&xInterruptController, &UartLiteInst2, UARTLITE_IRPT_INTR_2);
//		if (Status != XST_SUCCESS) {
//			return XST_FAILURE;
//		}
//	}
//	else if (3 == num)
//	{
//		Status = UartLite_Init(&UartLiteInst3, UARTLITE_DEVICE_ID_3, FuncPtr);
//		if (Status != XST_SUCCESS) {
//			return XST_FAILURE;
//		}
//		Status = UartLite_setup_IntrSystem(&xInterruptController, &UartLiteInst3, UARTLITE_IRPT_INTR_3);
//		if (Status != XST_SUCCESS) {
//			return XST_FAILURE;
//		}
//	}
//	else if (4 == num)
//	{
//		Status = UartLite_Init(&UartLiteInst4, UARTLITE_DEVICE_ID_4, FuncPtr);
//		if (Status != XST_SUCCESS) {
//			return XST_FAILURE;
//		}
//		Status = UartLite_setup_IntrSystem(&xInterruptController, &UartLiteInst4, UARTLITE_IRPT_INTR_4);
//		if (Status != XST_SUCCESS) {
//			return XST_FAILURE;
//		}
//	}
	Setup_Intr_Exception(&xInterruptController);

	return XST_SUCCESS;
}

void UartLite_RevData(uint8_t num, u8 *DataBufferPtr, uint32_t len)
{
	if (0 == num)
	{
		XUartLite_Recv(&UartLiteInst0, DataBufferPtr, len);
	}
	else if (1 == num)
	{
		XUartLite_Recv(&UartLiteInst1, DataBufferPtr, len);
	}
	else if (2 == num)
	{
		XUartLite_Recv(&UartLiteInst2, DataBufferPtr, len);
	}
	else if (3 == num)
	{
		XUartLite_Recv(&UartLiteInst3, DataBufferPtr, len);
	}
	else if (4 == num)
	{
		XUartLite_Recv(&UartLiteInst4, DataBufferPtr, len);
	}
}

int32_t UartLite_SendData(uint8_t num, u8 *DataBufferPtr, uint32_t len)
{
	int32_t ret = -1;

	if (0 == num)
	{
		ret = XUartLite_Send(&UartLiteInst0, DataBufferPtr, len);
	}
	else if (1 == num)
	{
		ret = XUartLite_Send(&UartLiteInst1, DataBufferPtr, len);
	}
	else if (2 == num)
	{
		ret = XUartLite_Send(&UartLiteInst2, DataBufferPtr, len);
	}
	else if (3 == num)
	{
		ret = XUartLite_Send(&UartLiteInst3, DataBufferPtr, len);
	}
	else if (4 == num)
	{
		ret = XUartLite_Send(&UartLiteInst4, DataBufferPtr, len);
	}

	return ret;
}

int32_t GetUartLiteInstSendStatus(uint8_t num)
{
	int32_t ret = -1;

	if (0 == num)
	{
		ret = UartLiteInst0.SendBuffer.RemainingBytes;
	}
	else if (1 == num)
	{
		ret = UartLiteInst1.SendBuffer.RemainingBytes;
	}
	else if (2 == num)
	{
		ret = UartLiteInst2.SendBuffer.RemainingBytes;
	}
	else if (3 == num)
	{
		ret = UartLiteInst3.SendBuffer.RemainingBytes;
	}

	return ret;
}

