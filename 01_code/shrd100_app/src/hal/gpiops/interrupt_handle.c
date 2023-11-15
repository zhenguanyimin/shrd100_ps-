/**
 * Copyright (C), Autoroad Tech. Co., Ltd.
 * @brief   Realize interrupt chip function configuration
 * @file    interrupt_handle.c
 * @author  X22012
 * @date    2022.11.15
 *
 * -History:
 *      -# author : X22012  
 *         date   : 2022.11.15
 *         Version: V1.0
 *         details: Created
 */


/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "interrupt_handle.h"
#include "gpiops_intr.h"


/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Global Variables
 ******************************************************************************/

/*******************************************************************************
 * Local Functions - Implementation
 ******************************************************************************/

/*******************************************************************************
 * API - Implementation
 ******************************************************************************/
#include <stdio.h>
#include "xil_printf.h"
#include "xil_exception.h"

#define INT_CFG0_OFFSET 			0x00000C00
#define SCUGIC_0_ID 				XPAR_SCUGIC_0_DEVICE_ID
#define INT_TYPE_MASK               0x03

extern XScuGic xInterruptController;
//static XGpioPs_Config *XGpioPs_Cfg;
//static XScuGic_Config *XScuGic_Cfg;

/*******************************************************************************
 * API - Implementation
 ******************************************************************************/
void Intc_Type_Setup(XScuGic *InstancePtr, int intId, int intType)
{
    int mask;

    intType &= INT_TYPE_MASK;
    mask = XScuGic_DistReadReg(InstancePtr, INT_CFG0_OFFSET + (intId/16)*4);
    mask &= ~(INT_TYPE_MASK << (intId%16)*2);
    mask |= intType << ((intId%16)*2);
    XScuGic_DistWriteReg(InstancePtr, INT_CFG0_OFFSET + (intId/16)*4, mask);
}

void Intc_Init_Function(u16 DeviceId)
{

//	Init_Intr_System(&xInterruptController);

	Gpiops_Setup_Intr_System(&xInterruptController, &GpioPs, GPIO_INTERRUPT_ID);


//	Setup_Intr_Exception(&xInterruptController);
}

