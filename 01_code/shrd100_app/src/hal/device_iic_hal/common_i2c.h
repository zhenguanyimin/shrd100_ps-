/*
 * common_i2c_api.h
 *
 *  Created on: 2023年3月30日
 *      Author: A22745
 */

#ifndef SRC_HAL_DEVICE_IIC_HAL_COMMON_I2C_H_
#define SRC_HAL_DEVICE_IIC_HAL_COMMON_I2C_H_

#include "../../inc/common.h"
#include "../../drv/device_iic_drv/device_iic_drv.h"

//------------------IIC ADDR---------------------------------------
#define CORE_BOARD_TEMPERATE_TMP75C_IIC_ADDR  			(0x48)
#define POWER_CURRENT_U48_INA230_IIC_ADDR  				(0x40)
#define POWER_CURRENT_U49_INA230_IIC_ADDR  				(0x41)
#define POWER_CURRENT_U50_INA230_IIC_ADDR  				(0x42)
#define POWER_BOARD_TEMPERATURE_U61_NSA2300_IIC_ADDR  	(0x6D)
#define BATTERY_6S2P_IIC_ADDR  							(0x55)
#define POWER_BOARD_TEMPERATURE_U61_ADS1115_IIC_ADDR  	(0x49)

#define EXTENDED_IO_TCA9534_IIC_ADDR					(0X20)

#define MIN_LEN(x,y) (((x)>(y))?(y):(x))

typedef enum i2c_slaver_select
{
	CORE_BOARD_TEMPERATE_TMP75C_U52 		= 0,
	POWER_BOARD_CURRENT_INA230_U48_12V 		= 1,
	POWER_BOARD_CURRENT_INA230_U49_28V 		= 2,
	POWER_BOARD_CURRENT_INA230_U50_32V 		= 3,
	POWER_BOARD_TEMPERATURE_NSA2300_U61 	= 4,
	BATTERY_BOARD_6S2P						= 5,
	EXTENDED_IO								= 6,
	POWER_BOARD_TEMPERATURE_ADS1115_U61		= 7,
} i2c_slaver_select_t;

void CommonI2cInitHal(void);
ret_code_t I2c_Reg_Read(i2c_slaver_select_t select, uint8_t reg,  uint8_t *data, uint8_t len);
ret_code_t I2c_Reg_Write(i2c_slaver_select_t select, uint8_t reg, uint8_t *data, uint8_t len);

#endif /* SRC_HAL_DEVICE_IIC_HAL_COMMON_I2C_H_ */
