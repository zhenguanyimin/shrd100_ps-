/*
 * battery_info.h
 *
 *  Created on: 2022年11月15日
 *      Author: A22745
 */

#ifndef SRC_APP_BATTERY_BATTERY_INFO_H_
#define SRC_APP_BATTERY_BATTERY_INFO_H_

uint8_t UpdataBatteryCapacityAndDisplayLedState(uint8_t original_data);
int16_t get_CalcPercentOfPower(void);
uint16_t get_batteryRemainCaphandler(uint16_t *capacity);
int32_t GetRelativeStateOfCharge(uint8_t *value);
#endif /* SRC_APP_BATTERY_BATTERY_INFO_H_ */
