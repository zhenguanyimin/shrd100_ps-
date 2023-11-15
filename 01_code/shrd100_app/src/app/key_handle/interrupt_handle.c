/**
 * Copyright (C), Autel Tech. Co., Ltd.
 * @brief   Realize chip function configuration
 * @file    interrupt_handle.c
 * @author  X22012
 *
 * -History:
 *      -# author : X22012
 *         date   : 2022-05-15
 *         Version: V1.0
 *         details: Created
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "interrupt_handle.h"

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
#include "xscugic.h"
#include "xil_printf.h"
#include "xil_exception.h"
#include "../data_path/data_path_droneid.h"
#include "../sys_status_data/detection_param.h"
#include "../process_task/detect_process_task.h"
#include "../rf_config/pl_config.h"



#define INT_CFG0_OFFSET 0x00000C00
#define INT_TYPE_RISING_EDGE 0x03
#define INT_TYPE_HIGHLEVEL 0x01
#define INT_TYPE_MASK 0x03

// Parameter definitions
// Definitions for DDR data used for function droneid
#define READ_DDR_0_256M_ID 		139
#define READ_DDR_256_512M_ID	140
#define READ_DDR_BURST_ID 		128

extern XScuGic xInterruptController;

void ReadDDR_0_256M_intr_Handler(void *param)
{
#if( WORKMODE == DRONEID)
	DataPath_Post(eDATA_PATH_DDR_0_256M);
#endif
}

void ReadDDR_256_512M_intr_Handler(void *param)
{
#if( WORKMODE == DRONEID)
	DataPath_Post(eDATA_PATH_DDR_256_512M);
#endif

}

void ReadDDRBurst_intr_Handler(void *param)
{
#if( WORKMODE == DRONEID)
	DataPath_Post(eDATA_PATH_DDR_BURST);
#endif

}

void Droneid_timer_intr_Handler( )
{
	DataPath_Post(eDATA_PATH_TIMER);

#if( WORKMODE == DRONEID)
	RF_StopPLAlg();
	DET_CfgNextDetection();
	RF_StartPLAlgByTimer();
#endif

}

void IntcTypeSetup(XScuGic *InstancePtr, int intId, int intType)
{
	int mask;
	uint32_t nLocalIntId = (uint32_t)intId;

	intType &= INT_TYPE_MASK;
	mask = XScuGic_DistReadReg(InstancePtr, INT_CFG0_OFFSET + (nLocalIntId / 16) * 4);
	mask &= ~(INT_TYPE_MASK << (intId % 16) * 2);
	mask |= intType << ((intId % 16) * 2);
	XScuGic_DistWriteReg(InstancePtr, INT_CFG0_OFFSET + (nLocalIntId / 16) * 4, mask);
}

int IntcInitFunction(u16 DeviceId)
{
	int status;

	// Connect interrupt to handler
	status = XScuGic_Connect(&xInterruptController,
			READ_DDR_0_256M_ID,
							 (Xil_ExceptionHandler)ReadDDR_0_256M_intr_Handler,
							 (void *)NULL);
	if (status != XST_SUCCESS)
		return XST_FAILURE;

	status = XScuGic_Connect(&xInterruptController,
			READ_DDR_256_512M_ID,
							 (Xil_ExceptionHandler)ReadDDR_256_512M_intr_Handler,
							 (void *)NULL);
	if (status != XST_SUCCESS)
		return XST_FAILURE;

	status = XScuGic_Connect(&xInterruptController,
			READ_DDR_BURST_ID,
							 (Xil_ExceptionHandler)ReadDDRBurst_intr_Handler,
							 (void *)NULL);
	if (status != XST_SUCCESS)
		return XST_FAILURE;

	IntcTypeSetup(&xInterruptController, READ_DDR_0_256M_ID, INT_TYPE_RISING_EDGE);
	IntcTypeSetup(&xInterruptController, READ_DDR_256_512M_ID, INT_TYPE_RISING_EDGE);
	IntcTypeSetup(&xInterruptController, READ_DDR_BURST_ID, INT_TYPE_RISING_EDGE);

	XScuGic_Enable(&xInterruptController, READ_DDR_0_256M_ID);
	XScuGic_Enable(&xInterruptController, READ_DDR_256_512M_ID);
	XScuGic_Enable(&xInterruptController, READ_DDR_BURST_ID);

	return XST_SUCCESS;
}

void *IntcGetGicInst(void)
{
	return (void *)&xInterruptController;
}
