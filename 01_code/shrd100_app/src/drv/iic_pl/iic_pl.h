/*
 * temperature_iic.h
 *
 *  Created on: 2023年3月22日
 *      Author: A22745
 */

#ifndef SRC_DRV_IIC_PL_H_
#define SRC_DRV_IIC_PL_H_

#include "FreeRTOS.h"
#include "semphr.h"
#include "xparameters.h"	/* EDK generated parameters */
#include "xiic_l.h"
#include "xil_printf.h"
#include <stdio.h>
#include "../../inc/common.h"


int DeviceI2cPlInitDrv(u16 IicDeviceId);
int I2cPl_write(u8 *MsgPtr, s32 ByteCount, u16 SlaveAddr);
int I2cPl_read(u8 *MsgPtr, s32 ByteCount, u16 SlaveAddr);

#endif /* SRC_DRV_TEMPERATURE_TEMPERATURE_IIC_H_ */
