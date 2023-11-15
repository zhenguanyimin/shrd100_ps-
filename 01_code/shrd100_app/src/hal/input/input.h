/*
 * input.h
 *
 *  Created on:
 *      Author: Ultimate
 */
#ifndef INPUT_H_
#define INPUT_H_
#include <sys/_stdint.h>
#include "../output/output.h"

#include "../hal.h"
typedef enum
{
	MIO_MUTE_SWITCH,
	MIO_SWITCH_MODE0,
	MIO_SWITCH_MODE1,
	MIO_SWITCH_MODE2,
	MIO_SWITCH_MODE3,
	EMIO_CONFIG_SW,
	EMIO_GPS_SW,
	EMIO_TRIGGER,
	EMIO_VCC0_21V_OUT_INT,
	EMIO_FAN_SENSOR,
	EMIO_GYRO_INT2_DRDY,
	EMIO_GYRO_INT1,
	EMIO_ACC_MAGINT,
	EMIO_ACC_INT2_XL,
	EMIO_ACC_INT1_XL,
	EMIO_WIFI_NET_STATUS,
	EMIO_WIFI_WAKEUP_IN,
	EMIO_WIFI_RX1,
	EMIO_CTRL_OUT0,   //
	EMIO_CTRL_OUT1,   //
	EMIO_CTRL_OUT2,   //
	EMIO_CTRL_OUT3,   //
	EMIO_CTRL_OUT4,   //
	EMIO_CTRL_OUT5,   //
	EMIO_CTRL_OUT6,   //
	EMIO_CTRL_OUT7,   //
	EMIO_GPS_PPS,
	EMIO_VCC28V_PG_1V8,
	EMIO_VCC32V_PG_1V8,
	EMIO_RF_1Q,
	EMIO_RF_2Q,
	INPUT_NAME_MAX,
}INPUT_NAME;


typedef struct
{
	uint8_t Enable;//1 初始化   0 不初始化
	uint8_t GpioMode;
	uint8_t DeviceId;
	uint8_t AxiGpioChl;
	uint16_t Pin;
}INPUT_ARG;

uint8_t Input_GetValue(INPUT_NAME name);
int32_t InputInit(void);

#endif /* INPUT_H_ */
