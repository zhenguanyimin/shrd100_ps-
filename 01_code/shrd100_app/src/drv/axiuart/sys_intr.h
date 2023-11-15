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

#ifndef SYS_INTR_H_
#define SYS_INTR_H_

#include "xparameters.h"
#include "xil_exception.h"
#include "xdebug.h"
#include "xscugic.h"
#include "xuartlite.h"

#define INTC_DEVICE_ID XPAR_SCUGIC_SINGLE_DEVICE_ID

int Init_Intr_System(XScuGic *IntcInstancePtr);
void Setup_Intr_Exception(XScuGic *IntcInstancePtr);
int Axi_UartLite_Init(uint8_t num, XUartLite_Handler FuncPtr);
void UartLite_RevData(uint8_t num, u8 *DataBufferPtr, uint32_t len);
int32_t UartLite_SendData(uint8_t num, u8 *DataBufferPtr, uint32_t len);
int32_t GetUartLiteInstSendStatus(uint8_t num);

#endif /* SYS_INTR_H_ */
