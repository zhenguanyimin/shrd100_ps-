/*
 * battery.h
 *
 *  Created on: 2023年4月11日
 *      Author: A22745
 */

#ifndef SRC_HAL_DEVICE_IIC_HAL_BATTERY_H_
#define SRC_HAL_DEVICE_IIC_HAL_BATTERY_H_

#include "common_i2c.h"

typedef enum batteryRegisterAddr
{
	BATTERY_TEMPERATURE = 0X00,
	BATTERY_VOLTAGE		= 0X01,
	BATTERY_CURRENT		= 0X02,
	BATTERY_CAPACITY	= 0X03,
	BATTERY_STATUS		= 0X13,
} batteryRegisterAddr_e;


ret_code_t GetBatteryInfo(batteryRegisterAddr_e reg, uint16_t *data);
#endif /* SRC_HAL_DEVICE_IIC_HAL_BATTERY_H_ */
