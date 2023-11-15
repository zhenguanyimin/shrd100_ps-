/*
 * system_monitor.c
 *
 *  Created on: 2023年4月12日
 *      Author: A21001
 */

#include <stdint.h>

#include "../../hal/hal.h"

#include "system_monitor.h"

#include "../log/log.h"
#include "../../hal/device_iic_hal/temperature.h"
#include "../../hal/device_iic_hal/battery.h"
#include "../../hal/device_iic_hal/extended_io.h"
#include "../../srv/temperature_ctr/temperature_ctr.h"
/* 电流，单位毫安，0xFFFF无效数据 */
static float fCurrentV12 = 0;
static float fCurrentV28 = 0.0;
static float fCurrentV32 = 0.0;

/* 电压，单位毫伏，0xFFFF无效数据 */
static float fVoltageV12 = 0;
static float fVoltageV28 = 0.0;
static float fVoltageV32 = 0.0;

/* FPGA芯片温度，单位度  */
static float fTempFpga = 0.0;
/* 电池转接板温度1，单位 度  */
static float fTempBatBoard1 = 0.0;
/* 电池转接板温度2，单位 度   */
static float fTempBatBoard2 = 0.0;
/* 信号处理板温度，单位 度    */
static float fTempSignalBoard = 0.0;
/* 电池内部读出来的温度，单位 度    */
static float fTempbat = 0.0;

/* 电池电量，满电值：100% */
static uint16_t g_batCap = 100;
/* 电池电压，毫伏 ，无效数据：0xFFFF */
static uint16_t BatVolt = 0xFFFF;

/* 电源适配器插、电池接入状态 */
static uint8_t g_batteryAndAdapterState = 0;

static uint8_t g_batCapValid = 0;

/* 周期通过log输出监测信息准许标志位 */
static uint8_t g_periodOutPutI2cDeviceInfoFlag= 1;


uint8_t GetPeriodOutPutI2cDeviceInfoFlag(void)
{
	return g_periodOutPutI2cDeviceInfoFlag;
}

void SetPeriodOutPutI2cDeviceInfoFlag(uint8_t flag)
{
	g_periodOutPutI2cDeviceInfoFlag = flag;
}

uint16_t GetBatCap(void)
{
	return g_batCap;
}

uint16_t GetBatVolt(void)
{
	return BatVolt;
}

uint8_t GetBatCapValid(void)
{
	return g_batCapValid;
}

float GetCurrentV12(void)
{
	return fCurrentV12;
}

float GetCurrentV28(void)
{
	return fCurrentV28;
}

float GetCurrentV32(void)
{
	return fCurrentV32;
}

float GetVoltageV12(void)
{
	return fVoltageV12;
}

float GetVoltageV28(void)
{
	return fVoltageV28;
}

float GetVoltageV32(void)
{
	return fVoltageV32;
}



float GetTempFpga(void)
{
	return fTempFpga;
}

float GetTempBatBoard1(void)
{
	return fTempBatBoard1;
}

float GetTempBatBoard2(void)
{
	return fTempBatBoard2;
}

float GetTempSignalBoard(void)
{
	return fTempSignalBoard;
}

void ReadFpgaTemp(float *FpgaTemp)
{
	int32_t ReadTemp = 0;
	float fReadTemp = 0.0;

	ReadTemp = axi_read_data(PL_I_FPGA_TEMP0);

	/* 公式中需要除以2的AD位数的幂 ， 这里用256 * 256 */
	fReadTemp = (ReadTemp * 507.5921310)/(256 * 256) - 279.42657680;

	*FpgaTemp = fReadTemp;
}

void GetSystemInchargeState(void)
{
	uint8_t state;
	ret_code_t ret = RET_OK;
	extend_io_level_e level_plug_in;
	uint16_t dataTemp;

	level_plug_in = GetSpecifyExtendIo(EXTEND_IO_INPUT_VDC_PLUG_IN);
	ret = GetBatteryInfo(BATTERY_TEMPERATURE, &dataTemp);

	if ((level_plug_in == EXTEND_IO_HIGH_LEVEL) && (ret == RET_OK))
	{
		g_batteryAndAdapterState = STATUS_BATTERY_AND_POWER_ADAPTER;
	}
	else if (ret == RET_OK)
	{
		g_batteryAndAdapterState = STATUS_ONLY_BATTERY;
	}
	else if (level_plug_in == EXTEND_IO_HIGH_LEVEL)
	{
		g_batteryAndAdapterState = STATUS_ONLY_POWER_ADAPTER;
	}

//	LOG_DEBUG("g_batteryAndAdapterState:%d\r\n", g_batteryAndAdapterState);
}

uint8_t GetBatteryAndAdapterState(void)
{
	return g_batteryAndAdapterState;
}

void GetSystemVol(void)
{
//	GetBatteryInfo(BATTERY_VOLTAGE, &BatVolt);
	g_batCapValid = GetBatteryInfo(BATTERY_CAPACITY, &g_batCap);
//	GetVoltage(POWER_BOARD_12V_CURRENT_TYPE, &fVoltageV12);
//	GetVoltage(POWER_BOARD_28V_CURRENT_TYPE, &fVoltageV28);
//	GetVoltage(POWER_BOARD_32V_CURRENT_TYPE, &fVoltageV32);

	if (GetPeriodOutPutI2cDeviceInfoFlag() == 1)
	{

		LOG_DEBUG("bat cap=%d\r\n", g_batCap);
//		LOG_DEBUG("bat vol=%dmv,bat cap=%d, g_batCapValid=%d\r\n", BatVolt, g_batCap, g_batCapValid);
//		LOG_DEBUG("12V_vol=%d.%dmv,28V_vol=%d.%dmv,32V_vol=%d.%dmv\r\n", (uint16_t)fVoltageV12, (uint16_t)((uint16_t)(fVoltageV12*100)%100),
//				(uint16_t)fVoltageV28, (uint16_t)((uint16_t)(fVoltageV28*100)%100),
//				(uint16_t)fVoltageV32, (uint16_t)((uint16_t)(fVoltageV32*100)%100));
	}
}

void GetSystemTemperature(void)
{
	GetTemperature(core_board_temperature_tmp75, &fTempSignalBoard);
//	GetTemperature(power_board_temperature_nsa2300_1, &fTempBatBoard1);
	GetTemperature(power_board_temperature_ads1115_0, &fTempBatBoard1);
	GetTemperature(power_board_temperature_ads1115_1, &fTempBatBoard2);
//	GetTemperature(battery_temperature, &fTempbat);

//	ReadFpgaTemp(&fTempFpga);
	if (GetPeriodOutPutI2cDeviceInfoFlag() == 1)
	{
		LOG_DEBUG("FPGA temp=%d.%d; board temp%d.%d; pwr board temp=%d %d;\r\n", (int)fTempbat,
			(int)((int)(fTempbat*100)%100),(int)fTempSignalBoard, (int)((int)(fTempSignalBoard*1000)%1000));
//		LOG_DEBUG("bat temp=%d.%d; board temp%d.%d; pwr board temp=%d %d;FPGA temp:%d.%d,AdapterState\r\n", (int)fTempbat,
//			(int)((int)(fTempbat*100)%100),(int)fTempSignalBoard, (int)((int)(fTempSignalBoard*1000)%1000), (int)fTempBatBoard1,(int)fTempBatBoard2,
//			(int)fTempFpga, (int)((int)(fTempFpga*1000)%1000), GetBatteryAndAdapterState());
	}
}

void UpdateAllSensorData(void)
{
	GetSystemTemperature();
	GetSystemVol();
}

void FaultDiagnosis_Task(void *p_arg)
{


	while (1)
	{
		vTaskDelay(3000);

		UpdateAllSensorData();

		JudgFanStatus();

		TemperatureCtlWindSpeed();
	}
}


int32_t InitFaultDiagnosis_Task()
{
	int32_t retVal = -1;

	if (xTaskCreate(FaultDiagnosis_Task, "FaultDiagnosis_Task", TASK_STACK_SIZE_DFT, NULL, TASK_PRI_DFT, NULL) == 1)
	{
		retVal = 0;
	}

	return retVal;
}
