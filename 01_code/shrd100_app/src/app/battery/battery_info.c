/*
 * battery_info.c
 *
 *  Created on: 2022年11月15日
 *      Author: A22745
 */

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

#include "../../hal/smbus_driver/smbus_driver.h"
#include "../../hal/output/output.h"
#include "../../srv/log/log.h"

int32_t GetRelativeStateOfCharge(uint8_t *value)
{
	uint8_t slave_addr = 0x16;
	uint8_t reg_addr = 0x0d;
	uint8_t buff[2] = {0};
	uint8_t len = 2;
	uint8_t ret = 1;
	static uint8_t cnt = 0;

	ret = smbus_sw_read_register(slave_addr, reg_addr, buff, len);
	if ((ret == 0) || (cnt > 3))
	{
		cnt = 0;
		ret = 0;
		*value = buff[0];
	}
	cnt++;

	return ret;
}

int32_t GetCurrent(uint8_t *value)
{
	uint8_t slave_addr = 0x16;
	uint8_t reg_addr = 0x0A;
	uint8_t buff[4] = {0};
	uint8_t len = 2;
	uint8_t ret = 1;
	static uint8_t cnt = 0;

	ret = smbus_sw_read_register(slave_addr, reg_addr, buff, len);
	if ((ret == 0) || (cnt > 3))
	{
		cnt = 0;
		ret = 0;
		*value = buff[0];
	}
	cnt++;

	return ret;
}

uint8_t UpdataBatteryCapacityAndDisplayLedState(uint8_t original_data)
{
	uint8_t ret = 1;
	uint8_t remaincap = original_data;

	ret = GetRelativeStateOfCharge(&remaincap);
	if (ret == 0)
	{
		if ((remaincap > 40) && ((remaincap <= 100)))
		{
//			GPIO_OutputCtrl(IO_LED_BAT_GREEN, LED_ON);
//			GPIO_OutputCtrl(IO_LED_BAT_YELLOW, LED_OFF); // green
		}
		else if (remaincap <= 40)
		{
//			GPIO_OutputCtrl(IO_LED_BAT_GREEN, LED_OFF);
//			GPIO_OutputCtrl(IO_LED_BAT_YELLOW, LED_ON); // yellow
		}
	}
	LOG_DEBUG("remaincp=%d ret:%d\r\n", remaincap, ret);

	return remaincap;
}

// uint16_t get_batteryFullChargehandler(void)
//{
//     uint8_t slave_addr = 0x16;
//     uint8_t reg_addr = 0x10;
//     uint8_t buff[2] = {0};
//     uint8_t len = 2;
//     uint16_t fullcharge = 0;
//
//	smbus_sw_read_register(slave_addr, reg_addr, buff, len);
//	fullcharge = *(uint16_t *)buff;
////	xil_printf("batteryFullCharge:  The register %d value is %x%x.\r\n", reg_addr, buff[1], buff[0]);
//
//	return fullcharge;
//}

uint16_t get_batteryFullChargehandler(uint16_t *fullcharge)
{
	uint8_t slave_addr = 0x16;
	uint8_t reg_addr = 0x10;
	uint8_t buff[2] = {0};
	uint8_t len = 2;
	int8_t ret = -1;

	ret = smbus_sw_read_register(slave_addr, reg_addr, buff, len);
	if (ret == 0)
	{
		*fullcharge = buff[0] + (buff[1] << 8);
	}

	return ret;
}

uint16_t get_batteryRemainCaphandler(uint16_t *capacity)
{
	uint8_t slave_addr = 0x16;
	uint8_t reg_addr = 0x0f;
	uint8_t buff[2] = {0};
	uint8_t len = 2;
	uint8_t ret = 1;

	ret = smbus_sw_read_register(slave_addr, reg_addr, buff, len);
	if (ret == 0)
	{
		*capacity = buff[0] + (buff[1] << 8);
	}
	return ret;
}

int16_t get_CalcPercentOfPower(void)
{
	uint16_t percent = 0;
	uint16_t fullcharge = 0;
	uint16_t capacity = 0;
	uint16_t ret = 1;

	ret = get_batteryFullChargehandler(&fullcharge);
	if (ret == 0)
	{
		if (get_batteryRemainCaphandler(&capacity) == 0)
		{
			percent = ((1.f * capacity) / (1.f * fullcharge)) * 100;
		}
		else
		{
			return -1;
		}
	}
	else
	{
		return -1;
	}

	return percent;
}
