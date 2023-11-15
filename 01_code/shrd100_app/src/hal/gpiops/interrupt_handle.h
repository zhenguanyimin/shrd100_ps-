/**
 * Copyright (C), Autoroad Tech. Co., Ltd.
 * @brief   Realize interrupt chip function configuration
 * @file    interrupt_handle.h
 * @author  X22012
 * @date    2022.11.15
 *
 * -History:
 *      -# author : X22012  
 *         date   : 2022.11.15
 *         Version: V1.0
 *         details: Created
 */

#ifndef INTERRUPT_HANDLE_H_
#define INTERRUPT_HANDLE_H_


/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h>
#include <xparameters.h>
#include "xscugic.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define INTC_DEVICE_ID          XPAR_SCUGIC_SINGLE_DEVICE_ID

/*******************************************************************************
 * Global Variables
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/
/** @addtogroup API
 * @{ */
/** @brief Set interrupt trigger type.
 *  @param [in]  InstancePtr      Ptr of interrupt instance.
 *  @param [in]  intId      	  interrupt id.
 *  @param [in]  intType          interrupt trigger type.
 * @return @ref None." */
void Intc_Type_Setup(XScuGic *InstancePtr, int intId, int intType);

/** @brief Initialize the interrupt controller driver.
 *  @param [in]  InstancePtr      Ptr of interrupt instance.
 * @return @ref "Status return code." */

/** @brief Initialize the interrupt exception.
 *  @param [in]  InstancePtr      Ptr of interrupt instance.
 * @return @ref "Status return code." */
void Setup_Intr_Exception(XScuGic * IntcInstancePtr);

/** @brief Initilizate interrupt function.
 *  @param [in]  DeviceId      device id.
 * @return @ref chc2442_status_t "Status return code." */
void Intc_Init_Function(uint16_t DeviceId);

/** @brief GPIO interrupt function.
 * @return @ref int "Status return code." */
int set_gpio_interrupt(void);

#endif /* INTERRUPT_HANDLE_H_ */
