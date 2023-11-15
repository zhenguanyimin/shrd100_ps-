/*
 * temperature_iic.h
 *
 *  Created on: 2023年3月22日
 *      Author: A22745
 */

#ifndef SRC_DRV_TEMPERATURE_TEMPERATURE_IIC_H_
#define SRC_DRV_TEMPERATURE_TEMPERATURE_IIC_H_

#include "FreeRTOS.h"
#include "semphr.h"
#include "xparameters.h"	/* EDK generated parameters */
#include "xiicps.h"
#include "xil_printf.h"
#include <stdio.h>
#include "../../inc/common.h"

ret_code_t I2cPs_write(u8 *MsgPtr, s32 ByteCount, u16 SlaveAddr);
ret_code_t I2cPs_read(u8 *MsgPtr, s32 ByteCount, u16 SlaveAddr);
int DeviceI2cInitDrv(u16 DeviceId);

#endif /* SRC_DRV_TEMPERATURE_TEMPERATURE_IIC_H_ */
