/*
 * system_monitor.h
 *
 *  Created on: 2023年4月12日
 *      Author: A21001
 */

#ifndef SRC_SRV_SYSTEM_MONITOR_SYSTEM_MONITOR_H_
#define SRC_SRV_SYSTEM_MONITOR_SYSTEM_MONITOR_H_

typedef enum batStatus
{
	STATUS_ONLY_BATTERY 				= 0X00,
	STATUS_BATTERY_AND_POWER_ADAPTER 	= 0X01,
	STATUS_ONLY_POWER_ADAPTER 			= 0X02,
} batStatus_e;


float GetTempFpga(void);
float GetTempBatBoard1(void);
float GetTempBatBoard2(void);
float GetTempSignalBoard(void);
void GetSystemTemperature(void);


uint16_t GetBatCap(void);

uint8_t GetBatteryAndAdapterState(void);
void GetSystemInchargeState(void);

uint8_t GetBatCapValid(void);
uint16_t GetBatVolt(void);

uint8_t GetPeriodOutPutI2cDeviceInfoFlag(void);
void SetPeriodOutPutI2cDeviceInfoFlag(uint8_t flag);
uint8_t GetFanStatus(void);
int32_t InitFaultDiagnosis_Task();
#endif /* SRC_SRV_SYSTEM_MONITOR_SYSTEM_MONITOR_H_ */
