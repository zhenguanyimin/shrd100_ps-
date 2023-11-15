/**
 * Copyright (C), Autoroad Tech. Co., Ltd.
 * @brief   Realize interrupt function configuration
 * @file    gpiops_intr.c
 * @author  X22012
 * @date    2022.11.20
 *
 * -History:
 *      -# author : X22012  
 *         date   : 2022.11.20
 *         Version: V1.0
 *         details: Created
 */


/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "gpiops_intr.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#include <stdio.h>
#include "xscugic.h"
#include "xil_printf.h"
#include "xil_exception.h"

/*******************************************************************************
 * Variables
 ******************************************************************************/
XGpioPs GpioPs;
/*******************************************************************************
 * Local Functions - Implementation
 ******************************************************************************/

void gpio_intr_isr( XScuGic *GicInstancePtr, u32 Bank, u32 Status )
{

	if( Bank == 0x03 )
	{
		if( ( Status & EMIO_INTR_GPS_PPS ) == EMIO_INTR_GPS_PPS )
		{
//			gps_pps_isr();
		}
	}
}
/*******************************************************************************
 * API - Implementation
 ******************************************************************************/
void Gpiops_Setup_Intr_System(XScuGic *GicInstancePtr, XGpioPs *InstancePtr, u16 IntrId)
{
	u8 Bank;
	u8 PinNumber;

	XScuGic_Connect(GicInstancePtr, IntrId,
					(Xil_ExceptionHandler)XGpioPs_IntrHandler,	// set up the timer interrupt
					(void *)InstancePtr);

	/* Set the handler for gpio interrupts. */
	XGpioPs_SetCallbackHandler(InstancePtr, (void *)InstancePtr, (XGpioPs_Handler)gpio_intr_isr);

	XGpioPs_GetBankPin(RX_GPS_PPS_PIN, &Bank, &PinNumber);
    XGpioPs_IntrEnable(InstancePtr, Bank, (1 << PinNumber));

	XScuGic_Enable(GicInstancePtr, IntrId);		// enable the interrupt for GPIO at GIC

}

int Intr_gpiops_init(XGpioPs *InstancePtr, u32 DeviceId)
{
	u8 Bank;
	u8 PinNumber;
	u32 pin_idx;

	int Status;
	XGpioPs_Config *ConfigPtr;
	/* Initialize the GPIO driver. */
	ConfigPtr = XGpioPs_LookupConfig(GPIO_DEVICE_ID);
	Status = XGpioPs_CfgInitialize(InstancePtr, ConfigPtr,ConfigPtr->BaseAddr);

	//gps
	XGpioPs_SetDirectionPin(	InstancePtr,	TX_GPS_FORCE_ON_PIN,	1);
	XGpioPs_SetDirectionPin(	InstancePtr,	TX_GPS_RESET_PIN,		1);
	XGpioPs_SetOutputEnablePin(	InstancePtr,	TX_GPS_FORCE_ON_PIN,	1);
	XGpioPs_SetOutputEnablePin(	InstancePtr,	TX_GPS_RESET_PIN,		1);
	Gpio_WritePin(	TX_GPS_FORCE_ON_PIN,	1);
	Gpio_WritePin(	TX_GPS_RESET_PIN,		0);

    XGpioPs_SetDirectionPin(InstancePtr, RX_GPS_PPS_PIN, 0x00);
	XGpioPs_GetBankPin(RX_GPS_PPS_PIN, &Bank, &PinNumber);
    //Edge trigger, rising edge trigger
    XGpioPs_SetIntrType(InstancePtr, Bank, 0xFFFFFFFF, 0xFFFFFFFF, 0x00);

    return Status;
}

u32 Gpio_ReadPin( u32 Pin)
{
	return XGpioPs_ReadPin(&GpioPs,Pin);
}

u32 Gpio_WritePin( u32 Pin ,u32 Data)
{
	XGpioPs_WritePin(&GpioPs, Pin, Data);

	return 0;
}
