/*
 * extended_io.c
 *
 *  Created on: 2023年4月18日
 *      Author: A22745
 */

#include "extended_io.h"
#include "common_i2c.h"
#include "../../srv/log/log.h"


static uint8_t extend_io_input_register;
static uint8_t extend_io_output_register;


static ret_code_t ExtendedIoWrite(extendedIoRegisterAddr_e reg, uint8_t data)
{
	uint8_t w_data = data;
	ret_code_t ret = RET_OK;
	ret = I2c_Reg_Write(EXTENDED_IO, reg, &w_data, sizeof(w_data));
	return ret;
}

static ret_code_t ExtendedIoRead(extendedIoRegisterAddr_e reg, uint8_t *data)
{
	ret_code_t ret = RET_OK;
	uint8_t rData;

	ret = I2c_Reg_Read(EXTENDED_IO, reg, &rData, sizeof(rData));

	if (data != NULL)
	{
		*data = rData;
	}

	return ret;
}

//void UpdataExtendIoInputAndOutput(void)
//{
//	ExtendedIoRead(EXTENDED_IO_INPUT_REGISTER, &extend_io_input_register);
//	ExtendedIoWrite(EXTENDED_IO_OUTPUT_REGISTER, extend_io_output_register);
//}

static uint8_t GetExtendIoInput(void)
{
	ExtendedIoRead(EXTENDED_IO_INPUT_REGISTER, &extend_io_input_register);
	return extend_io_input_register;
}

static uint8_t GetExtendIoOutput(void)
{
//	uint8_t data;
//	ExtendedIoRead(EXTENDED_IO_OUTPUT_REGISTER, &data);
//	return data;
	return extend_io_output_register;
}

static void SetExtendIoOutput(uint8_t data)
{
	extend_io_output_register = data;
	ExtendedIoWrite(EXTENDED_IO_OUTPUT_REGISTER, extend_io_output_register);
}

extend_io_level_e GetSpecifyExtendIo(extendIoInputType_e type)
{
	extend_io_level_e ret = EXTEND_IO_HIGH_LEVEL;
	uint8_t inputData;
	uint8_t maskBit = 0;
	inputData = GetExtendIoInput();
	switch (type)
	{
		case EXTEND_IO_INPUT_VSENSE_ALT00:
		{
			maskBit = VSENSE_ALT00_BIT_0;
			break;
		}
		case EXTEND_IO_INPUT_VSENSE_ALT01:
		{
			maskBit = VSENSE_ALT01_BIT_1;
			break;
		}
		case EXTEND_IO_INPUT_VSENSE_ALT02:
		{
			maskBit = VSENSE_ALT02_BIT_2;
			break;
		}
		case EXTEND_IO_INPUT_VDC_PLUG_IN:
		{
			maskBit = VDC_PLUG_IN_BIT_3;
			break;
		}
		case EXTEND_IO_INPUT_VCHG_ON:
		{
			maskBit = CHG_ON_BIT_4;
			break;
		}
		default:
		{
			LOG_ERROR("%s[:%d], paramer invalid\r\n", __FUNCTION__, __LINE__);
			break;
		}
	}

	if (inputData & maskBit)
	{
		ret = EXTEND_IO_HIGH_LEVEL;
	}
	else
	{
		ret = EXTEND_IO_LOW_LEVEL;
	}

	return ret;
}

void SetSpecifyExtendIo(extendIoOutputType_e type, extend_io_level_e level)
{
	uint8_t outputData;
	uint8_t maskBit = 0;
	outputData = GetExtendIoOutput();

	switch (type)
	{
		case EXTEND_IO_OUTPUT_LED_1_TYPE:
		{
			maskBit = LED_1_EN_BIT_6;
			break;
		}
		case EXTEND_IO_OUTPUT_LED_2_TYP:
		{
			maskBit = LED_2_EN_BIT_5;
			break;
		}
		case EXTEND_IO_OUTPUT_CHARG_EN_TYP:
		{
			maskBit = CHG_EN_BIT_7;
			break;
		}
		default:
		{
			LOG_ERROR("%s[:%d], paramer invalid\r\n", __FUNCTION__, __LINE__);
			return;
		}
	}

	if (level == EXTEND_IO_HIGH_LEVEL)
	{
		outputData |=  maskBit;
	}
	else
	{
		outputData &=  ~(maskBit);
	}

	SetExtendIoOutput(outputData);

}

void ExtendedIoInit(void)
{
	// 配置 扩展IO 为对应输入输出
	ExtendedIoWrite(EXTENDED_IO_CONFIG_REGISTER, EXTENDED_IO_INPUT_OUTPUT_CONFIG);

	// 上电把LED1和LED2电源指示灯点亮
	SetSpecifyExtendIo(EXTEND_IO_OUTPUT_LED_1_TYPE, EXTEND_IO_HIGH_LEVEL);
	SetSpecifyExtendIo(EXTEND_IO_OUTPUT_LED_2_TYP, EXTEND_IO_HIGH_LEVEL);
}

void ExtendedIoTest(void)
{
	uint8_t input;
	static uint8_t i = 0;
	uint8_t output = 0;
	output = LED_2_EN_BIT_5 | LED_1_EN_BIT_6 | CHG_EN_BIT_7;

	input = GetExtendIoInput();

	LOG_DEBUG("input=%d=0x%x\r\n", input, input);
	i++;
	if ((i % 2) == 0)
	{
		output = ~output;
	}
	SetExtendIoOutput(output);
	LOG_DEBUG("output=%d=0x%x\r\n", output, output);

}
