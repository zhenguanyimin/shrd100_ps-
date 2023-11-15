/**
 * Copyright (C), Autoroad Tech. Co., Ltd.
 * @brief   Realize interrupt function configuration
 * @file    gpiops_intr.h
 * @author  X22012
 * @date    2022.11.20
 *
 * -History:
 *      -# author : X22012  
 *         date   : 2022.11.20
 *         Version: V1.0
 *         details: Created
 */

#ifndef GPIOPS_INTR_H_
#define GPIOPS_INTR_H_


/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdio.h>
#include "xscugic.h"
#include "xil_types.h"
#include "xparameters.h"
#include "xgpiops.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define GPIO_DEVICE_ID				XPAR_XGPIOPS_0_DEVICE_ID
#define GPIO_INTERRUPT_ID	 		XPAR_XGPIOPS_0_INTR

#define FAN_CTRL_PIN                (26)
#define RX_1V8_EN_PIN               (27)
#define TX_2V0A_EN_PIN              (28)
#define TX_2V0B_EN_PIN              (29)
#define TX0_1V8_EN_PIN              (30)
#define TX1_1V8_EN_PIN              (31)
#define WIFI_NETSTATUS_PIN          (34)
#define WIFI_WAKEUP_IN_PIN          (36)
#define WIFI_RST_PIN                (37)
#define WIFI_WAKEUP_OUT_PIN         (38)
#define LED6_CTRL_PIN               (39)
#define LED5_CTRL_PIN               (40)
#define LED4_CTRL_PIN               (41)
#define LED3_CTRL_PIN               (42)
#define LED2_CTRL_PIN               (43)
#define LED1_CTRL_PIN               (44)

#define ENET_POWER_EN_PIN           (61)
#define PS_ENET_RESET_PIN           (63)

//EMIO
#define EMIO_BASE_ADDR				77
#define TX_GPS_FORCE_ON_PIN			(EMIO_BASE_ADDR+7)//low : backup mode    high : full on mode
#define RX_GPS_PPS_PIN				(EMIO_BASE_ADDR+8)//rising edge
#define TX_GPS_RESET_PIN			(EMIO_BASE_ADDR+9)//restarted by driving the RESET to a low level voltage for at least 10ms and then releasing it.
#define RX_GYRO_INT1_PIN			(EMIO_BASE_ADDR+10)
#define RX_GYRO_INT2_DRDY_PIN		(EMIO_BASE_ADDR+11)
#define RX_ACC_INT1_XL_PIN			(EMIO_BASE_ADDR+21)
#define RX_ACC_INT2_XL_PIN			(EMIO_BASE_ADDR+22)
#define RX_ACC_MAGINT_PIN			(EMIO_BASE_ADDR+23)


#define EMIO_INTR_GPS_PPS				(1<<8)//rising edge
#define EMIO_INTR_GYRO_INT1				(1<<10)
#define EMIO_INTR_GYRO_INT2_DRDY		(1<<11)
#define EMIO_INTR_ACC_INT1_XL			(1<<21)
#define EMIO_INTR_ACC_INT2_XL			(1<<22)
#define EMIO_INTR_ACC_MAGINT			(1<<23)

/*******************************************************************************
 * Global Variables
 ******************************************************************************/

extern XGpioPs GpioPs;	/* The driver instance for GPIO Device. */
/*******************************************************************************
 * API
 ******************************************************************************/
/** @addtogroup API
 * @{ */
/** @brief Init gpio interrupt function.
 *  @param [in]  Value      Data to be read.
 * @return @ref chc2442_status_t "Status return code." */
void Gpiops_Setup_Intr_System(XScuGic *GicInstancePtr, XGpioPs *InstancePtr, u16 IntrId);

/** @brief Init gpio function.
 * @return @ref int "Status return code." */
int Intr_gpiops_init(XGpioPs *InstancePtr, u32 DeviceId);

u32 Gpio_ReadPin( u32 Pin);

u32 Gpio_WritePin( u32 Pin ,u32 Data);

#endif /* GPIOPS_INTR_H_ */
