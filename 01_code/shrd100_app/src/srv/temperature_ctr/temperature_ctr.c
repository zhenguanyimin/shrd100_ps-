/*
 * temperature_ctr.c
 *
 *  Created on: 2023年3月31日
 *      Author: A22745
 */

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include <timers.h>

#include "temperature_ctr.h"
#include "../../hal/hal.h"
#include "../system_monitor/system_monitor.h"
#include "../log/log.h"

#define ADJUSTMENT_TEMPERATUR_LOW_LIMIT		(55)
#define ADJUSTMENT_TEMPERATUR_UPPER_LIMIT	(75)

#define CAN_ADJUSTMENT_WIND_SPEED			(6)

//评判风扇转速正常与否的阈值
#define FAN_SPEED_NORMAL_THRESHOLD			(10)

//typedef enum fen_gear_position
//{
//	WIND_SPEED_0 = 0,
//	WIND_SPEED_1 = 1,
//	WIND_SPEED_2 = 2,
//	WIND_SPEED_3 = 3,
//	WIND_SPEED_4 = 4,
//	WIND_SPEED_5 = 5,
//	WIND_SPEED_6 = 6,
//	WIND_SPEED_7 = 7,
//	WIND_SPEED_8 = 8,
//	WIND_SPEED_9 = 9,
//	WIND_SPEED_10 = 10,   // max wind speed
//}fen_gear_position_t;

static fen_gear_position_t loca_speed = 0;
//static int FanRateList[11] = {0x107, 0xd7, 0xbc, 0xa1, 0x86, 0x6a, 0x4f, 0x33, 0x13, 0x0};
static int FanRateList[11] = {0x0, 0x13, 0x33, 0x4f, 0x6a, 0x86, 0xa1, 0xbc, 0xd7, 0xF5, 0x10E};
#define FAN_RATE_OFFSET 10


/* 0: 不正常， 1：正常 */
static uint8_t FanStatus = 0;

uint8_t GetFanStatus(void)
{
	return FanStatus;
}

void JudgFanStatus(void)
{
	int FanRate1 = axi_read_data(PL_I_FPGA_TEMP1);
	int FanRate2 = axi_read_data(PL_FAN_PWM_RATE);

	if (FanRate1 == FanRate2)
	{
		FanStatus = 1;
	}
	else
	{
		FanStatus = 0;
	}
}

void SetWindSpeed(fen_gear_position_t speed)
{
	uint8_t data;

	if (loca_speed == speed)
	{
		return;
	}
	switch (speed)
	{
		case WIND_SPEED_0:
		{
			data = 0x0A;
			break;
		}
		case WIND_SPEED_1:
		{
			data = 0x09;
			break;
		}
		case WIND_SPEED_2:
		{
			data = 0x08;
			break;
		}
		case WIND_SPEED_3:
		{
			data = 0x07;
			break;
		}
		case WIND_SPEED_4:
		{
			data = 0x06;
			break;
		}
		case WIND_SPEED_5:
		{
			data = 0x05;
			break;
		}
		case WIND_SPEED_6:
		{
			data = 0x04;
			break;
		}
		case WIND_SPEED_7:
		{
			data = 0x03;
			break;
		}
		case WIND_SPEED_8:
		{
			data = 0x02;
			break;
		}
		case WIND_SPEED_9:
		{
			data = 0x01;
			break;
		}
		case WIND_SPEED_10:
		{
			data = 0x00;
			break;
		}
		default :
		{
			data = 0x06;
			break;
		}
	}
	axi_write_data(PL_FAN_PWM_RATE, data);
	LOG_DEBUG("fan speed change,last speed=%d,new speed=%d,[max speed:%d]\r\n", loca_speed, speed, WIND_SPEED_10);
	loca_speed = speed;
}

static fen_gear_position_t CalculatWindSpeed(uint8_t temp)
{
	uint8_t res_temp;
	uint8_t deta_temp;
	fen_gear_position_t  speed = WIND_SPEED_5;
	uint8_t i = 0;

	uint8_t temperatureGradient = 0;

	if (temp <= ADJUSTMENT_TEMPERATUR_LOW_LIMIT)
		return WIND_SPEED_4;
	else if (temp >= ADJUSTMENT_TEMPERATUR_UPPER_LIMIT)
		return WIND_SPEED_10;

	res_temp = temp - ADJUSTMENT_TEMPERATUR_LOW_LIMIT;
	deta_temp = ADJUSTMENT_TEMPERATUR_UPPER_LIMIT - ADJUSTMENT_TEMPERATUR_LOW_LIMIT;

	temperatureGradient = (deta_temp / CAN_ADJUSTMENT_WIND_SPEED);
	if (deta_temp % CAN_ADJUSTMENT_WIND_SPEED != 0)
	{
		temperatureGradient++;
	}

	for (i = 1; i < (deta_temp / temperatureGradient); i++)
	{
		if (i*temperatureGradient > res_temp)
		{
			break;
		}
	}

	speed += (i-1);

	if (speed > WIND_SPEED_10)
		speed = WIND_SPEED_10;

	return speed;
}

void TemperatureCtlWindSpeed(void)
{
	static uint32_t last_set_time = 0;
	uint32_t cur_time = 0;
	/* after set speed, not set lower speed in 1 minute */
	uint32_t no_down_time = 3 * 60*1000;
	float max_temp;
	fen_gear_position_t wind_speed;
	static fen_gear_position_t last_wind_speed = WIND_SPEED_0;

	max_temp = MAX_GENAERAL (GetTempBatBoard2(), GetTempBatBoard1());
	max_temp = MAX_GENAERAL (GetTempSignalBoard(), max_temp);

	wind_speed = CalculatWindSpeed(max_temp);

	cur_time = xTaskGetTickCount();
	if (last_wind_speed < wind_speed)
	{
		SetWindSpeed(wind_speed);
		last_set_time = cur_time;
		last_wind_speed = wind_speed;
	}
	else
	{
		if ((cur_time - last_set_time) > no_down_time)
		{
			SetWindSpeed(wind_speed);
			last_set_time = cur_time;
			last_wind_speed = wind_speed;
		}
		else
		{

		}
	}
}
