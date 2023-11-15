/*
 * extended_io.h
 *
 *  Created on: 2023年4月18日
 *      Author: A22745
 */

#ifndef SRC_HAL_DEVICE_IIC_HAL_EXTENDED_IO_H_
#define SRC_HAL_DEVICE_IIC_HAL_EXTENDED_IO_H_

#include "common_i2c.h"

//1:INPUT,0:OUTPUT
#define  EXTENDED_IO_INPUT_OUTPUT_CONFIG	(0X1F)

#define  VSENSE_ALT00_BIT_0		(0X01 << 0)		// input: 1
#define  VSENSE_ALT01_BIT_1		(0X01 << 1)		// input: 1
#define  VSENSE_ALT02_BIT_2		(0X01 << 2)		// input: 1
#define  VDC_PLUG_IN_BIT_3		(0X01 << 3)		// input: 1
#define  CHG_ON_BIT_4			(0X01 << 4)		// input: 1
#define  LED_2_EN_BIT_5			(0X01 << 5)		// output:0
#define  LED_1_EN_BIT_6			(0X01 << 6)		// output:0
#define  CHG_EN_BIT_7			(0X01 << 7)		// output:0

//#define EXTEND_IO_HIGH_LEVEL			(1)
//#define EXTEND_IO_LOW_LEVEL				(0)

typedef enum extend_io_level
{
	EXTEND_IO_LOW_LEVEL = 0x00,
	EXTEND_IO_HIGH_LEVEL = 0x01,
} extend_io_level_e;

// 输入类型
typedef enum extendIoInputType
{
	EXTEND_IO_INPUT_VSENSE_ALT00	= 0X00,
	EXTEND_IO_INPUT_VSENSE_ALT01	= 0X01,
	EXTEND_IO_INPUT_VSENSE_ALT02	= 0X02,
	EXTEND_IO_INPUT_VDC_PLUG_IN		= 0X03,		// 充电插入
	EXTEND_IO_INPUT_VCHG_ON			= 0X04,		// 正常充电
} extendIoInputType_e;

// 输出类型
typedef enum extendIoOutputType
{
	EXTEND_IO_OUTPUT_LED_1_TYPE		= 0X00,
	EXTEND_IO_OUTPUT_LED_2_TYP		= 0X01,
	EXTEND_IO_OUTPUT_CHARG_EN_TYP	= 0X02,
} extendIoOutputType_e;


typedef enum extendedIoRegisterAddr
{
	EXTENDED_IO_INPUT_REGISTER 					= 0X00,
	EXTENDED_IO_OUTPUT_REGISTER					= 0X01,
	EXTENDED_IO_POLARITY_INVERSION_REGISTER		= 0X02,
	EXTENDED_IO_CONFIG_REGISTER					= 0X03,
} extendedIoRegisterAddr_e;


void ExtendedIoInit(void);
void SetSpecifyExtendIo(extendIoOutputType_e type, extend_io_level_e level);
extend_io_level_e GetSpecifyExtendIo(extendIoInputType_e type);
void ExtendedIoTest(void);

#endif /* SRC_HAL_DEVICE_IIC_HAL_EXTENDED_IO_H_ */
