/*
 * common_i2c_api.c
 *
 *  Created on: 2023年3月30日
 *      Author: A22745
 */

#include "common_i2c.h"
#include "temperature.h"
#include "extended_io.h"

#define BUF_LEN	(20)
#define IIC_NUMBER	(8)

static uint8_t g_wbuf_B[IIC_NUMBER][BUF_LEN] = {0};


uint8_t* GetChoiseWBuf(i2c_slaver_select_t select)
{
	uint8_t *ptr = NULL;
	switch(select)
	{
		case CORE_BOARD_TEMPERATE_TMP75C_U52:
		{
			ptr = &g_wbuf_B[0][0];
			break;
		}
		case POWER_BOARD_CURRENT_INA230_U48_12V:
		{
			ptr = &g_wbuf_B[1][0];
			break;
		}
		case POWER_BOARD_CURRENT_INA230_U49_28V:
		{
			ptr = &g_wbuf_B[2][0];
			break;
		}
		case POWER_BOARD_CURRENT_INA230_U50_32V:
		{
			ptr = &g_wbuf_B[3][0];
			break;
		}
		case POWER_BOARD_TEMPERATURE_NSA2300_U61:
		{
			ptr = &g_wbuf_B[4][0];
			break;
		}
		case BATTERY_BOARD_6S2P:
		{
			ptr = &g_wbuf_B[5][0];
			break;
		}
		case EXTENDED_IO:
		{
			ptr = &g_wbuf_B[6][0];
			break;
		}
		case POWER_BOARD_TEMPERATURE_ADS1115_U61:
		{
			ptr = &g_wbuf_B[7][0];
			break;
		}
		default:
		{
			ptr = &g_wbuf_B[0][0];
			break;
		}
	}
	return ptr;
}

uint8_t GetWBufLen(void)
{
	return BUF_LEN;
}


ret_code_t I2c_Reg_Read(i2c_slaver_select_t select, uint8_t reg,  uint8_t *data, uint8_t len)
{
	ret_code_t ret = RET_OK;
	uint8_t regAddr = reg;

//	uint16_t slaverAddr;
	uint8_t slaverAddr;

	if (data == NULL)
	{
		return RET_INVALID_PARAM;
	}

	switch (select)
	{
		case CORE_BOARD_TEMPERATE_TMP75C_U52:
		{
			slaverAddr = CORE_BOARD_TEMPERATE_TMP75C_IIC_ADDR;
			break;
		}
		case POWER_BOARD_CURRENT_INA230_U48_12V:
		{
			slaverAddr = POWER_CURRENT_U48_INA230_IIC_ADDR;
			break;
		}
		case POWER_BOARD_CURRENT_INA230_U49_28V:
		{
			slaverAddr = POWER_CURRENT_U49_INA230_IIC_ADDR;
			break;
		}
		case POWER_BOARD_CURRENT_INA230_U50_32V:
		{
			slaverAddr = POWER_CURRENT_U50_INA230_IIC_ADDR;
			break;
		}
		case POWER_BOARD_TEMPERATURE_NSA2300_U61:
		{
			slaverAddr = POWER_BOARD_TEMPERATURE_U61_NSA2300_IIC_ADDR;
			break;
		}
		case BATTERY_BOARD_6S2P:
		{
			slaverAddr = BATTERY_6S2P_IIC_ADDR;
			break;
		}
		case EXTENDED_IO:
		{
			slaverAddr = EXTENDED_IO_TCA9534_IIC_ADDR;
			break;
		}
		case POWER_BOARD_TEMPERATURE_ADS1115_U61:
		{
			slaverAddr = POWER_BOARD_TEMPERATURE_U61_ADS1115_IIC_ADDR;
			break;
		}
		default:
		{
			slaverAddr = CORE_BOARD_TEMPERATE_TMP75C_IIC_ADDR;
			break;
		}
	}

	if( slaverAddr == BATTERY_6S2P_IIC_ADDR )
	{
		ret = I2cPl_write(&regAddr, 1, slaverAddr);
		ret = I2cPl_read (data, len, slaverAddr);
	}
	else
	{
		ret = I2cPs_write(&regAddr, 1, slaverAddr);
		ret = I2cPs_read (data, len, slaverAddr);
	}

	return ret;
}


ret_code_t I2c_Reg_Write(i2c_slaver_select_t select, uint8_t reg, uint8_t *data, uint8_t len)
{
	ret_code_t ret = RET_OK;
	uint8_t minLen = 0;
	uint8_t bufLen = 0;
	uint8_t factLen = 0;

	uint16_t slaverAddr;

	uint8_t *buf = NULL;
	if (data == NULL)
	{
		return RET_INVALID_PARAM;
	}

	switch (select)
	{
		case CORE_BOARD_TEMPERATE_TMP75C_U52:
		{
			slaverAddr = CORE_BOARD_TEMPERATE_TMP75C_IIC_ADDR;
			break;
		}
		case POWER_BOARD_CURRENT_INA230_U48_12V:
		{
			slaverAddr = POWER_CURRENT_U48_INA230_IIC_ADDR;
			break;
		}
		case POWER_BOARD_CURRENT_INA230_U49_28V:
		{
			slaverAddr = POWER_CURRENT_U49_INA230_IIC_ADDR;
			break;
		}
		case POWER_BOARD_CURRENT_INA230_U50_32V:
		{
			slaverAddr = POWER_CURRENT_U50_INA230_IIC_ADDR;
			break;
		}
		case POWER_BOARD_TEMPERATURE_NSA2300_U61:
		{
			slaverAddr = POWER_BOARD_TEMPERATURE_U61_NSA2300_IIC_ADDR;
			break;
		}
		case BATTERY_BOARD_6S2P:
		{
			slaverAddr = BATTERY_6S2P_IIC_ADDR;
			break;
		}
		case EXTENDED_IO:
		{
			slaverAddr = EXTENDED_IO_TCA9534_IIC_ADDR;
			break;
		}
		case POWER_BOARD_TEMPERATURE_ADS1115_U61:
		{
			slaverAddr = POWER_BOARD_TEMPERATURE_U61_ADS1115_IIC_ADDR;
			break;
		}
		default:
		{
			slaverAddr = CORE_BOARD_TEMPERATE_TMP75C_IIC_ADDR;
			break;
		}
	}

	buf = GetChoiseWBuf(select);
	bufLen = GetWBufLen();
	buf[0] = reg;

	minLen = MIN_LEN((bufLen - 1), len);
	memcpy(&buf[1], data, minLen);

	if ((bufLen- 1) >= len) {
		factLen = len + 1;
	} else {
		factLen = bufLen;
	}

	ret = I2cPs_write(buf, factLen, slaverAddr);
	return ret;
}

void CommonI2cInitHal(void)
{

	DeviceI2cInitDrv(XPAR_XIICPS_1_DEVICE_ID);
	
	DeviceI2cPlInitDrv(XPAR_BAT_I2C_DEVICE_ID);

}

