/*
 * temperature_ctr.h
 *
 *  Created on: 2023年3月31日
 *      Author: A22745
 */

#ifndef SRC_SRV_TEMPERATURE_CTR_TEMPERATURE_CTR_H_
#define SRC_SRV_TEMPERATURE_CTR_TEMPERATURE_CTR_H_

#include "../../hal/device_iic_hal/common_i2c.h"
#include "../../hal/device_iic_hal/temperature.h"

typedef enum fen_gear_position
{
	WIND_SPEED_0 = 0,
	WIND_SPEED_1 = 1,
	WIND_SPEED_2 = 2,
	WIND_SPEED_3 = 3,
	WIND_SPEED_4 = 4,
	WIND_SPEED_5 = 5,
	WIND_SPEED_6 = 6,
	WIND_SPEED_7 = 7,
	WIND_SPEED_8 = 8,
	WIND_SPEED_9 = 9,
	WIND_SPEED_10 = 10,   // max wind speed
}fen_gear_position_t;

#define MAX_GENAERAL(x,y) (((x)>(y))?(x):(y))

void SetWindSpeed(fen_gear_position_t speed);
void TemperatureCtlWindSpeed(void);
uint8_t GetFanStatus(void);
void JudgFanStatus(void);

#endif /* SRC_SRV_TEMPERATURE_CTR_TEMPERATURE_CTR_H_ */
