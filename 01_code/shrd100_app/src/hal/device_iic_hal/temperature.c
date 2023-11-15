/*
 * temperature.c
 *
 *  Created on: 2023年3月30日
 *      Author: A22745
 */


#include "temperature.h"
#include "common_i2c.h"
#include "battery.h"
#include "../../srv/log/log.h"

//#define MIN_LEN(x,y) (((x)>(y))?(y):(x))
#define REPEAT_COUNT_100	(100)

//-------------------------------------TMP75C-----temperature------------------------------------------------------------
#define FACTOR_NUMBER	(0.0625)


//-------------------------------------NSA2300-----temperature------------------------------------------------------------
//与NTC串联的参考电阻5k欧姆，NSA2300芯片手册有
#define NSA2300_NTC_REFERENCE_RESISTANCE_TEMP	(5000)

#define NSA2300_ONECE_NTC_ADC_START				(0X08)
#define NSA2300_ONECE_SENSOR_ADC_START			(0X09)
#define NSA2300_SAMPLE_COMPLETE_MASK			(0X01 << 3)
#define NTC_TABLE_LEN	(167)


// t4 电源板在NTC电路中串联了4.99K欧姆
#define FIX_RESISTANCE 	(4990)

// 温度补偿
#define TEMPERATURE_COMPENSATION (17)

// NTC 表中第一个电阻值代表的温度
#define FIRST_NTC_TABLE_TEMP (-55)

// ads1115 温度变换异常判断阈值
#define ADS1115_TEMP_CHANGE_ABNORMAL_THRESHOLD		(15)

typedef struct tempChangeStatus
{
	uint16_t lastTemp;
	uint16_t resultTemp;
	uint8_t firstInFlag;
} tempChangeStatus_s;

typedef struct tmp75ChangeStatus
{
	float lastTemp;
	float resultTemp;
	uint8_t firstInFlag;
} tmp75ChangeStatus_s;

static tempChangeStatus_s ads1115tempStatus[ADS1115_INPUT_CHANNEL_MAX_NUM] =
{
		{.resultTemp = 0, .lastTemp = 0, .firstInFlag = 1},
		{.resultTemp = 0, .lastTemp = 0, .firstInFlag = 1}
};

static tmp75ChangeStatus_s tmp75TempStatus = {.resultTemp = 0, .lastTemp = 0, .firstInFlag = 1};

//电阻值 对应的温度
uint32_t NTC_TABLE[167] = {
	0x6F7A6, // -55
	0x68047, // -54
	0x60EA9, // -53
	0x5A35F, // -52
	0x53EE9, // -51
	0x4E181, // -50
	0x48B19, // -49
	0x43B6A, // -48
	0x3F207, // -47
	0x3AE72, // -46
	0x37026, // -45
	0x336A2, // -44
	0x30171, // -43
	0x2D027, // -42
	0x2A266, // -41
	0x277DD, // -40
	0x25046, // -39
	0x22B62, // -38
	0x208FC, // -37
	0x1E8E4, // -36
	0x1CAF0, // -35
	0x1AEF9, // -34
	0x194DD, // -33
	0x17C7A, // -32
	0x165B3, // -31
	0x1506B, // -30
	0x13C88, // -29
	0x129F2, // -28
	0x11891, // -27
	0x1084E, // -26
	0xF917,  // -25
	0xEAD9,  // -24
	0xDD80,  // -23
	0xD0FE,  // -22
	0xC544,  // -21
	0xBA42,  // -20
	0xAFEE,  // -19
	0xA63A,  // -18
	0x9D1C,  // -17
	0x9489,  // -16
	0x8C7A,  // -15
	0x84E4,  // -14
	0x7DC1,  // -13
	0x7709,  // -12
	0x70B5,  // -11
	0x6ABF,  // -10
	0x6522,  // -9
	0x5FD7,  // -8
	0x5ADA,  // -7
	0x5627,  // -6
	0x51B8,  // -5
	0x4D8A,  // -4
	0x4999,  // -3
	0x45E1,  // -2
	0x425F,  // -1
	0x3F0F,  // 0
	0x3BEF,  // 1
	0x38FC,  // 2
	0x3632,  // 3
	0x3391,  // 4
	0x3114,  // 5
	0x2EBA,  // 6
	0x2C81,  // 7
	0x2A67,  // 8
	0x286A,  // 9
	0x2687,  // 10
	0x24BF,  // 11
	0x230F,  // 12
	0x2175,  // 13
	0x1FF0,  // 14
	0x1E80,  // 15
	0x1D22,  // 16
	0x1BD7,  // 17
	0x1A9B,  // 18
	0x1970,  // 19
	0x1853,  // 20
	0x1745,  // 21
	0x1643,  // 22
	0x154F,  // 23
	0x1465,  // 24
	0x1388,  // 25
	0x12B4,  // 26
	0x11EB,  // 27
	0x112B,  // 28
	0x1074,  // 29
	0xFC5,   // 30
	0xF1E,   // 31
	0xE80,   // 32
	0xDE8,   // 33
	0xD57,   // 34
	0xCCD,   // 35
	0xC49,   // 36
	0xBCB,   // 37
	0xB52,   // 38
	0xADF,   // 39
	0xA71,   // 40
	0xA08,   // 41
	0x9A4,   // 42
	0x944,   // 43
	0x8E8,   // 44
	0x890,   // 45
	0x83C,   // 46
	0x7EC,   // 47
	0x79F,   // 48
	0x755,   // 49
	0x70F,   // 50
	0x6CB,   // 51
	0x68B,   // 52
	0x64D,   // 53
	0x611,   // 54
	0x5D9,   // 55
	0x5A2,   // 56
	0x56E,   // 57
	0x53C,   // 58
	0x50D,   // 59
	0x4DF,   // 60
	0x4B3,   // 61
	0x489,   // 62
	0x460,   // 63
	0x439,   // 64
	0x414,   // 65
	0x3F0,   // 66
	0x3CE,   // 67
	0x3AD,   // 68
	0x38D,   // 69
	0x36E,   // 70
	0x351,   // 71
	0x335,   // 72
	0x31A,   // 73
	0x300,   // 74
	0x2E7,   // 75
	0x2CE,   // 76
	0x2B7,   // 77
	0x2A1,   // 78
	0x28B,   // 79
	0x276,   // 80
	0x262,   // 81
	0x24F,   // 82
	0x23D,   // 83
	0x22B,   // 84
	0x219,   // 85
	0x209,   // 86
	0x1F9,   // 87
	0x1E9,   // 88
	0x1DA,   // 89
	0x1CC,   // 90
	0x1BE,   // 91
	0x1B0,   // 92
	0x1A3,   // 93
	0x196,   // 94
	0x18A,   // 95
	0x17F,   // 96
	0x173,   // 97
	0x168,   // 98
	0x15E,   // 99
	0x153,   // 100
	0x14A,   // 101
	0x140,   // 102
	0x137,   // 103
	0x12E,   // 104
	0x125,   // 105
	0x11D,   // 106
	0x115,   // 107
	0x10D,   // 108
	0x105,   // 109
	0xFE, 	 // 110
	0xF7,	 // 111
};

//-------------------------------------NSA2300-----temperature------------------------------------------------------------
static uint32_t g_ntc_adc_calibra_full_value;


//-------------------------------------TMP75C-----temperature------------------------------------------------------------

ret_code_t Tmp75_Temperature_Read(float *temp)
{
	ret_code_t ret = RET_OK;
	uint8_t data_s[2] = {0};
	uint16_t data = 0;
	float  temperature =0.0;

	if (temp == NULL)
	{
		return RET_INVALID_PARAM;
	}
	ret = I2c_Reg_Read(CORE_BOARD_TEMPERATE_TMP75C_U52, TMP75_TEMPERATURE_REGISTER, &data_s[0], 2);

	if (ret != RET_OK)
	{
		temperature =0.0;
		*temp = 0;
		return ret;
	}
	data = (data_s[0] << 4) | (data_s[1] >> 4);
	temperature = data * FACTOR_NUMBER;

	if (tmp75TempStatus.firstInFlag == 1)
	{
		tmp75TempStatus.resultTemp = temperature;
		tmp75TempStatus.lastTemp = temperature;
		tmp75TempStatus.firstInFlag = 0;
	}
	else
	{
		if (abs(tmp75TempStatus.lastTemp - temperature) >= ADS1115_TEMP_CHANGE_ABNORMAL_THRESHOLD)
		{
		}
		else
		{
			tmp75TempStatus.resultTemp = temperature;
		}
		tmp75TempStatus.lastTemp = temperature;
	}

	*temp = tmp75TempStatus.resultTemp;
	return ret;
}

//ret_code_t Tmp75_Temperature_Read(float *temp)
//{
//	ret_code_t ret = RET_OK;
//	uint8_t data_s[2] = {0};
//	uint16_t data = 0;
//	float  temperature =0.0;
//
//	if (temp == NULL)
//	{
//		return RET_INVALID_PARAM;
//	}
//	ret = I2c_Reg_Read(CORE_BOARD_TEMPERATE_TMP75C_U52, TMP75_TEMPERATURE_REGISTER, &data_s[0], 2);
//	data = (data_s[0] << 4) | (data_s[1] >> 4);
//	temperature = data * FACTOR_NUMBER;
//	*temp = temperature;
//
//	return ret;
//}


//-------------------------------------NSA2300-----temperature------------------------------------------------------------

uint32_t GetNtcCalibraFullAdcValue(void)
{
	return g_ntc_adc_calibra_full_value;
}


/*
 * function:
 * 			查表获取获取当前阻值的index号
 * paramer:
 * 		table: NTC电阻数组
 * 		table: 数组长度
 * 		data:  需要查找的电阻
 * return:
 * 		返回输入电阻值在表中的index号
 */
uint8_t LookUpNtcTable(uint32_t *table, uint8_t tableLen, uint16_t data)
{
	uint16_t begin,end,middle ;
	uint8_t i = 0;

	begin = 0;
	end = tableLen - 1;

	if (table == NULL)
	{
		return 0;
	}

	if (data > table[0])
	{
		return 0;
	}
	else if (data <= table[end])
	{
		return end;
	}

	while (begin < end)
	{
		i++;
		middle = (begin + end) / 2;
		if (data == table[middle])
		{
			break;
		}
		else if ((data < table[middle]) && (data > table[middle + 1]))
		{
			break;
		}

		if (data > table[middle])
		{
			end = middle;
		}
		else
		{
			begin = middle;
		}

		if (i > tableLen)
		{
			break;
		}
	}

	if(begin > end)
	{
		return 0;
	}

	return middle;
}

ret_code_t Nsa2300_Read_Id(uint8_t *temp)
{
	ret_code_t ret = RET_OK;
	uint8_t data = 0;

	ret = I2c_Reg_Read(POWER_BOARD_TEMPERATURE_NSA2300_U61, NSA2300_PART_ID_REGISTER, &data, 1);
	if (temp != NULL)
	{
		*temp = data;
	}

	return ret;
}

ret_code_t Nsa2300_Ntc_adc_Read(uint32_t *value, float *resistance)
{
	ret_code_t ret = RET_OK;
	int8_t sfirst_data = 0;
	uint8_t data_s[2] = {0};
	int32_t data = 0;
	float resist;
	uint8_t configData;
	uint8_t state = 0;
	uint8_t count = 0;

	configData = NSA2300_ONECE_NTC_ADC_START; // START adc
	I2c_Reg_Write(POWER_BOARD_TEMPERATURE_NSA2300_U61, NSA2300_CMD_REGISTER, &configData, sizeof(configData));

	do
	{
		count++;
		ret = I2c_Reg_Read(POWER_BOARD_TEMPERATURE_NSA2300_U61, NSA2300_STATUS_REGISTER, &state, 1);
	} while( ((state & 0x01) != 0) && (RET_OK == ret) && (count < REPEAT_COUNT_100));

	if (count == REPEAT_COUNT_100)
	{
		ret = RET_GENERAL_ERR;
	}
	ret = I2c_Reg_Read(POWER_BOARD_TEMPERATURE_NSA2300_U61, NSA2300_DATA_MSB_REGISTER, (uint8_t*)&sfirst_data, 1);
	ret = I2c_Reg_Read(POWER_BOARD_TEMPERATURE_NSA2300_U61, NSA2300_DATA_CSB_REGISTER, &data_s[0], 1);
	ret = I2c_Reg_Read(POWER_BOARD_TEMPERATURE_NSA2300_U61, NSA2300_DATA_LSB_REGISTER, &data_s[1], 1);

	data = (sfirst_data << (8 * 2)) | (data_s[0] << (8 * 1)) | (data_s[1] << (8 * 0));
	if (value != NULL)
	{
		*value = data;
	}

//	xil_printf(" read adc value=%d\r\n", data);
//	resist = (float)((float)(g_ntc_adc_calibra_full_value - data) / data) * NSA2300_NTC_REFERENCE_RESISTANCE_TEMP;

	resist = (float)((float)(g_ntc_adc_calibra_full_value - data) / data) * NSA2300_NTC_REFERENCE_RESISTANCE_TEMP - FIX_RESISTANCE;

	if (resistance != NULL)
	{
		*resistance = resist;
	}

	return ret;
}

ret_code_t Nsa2300_Sensor_adc_Read(uint32_t *value)
{
	ret_code_t ret = RET_OK;
	uint8_t sfirst_data = 0;
	uint8_t data_s[2] = {0};
	int32_t data = 0;
	uint8_t configData;
	uint8_t state = 0xff;
	uint8_t count = 0;


	configData = NSA2300_ONECE_SENSOR_ADC_START; // START adc
	I2c_Reg_Write(POWER_BOARD_TEMPERATURE_NSA2300_U61, NSA2300_CMD_REGISTER, &configData, sizeof(configData));
	do
	{
		count++;
		ret = I2c_Reg_Read(POWER_BOARD_TEMPERATURE_NSA2300_U61, NSA2300_CMD_REGISTER, &state, 1);
	} while( ((state & NSA2300_SAMPLE_COMPLETE_MASK) != 0) && (RET_OK == ret) && (count < REPEAT_COUNT_100));

	if (count == REPEAT_COUNT_100)
	{
		ret = RET_GENERAL_ERR;
	}
	ret = I2c_Reg_Read(POWER_BOARD_TEMPERATURE_NSA2300_U61, NSA2300_DATA_MSB_REGISTER, &sfirst_data, 1);
	ret = I2c_Reg_Read(POWER_BOARD_TEMPERATURE_NSA2300_U61, NSA2300_DATA_CSB_REGISTER, &data_s[0], 1);
	ret = I2c_Reg_Read(POWER_BOARD_TEMPERATURE_NSA2300_U61, NSA2300_DATA_LSB_REGISTER, &data_s[1], 1);

	data = (sfirst_data << (8 * 2)) | (data_s[0] << (8 * 1)) | (data_s[1] << (8 * 0));

	if (value != NULL)
	{
		*value = data;
	}
	return ret;
}

int16_t Nsa2300GetNtcTemp(void)
{
	uint16_t resistance;
	uint8_t tempIndex;
	int16_t temp;
	float resist;

	Nsa2300_Ntc_adc_Read(NULL, &resist);
	resistance = (uint16_t)resist;
	tempIndex = LookUpNtcTable(&NTC_TABLE[0], NTC_TABLE_LEN, resistance);
//	temp = tempIndex + FIRST_NTC_TABLE_TEMP;
	temp = tempIndex + FIRST_NTC_TABLE_TEMP + TEMPERATURE_COMPENSATION;
	return temp;
}

ret_code_t Nsa2300_init(void)
{
	uint8_t data;
	ret_code_t ret = RET_OK;
	uint8_t configData;

	configData = 0x12; // 0x12 配置VEXT电压为1.8V, 0x1A 配置VEXT电压为3.6V, ADC进入RawDataM模式
	I2c_Reg_Write(POWER_BOARD_TEMPERATURE_NSA2300_U61, NSA2300_SYS_CONFIG_REGISTER, &configData, sizeof(configData));

	configData = 0x31; // 配置红外传感器通道过采样率 2048，红外传感器通道过运放增益为64X
	I2c_Reg_Write(POWER_BOARD_TEMPERATURE_NSA2300_U61, NSA2300_P_CONFIG_REGISTER, &configData, sizeof(configData));

	configData = 0x81; // 设置使用哪个温度传感器   c1：使用内部温度传感器， 81使用外部传感器,内部无下拉电阻 ， 01使用外部传感器，内接下拉电阻
	I2c_Reg_Write(POWER_BOARD_TEMPERATURE_NSA2300_U61, NSA2300_T_CONFIG_REGISTER, &configData, sizeof(configData));

	// 先获取电路无下拉电阻时即满量程时的ADC值，为后续计算做好准备
	Nsa2300_Ntc_adc_Read(&g_ntc_adc_calibra_full_value, NULL);
	LOG_DEBUG("init read adc value=%d\r\n", g_ntc_adc_calibra_full_value);

	// 配置电路内部下拉电阻 5k
	configData = 0x01; // 设置使用哪个温度传感器   c1：使用内部温度传感器， 81使用外部传感器 ， 01使用外部传感器，内接下拉电阻
	I2c_Reg_Write(POWER_BOARD_TEMPERATURE_NSA2300_U61, NSA2300_T_CONFIG_REGISTER, &configData, sizeof(configData));

	configData = 0x06; // 配置ID号为6
	I2c_Reg_Write(POWER_BOARD_TEMPERATURE_NSA2300_U61, NSA2300_CONFIG_ID_REGISTER, &configData, sizeof(configData));

	return ret;
}

ret_code_t ADS1115_Ntc_adc_Read(uint16_t *value, float *resistance, ads1115InputChannel_t channel)
{
	ret_code_t ret = RET_OK;
	uint8_t data_s[2] = {0};
	int16_t data = 0;
	float resist;
	uint16_t configData;

	double vol_value;

	if (channel == ADS1115_INPUT_CHANNEL_0_J27)
	{
		configData = (CONFIG_REG_AIN0_L << 8) | CONFIG_REG_AIN0_H; //
	}
	else
	{
		configData = (CONFIG_REG_AIN1_L << 8) | CONFIG_REG_AIN1_H; //
	}


	ret = I2c_Reg_Write(POWER_BOARD_TEMPERATURE_ADS1115_U61, ADS1115_CONFIG_REGISTER, (uint8_t*)&configData, sizeof(configData));

	vTaskDelay(10);
	ret = I2c_Reg_Read(POWER_BOARD_TEMPERATURE_ADS1115_U61, ADS1115_CONVERSION_REGISTER, &data_s[0], 2);
	if (ret == RET_OK)
	{
		data = (data_s[0] << 8) | data_s[1];
		if (value != NULL)
		{
			*value = data;
		}

	    if ((data==0x7FFF) | (data==0X8000))//是否超量程了
	    {
	        LOG_ERROR("%s[:%d] fail,over PGA, Adc data=%dr\n", __FUNCTION__, __LINE__, data);
	        return RET_GENERAL_ERR;
	    }

	    vol_value = (data * VOLTAGE_RESOLUTION_62_5_UV)/1000000;

	    resist = (((VCC_3V3 - vol_value) * RESISTANCE_2) / vol_value) - RESISTANCE_1;

		if (resistance != NULL)
		{
			*resistance = resist;
		}

		if (GetPeriodOutPutI2cDeviceInfoFlag() == 1)
		{
			LOG_DEBUG("ads1115,read adc:%d,resistance=%dr\n", data, (uint16_t)resist);
		}

	     if (data <= 0)
	     {
	    	 if (channel == ADS1115_INPUT_CHANNEL_0_J27)
	    	 {
	    		 if (GetPeriodOutPutI2cDeviceInfoFlag() == 1)
	    			 LOG_DEBUG("ads1115,ADS1115_INPUT_CHANNEL_0_J27 have no ntc\r\n");
	    	 }
	    	 else
	    	 {
	    		 if (GetPeriodOutPutI2cDeviceInfoFlag() == 1)
	    			 LOG_DEBUG("ads1115,ADS1115_INPUT_CHANNEL_1_J28 have no ntc\r\n");
	    	 }

	    	 return RET_NO_RESOURCE;
	     }
	}
	else
	{
		LOG_ERROR("%s[:%d] fail,ret=%d\r\n", __FUNCTION__, __LINE__, ret);
		return ret;
	}
	return ret;
}

//int16_t ADS1115GetNtcTemp(ads1115InputChannel_t channel)
//{
//	uint16_t resistance;
//	uint8_t tempIndex;
//	int16_t temp;
//	float resist;
//	ret_code_t ret = RET_OK;
//
//	ret = ADS1115_Ntc_adc_Read(NULL, &resist, channel);
//	if (ret != RET_OK)
//	{
//		temp = 0;
//		return temp;
//	}
//	resistance = (uint16_t)resist;
//	tempIndex = LookUpNtcTable(&NTC_TABLE[0], NTC_TABLE_LEN, resistance);
//	temp = tempIndex + FIRST_NTC_TABLE_TEMP;
////	temp = tempIndex + FIRST_NTC_TABLE_TEMP + TEMPERATURE_COMPENSATION;
//	return temp;
//}

int16_t ADS1115GetNtcTemp(ads1115InputChannel_t channel)
{
	uint16_t resistance;
	uint8_t tempIndex;
	int16_t temp;
	float resist;
	ret_code_t ret = RET_OK;

	ret = ADS1115_Ntc_adc_Read(NULL, &resist, channel);
	if (ret != RET_OK)
	{
		temp = 0;
		return temp;
//		return ads1115tempStatus[channel].resultTemp;
	}
	resistance = (uint16_t)resist;
	tempIndex = LookUpNtcTable(&NTC_TABLE[0], NTC_TABLE_LEN, resistance);
	temp = tempIndex + FIRST_NTC_TABLE_TEMP;
//	temp = tempIndex + FIRST_NTC_TABLE_TEMP + TEMPERATURE_COMPENSATION;

	if (ads1115tempStatus[channel].firstInFlag == 1)
	{
		ads1115tempStatus[channel].resultTemp = temp;
		ads1115tempStatus[channel].lastTemp = temp;
		ads1115tempStatus[channel].firstInFlag = 0;
	}
	else
	{
		if (abs(ads1115tempStatus[channel].lastTemp - temp) >= ADS1115_TEMP_CHANGE_ABNORMAL_THRESHOLD)
		{
		}
		else
		{
			ads1115tempStatus[channel].resultTemp = temp;
		}
		ads1115tempStatus[channel].lastTemp = temp;
	}



	return ads1115tempStatus[channel].resultTemp;
}

//temperature_type_t
ret_code_t GetTemperature(temperature_type_t type, float *getTemp)
{
	ret_code_t ret = RET_OK;
	float temperature;
	uint16_t temp_data  = 0;
	switch (type)
	{
		case core_board_temperature_tmp75:
		{
			ret = Tmp75_Temperature_Read(&temperature);
			break;
		}
		case power_board_temperature_nsa2300_1:
		{
			temperature = (float)Nsa2300GetNtcTemp();
			break;
		}
		case power_board_temperature_ads1115_0:
		{
			temperature = (float)ADS1115GetNtcTemp(ADS1115_INPUT_CHANNEL_0_J27);
			break;
		}
		case power_board_temperature_ads1115_1:
		{
			temperature = (float)ADS1115GetNtcTemp(ADS1115_INPUT_CHANNEL_1_J28);
			break;
		}
		case battery_temperature:
		{
			ret = GetBatteryInfo(BATTERY_TEMPERATURE, &temp_data);
			if (ret == RET_OK)
			{
				temperature = (float)((float)(temp_data) / 10 - 273.15);
			}
			break;
		}
		default:
		{
			ret = RET_GENERAL_ERR;
			break;
		}
	}

	if (ret == RET_OK)
	{
		if (getTemp != NULL)
		{
			*getTemp = temperature;
		}
	}
	return ret;
}

ret_code_t Ads1115_init(void)
{
	ret_code_t ret = RET_OK;
//	uint16_t configData;
//
//	configData = (CONFIG_REG_AINO_H << 8) | CONFIG_REG_AINO_L; //
////	configData = (CONFIG_REG_AIN1_H << 8) | CONFIG_REG_AIN1_L; //
//	ret = I2c_Reg_Write(POWER_BOARD_TEMPERATURE_ADS1115_U61, ADS1115_CONFIG_REGISTER, (uint8_t*)&configData, sizeof(configData));
//	if (ret == RET_OK)
//	{
//		LOG_INFO("ads1115 init ok,config data=0x%x\r\n", configData);
//	}
//	else
//	{
//		LOG_INFO("ads1115 init fail\r\n");
//	}
	return ret;
}

void GetAds1115ConfigTest(void)
{
	ret_code_t ret = RET_OK;
	uint16_t configData;
	ret = I2c_Reg_Read(POWER_BOARD_TEMPERATURE_ADS1115_U61, ADS1115_CONFIG_REGISTER, (uint8_t*)&configData, sizeof(configData));
	if (ret == RET_OK)
	{
		LOG_DEBUG("ads1115 config=0x%x\r\n", configData);
	}
	else
	{
		LOG_DEBUG("ads1115 read fail\r\n");
	}

}

