/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* FreeRTOS+CLI includes. */
#include "../../srv/cli/cli_if.h"
#include "ut_led_ctrl.h"

#include "../../app/data_path/data_path.h"
#include "../../app/data_path/data_path_droneid.h"
#include "../../app/alg/droneID/droneID_utils.h"
#include "../../app/rf_config/rf_config.h"
#include "../../app/switch_filter/switch_filter.h"
#include "../../drv/flash/flash.h"
#include "../../drv/usb/xusb_cdc_acm_api.h"
#include "../../hal/flash_hal/flash_hal.h"
#include "../../hal/hal.h"
#include "../../hal/hal_ad9361/ad9361_spi.h"
#include "../../hal/hal_ad9361/ad9361_config.h"
#include "../../srv/log/log.h"
#include "../../cfg/shrd_config.h"
#include "../../hal/input/input.h"
#include "../../hal/output/output.h"
#include "../../srv/flash_info/cali_para_config.h"
#include "../../srv/flash_nv/flash_nv.h"
#include "../../srv/protocol/protocol_common.h"


BaseType_t SetIrdRate_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	s32 type = -1;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if (len1 > 0)
	{
		type = strtoul(param1, NULL, 0);

		axi_write_data( 0x10, type );
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return pdFALSE;
}

BaseType_t ad_start_enable_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	s32 type = -1;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if (len1 > 0)
	{
		type = strtoul(param1, NULL, 0);
		if (1 == type)
		{
			axi_write_data(0x01, 1);
		}
		else if (0 == type)
		{
			axi_write_data(0x01, 0);
		}

		if ((1 == type) || (0 == type))
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "OK");
		}
		else
			snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}
	else
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");

	return pdFALSE;
}

BaseType_t ad9361_cfg_f_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	s32 type = -1;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if (len1 > 0)
	{
		type = strtoul(param1, NULL, 0);
		Rf_ConfigTask_Post(type);
	}
	else
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");

	return pdFALSE;
}

BaseType_t ADRF5250_RF_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	s32 type = -1;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if (len1 > 0)
	{
		type = strtoul(param1, NULL, 0);
		Adrf5250Ctrl((uint8_t)type);
	}
	else
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");

	return pdFALSE;
}

BaseType_t SERNIN_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	s32 type = -1;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if (len1 > 0)
	{
		type = strtoul(param1, NULL, 0);
		SetSwitchFilterGain(type);
	}
	else
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");

	return pdFALSE;
}

BaseType_t SetAD9361Gpio_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	s32 type = -1;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if (len1 > 0)
	{
		type = strtoul(param1, NULL, 0);
		SetAD9361Gpio(type);
	}
	else
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");

	return pdFALSE;
}


BaseType_t SetSweepFlag_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	s32 type = -1;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if (len1 > 0)
	{
		type = strtoul(param1, NULL, 0);
		SetSweepFlag(type);
	}
	else
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");

	return pdFALSE;
}

BaseType_t ut_SetAd9361Gain_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	s32 type = -1;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if (len1 > 0)
	{
		type = strtoul(param1, NULL, 0);
		SetAd9361Gain(type,type);
		snprintf(pcWriteBuffer, xWriteBufferLen, "OK");
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return pdFALSE;
}

BaseType_t ut_GetAd9361Reg_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	s32 type = -1;
	u16 addr;
	u8 data = 0x00 ;

	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if (len1 > 0)
	{
		type = strtoul(param1, NULL, 0);
		addr = type&0xffff;
		data = ad9361_spi_read_byte(addr);
		snprintf(pcWriteBuffer, xWriteBufferLen, "OK! addr 0x%x value = 0x%x\r\n", addr , data );
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return pdFALSE;
}



BaseType_t ut_SetAd9361GainAGC_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	s32 type = -1;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if (len1 > 0)
	{
		type = strtoul(param1, NULL, 0);
		SetAd9361GainAGC(type);
		snprintf(pcWriteBuffer, xWriteBufferLen, "OK");
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return pdFALSE;
}

BaseType_t ut_SetAd9361GainCutover_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0, len2 = 0, len3 = 0;
	s32 type = -1, data1, data2;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
	const char *param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &len2);
	const char *param3 = FreeRTOS_CLIGetParameter(pcCommandString, 3, &len3);

	if ((len1 > 0) && (len2 > 0) && (len3 > 0))
	{
		type = strtoul(param1, NULL, 0);
		data1 = strtoul(param2, NULL, 0);
		data2 = strtoul(param3, NULL, 0);
		SetAd9361GainCutoverFlag((uint8_t)type);
		SetAd9361GainCutoverValue((uint8_t)data1, (uint8_t)data2);
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return pdFALSE;
}

BaseType_t SetSwitchOutputChl_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	s32 type = -1;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if (len1 > 0)
	{
		type = strtoul(param1, NULL, 0);
		SetSwitchOutputChl(type);
		if (type)
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "OK");
		}
		else
			snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}
	else
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");

	return pdFALSE;
}

BaseType_t SetOutputSpecMat_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	s32 type = -1;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if (len1 > 0)
	{
		type = strtoul(param1, NULL, 0);
		SetOutputSpecMat(type);
		snprintf(pcWriteBuffer, xWriteBufferLen, "OK");
	}
	else
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");

	return pdFALSE;
}

BaseType_t SetBurstCkeckEn_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	s32 type = -1;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if (len1 > 0)
	{
		type = strtoul(param1, NULL, 0);
		SetBurstCheckEn(type);
		snprintf(pcWriteBuffer, xWriteBufferLen, "OK");
	}
	else
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");

	return pdFALSE;
}
BaseType_t SetIDelay_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	s32 type = -1;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);


	if (len1 > 0)
	{
		type = strtoul(param1, NULL, 0);
		axi_write_data(0x09, 0x00);
		axi_write_data(0x0B, type);
		axi_write_data(0x0A, 0x7F);
		axi_write_data(0x0A, 0x00);
		axi_write_data(0x09, 0x7F);
		LOG_PRINTF("value: %s\n", type);
		snprintf(pcWriteBuffer, xWriteBufferLen, "OK");
	}
	else
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");

	return pdFALSE;
}

BaseType_t IoCtrl_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0, len2 = 0;
	uint16_t reg_name = 0;
	uint32_t value = 0, rev = 0;;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
	const char *param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &len2);

	if ((len1 > 0) && (len2 > 0))
	{
		reg_name = strtoul(param1, NULL, 0);
		value = strtoul(param2, NULL, 0);
		GPIO_OutputCtrl(reg_name, value);
		rev = Output_GetValue(reg_name);
		LOG_PRINTF("reg_name:%d value: %d rev:%d\n",reg_name, value, rev);
		snprintf(pcWriteBuffer, xWriteBufferLen, "OK");
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return pdFALSE;
}

BaseType_t GetOutputIo_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	uint16_t reg_name = 0;
	uint32_t value = 0;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if (len1 > 0)
	{
		reg_name = strtoul(param1, NULL, 0);
		value = Output_GetValue(reg_name);
		LOG_PRINTF("reg_name:%d value: %d rev:%d\n",reg_name, value);
		snprintf(pcWriteBuffer, xWriteBufferLen, "OK");
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return pdFALSE;
}

BaseType_t GetInputIo_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	uint16_t reg_name = 0;
	uint32_t value = 0;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if (len1 > 0)
	{
		reg_name = strtoul(param1, NULL, 0);
		value = Input_GetValue(reg_name);
		LOG_PRINTF("reg_name:%d value: %d rev:%d\n",reg_name, value);
		snprintf(pcWriteBuffer, xWriteBufferLen, "OK");
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return pdFALSE;
}

static BaseType_t ReadDDRAddr_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
    BaseType_t len1 = 0, len2 = 0;
    uint8_t *buf = GetAdcBufDroneid();
    uint32_t addr, len, index;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
	const char *param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &len2);

	if ((len1 > 0) && (len2 > 0))
	{
//		addr = strtoul(param1, NULL, 0);
//		len = strtoul(param2, NULL, 0);
		snprintf(pcWriteBuffer, xWriteBufferLen, "addr 0x%X", buf);

		if (buf)
		{
			Usb_Send_data_func((uint8_t*)"################", 16);
			Usb_Send_data_func((uint8_t*)buf, ADC_MAX_LEN_DRONEID);
			Usb_Send_data_func((uint8_t*)"&&&&&&&&&&&&&&&&", 16);
		}
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

    return pdFALSE;
}

static BaseType_t ReadDDRAddrIndex_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
    BaseType_t len1 = 0, len2 = 0, len3 = 0;
    uint32_t index, addr, len;
    uint64_t uAddr = 0;
    uint16_t cnt = 0;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
	const char *param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &len2);
	const char *param3 = FreeRTOS_CLIGetParameter(pcCommandString, 3, &len3);

	if ((len1 > 0) && (len2 > 0) && (len3 > 0))
	{
		index = strtoul(param1, NULL, 0);
		addr = strtoul(param2, NULL, 0);
		len = strtoul(param3, NULL, 0);
		uAddr = index * 0x10000000 + addr;
		snprintf(pcWriteBuffer, xWriteBufferLen, "index %d addr 0x%X, len %d",index, addr, len);


		if ((len % 10000000) == 0)
		{
			cnt = len / 10000000;
		}
		else
		{
			cnt = len / 10000000 + 1;
		}

		for (uint16_t i = 0; i < cnt; i++)
		{
			protocol_udp_send_data2((uint32_t*)uAddr, len);
			vTaskDelay(10);
		}
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

    return pdFALSE;
}

static BaseType_t UartReadDDRAddrIndex_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
    BaseType_t len1 = 0, len2 = 0, len3 = 0;
    uint32_t index, addr, len;
    uint64_t uAddr = 0;
    uint16_t cnt = 0;
	uint16_t posIdx = 0;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
	const char *param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &len2);
	const char *param3 = FreeRTOS_CLIGetParameter(pcCommandString, 3, &len3);

	if ((len1 > 0) && (len2 > 0) && (len3 > 0))
	{
		index = strtoul(param1, NULL, 0);
		addr = strtoul(param2, NULL, 0);
		len = strtoul(param3, NULL, 0);
		uAddr = index * 0x10000000 + addr;
		snprintf(pcWriteBuffer, xWriteBufferLen, "index %d addr 0x%X, len %d",index, addr, len);

		cnt = len % 10000000;
		for (uint16_t i = 0; i < cnt; i++)
		{
			posIdx = (uint16_t)((*(uint64_t *)(uAddr + i * 8) >> 32) & 0xFFFF);
			LOG_PRINTF("%4X ", posIdx);
		}
		LOG_PRINTF("%\r\n");
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

    return pdFALSE;
}

static BaseType_t SetuAlgorithmRunFlag_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
    BaseType_t len1 = 0;
    uint32_t val = 0;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if (len1 > 0)
	{
		val = strtoul(param1, NULL, 0);
		SetAlgorithmRunFlag((uint8_t)val);
		snprintf(pcWriteBuffer, xWriteBufferLen, "OK  %d", val);
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

    return pdFALSE;
}

static BaseType_t SetDetectFreqItem_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	s32 freqItem = -1;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if (len1 > 0)
	{
		freqItem = strtoul(param1, NULL, 0);
		if ((freqItem >= 0) && (freqItem < Freq_MAX_CNT))
		{
			SetDetectFreqItem(freqItem);
			snprintf(pcWriteBuffer, xWriteBufferLen, "OK, freq item: %d", freqItem);
		}
		else
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR, freq item: %d, not valid", freqItem);
		}
	}
	else
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");

	return pdFALSE;
}



static const CLI_Command_Definition_t SetSweepFlag_cmd =
	{
		"SetSweepFlag",
		"\r\nSetSweepFlag <type>:\r\n type:0 for disable ,1 for enable \r\n",
		SetSweepFlag_handler,
		1};

static const CLI_Command_Definition_t ad9361_cfg_f =
	{
		"ad9361_cfg_f",
		"\r\nad9361_cfg_f :\r\n",
		ad9361_cfg_f_handler,
		1};

static const CLI_Command_Definition_t enable_ad936x_f =
	{
		"SetAD9361Gpio",
		"\r\nenable_ad936x <type>:\r\n type:0 for disable ,1 for enable \r\n",
		SetAD9361Gpio_handler,
		1};

static const CLI_Command_Definition_t ADRF5250_RF_enable =
	{
		"ADRF5250_RF",
		"\r\nADRF5250_RF <type>: 1-5 RF channel\r\n",
		ADRF5250_RF_handler,
		1};

static const CLI_Command_Definition_t SERNIN_enable =
	{
		"SERNIN",
		"\r\nSERNIN <type>: 0-8 Attenuation assignment\r\n",
		SERNIN_handler,
		1};


static const CLI_Command_Definition_t ut_SetAd9361Gain_ctrl =
	{
		"SetAd9361Gain",
		"\r\nSetAd9361Gain <value>: Ad9361 Gain\r\n",
		ut_SetAd9361Gain_handler,
		1};

static const CLI_Command_Definition_t ut_GetAd9361Reg_ctrl =
	{
		"GetAd9361Reg",
		"\r\nGetAd9361Reg <addr>: Get Ad9361 register value\r\n",
		ut_GetAd9361Reg_handler,
		1};

static const CLI_Command_Definition_t ut_SetAd9361GainAGC_ctrl =
	{
		"SetAd9361GainAGC",
		"\r\nSetAd9361GainAGC <value>: Ad9361 Gain\r\n",
		ut_SetAd9361GainAGC_handler,
		1};

static const CLI_Command_Definition_t ut_SetAd9361GainCutover_ctrl =
	{
		"SetAd9361GainCutover",
		"\r\nSetAd9361GainCutover <mode> <value1> <value2>: <mode> 1 open 0 close  <value1> Gain <value2> Gain\r\n",
		ut_SetAd9361GainCutover_handler,
		3};

static const CLI_Command_Definition_t SetSwitchOutputChl_cmd =
	{
		"SetSwitchOutputChl",
		"\r\nSetSwitchOutputChl <type>:\r\n type:0 ps printf ,1 pl log printf \r\n",
		SetSwitchOutputChl_handler,
		1};

static const CLI_Command_Definition_t SetOutputSpecMat_cmd =
	{
		"SetOutputSpecMat",
		"\r\nSetOutputSpecMat <type>:0:disable  1:enable\r\n  \r\n",
		SetOutputSpecMat_handler,
		1};

static const CLI_Command_Definition_t SetBurstCkeckEn_cmd =
	{
		"SetBurstCkeckEn",
		"\r\nSetBurstCkeckEn <type>:0:disable  1:enable\r\n  \r\n",
		SetBurstCkeckEn_handler,
		1};

static const CLI_Command_Definition_t SetIDelay_cmd =
	{
		"SetIDelay",
		"\r\n SetIDelay <type>: value\r\n  \r\n",
		SetIDelay_handler,
		1};

static const CLI_Command_Definition_t IoCtrl_cmd =
	{
		"IoCtrl",
		"\r\n IoCtrl <pin> <value>: value\r\n  \r\n",
		IoCtrl_handler,
		2};

static const CLI_Command_Definition_t GetOutputIo_cmd =
	{
		"GetOutputIo",
		"\r\n IoCtrl <pin>:\r\n  \r\n",
		GetOutputIo_handler,
		1};

static const CLI_Command_Definition_t GetInputIo_cmd =
	{
		"GetInputIo",
		"\r\n IoCtrl <pin>:\r\n  \r\n",
		GetInputIo_handler,
		1};

static const CLI_Command_Definition_t ReadDDRAddr_cmd =
{
    "ReadDDRAddr",
    "\r\nReadDDRAddr <Addr> <Len>:\r\n Read address and len\r\n",
	ReadDDRAddr_handler,
    2
};

static const CLI_Command_Definition_t ReadDDRAddrIndex_cmd =
{
    "ReadDDRAddrIndex",
    "\r\nReadDDRAddrIndex <Index> <Addr> <Len>:\r\n Index:value*0x10000000 + Addr and len\r\n",
	ReadDDRAddrIndex_handler,
    3
};

static const CLI_Command_Definition_t UartReadDDRAddrIndex_cmd =
{
    "UartReadDDRAddrIndex",
    "\r\nReadDDRAddrIndex <Index> <Addr> <Len>:\r\n Index:value*0x10000000 + Addr and len\r\n",
	UartReadDDRAddrIndex_handler,
    3
};


static const CLI_Command_Definition_t SetDetectFreqItem_cmd =
{
	"SetDetectFreqItem",
	"\r\nSetDetectFreqItem <item>:\r\n freq item: 0 to (Freq_MAX_CNT-1) \r\n",
	SetDetectFreqItem_handler,
	1
};

static const CLI_Command_Definition_t SetuAlgorithmRunFlag_cmd =
{
	"SetuAlgorithmRunFlag",
	"\r\SetuAlgorithmRunFlag <value>:\r\n 2:output droneid origin data\r\n",
	SetuAlgorithmRunFlag_handler,
	1
};

void ut_led_ctrl_init(void)
{
	FreeRTOS_CLIRegisterCommand(&SetSweepFlag_cmd);
	FreeRTOS_CLIRegisterCommand(&ad9361_cfg_f);
	FreeRTOS_CLIRegisterCommand(&ADRF5250_RF_enable);
	FreeRTOS_CLIRegisterCommand(&SERNIN_enable);
	FreeRTOS_CLIRegisterCommand(&enable_ad936x_f);
	FreeRTOS_CLIRegisterCommand(&ut_SetAd9361Gain_ctrl);
	FreeRTOS_CLIRegisterCommand(&ut_GetAd9361Reg_ctrl);
	FreeRTOS_CLIRegisterCommand(&ut_SetAd9361GainAGC_ctrl);
	FreeRTOS_CLIRegisterCommand(&ut_SetAd9361GainCutover_ctrl);
	FreeRTOS_CLIRegisterCommand(&SetSwitchOutputChl_cmd);
	FreeRTOS_CLIRegisterCommand(&SetOutputSpecMat_cmd);
	FreeRTOS_CLIRegisterCommand(&SetBurstCkeckEn_cmd);
	FreeRTOS_CLIRegisterCommand(&SetIDelay_cmd);
	FreeRTOS_CLIRegisterCommand(&IoCtrl_cmd);
	FreeRTOS_CLIRegisterCommand(&GetOutputIo_cmd);
	FreeRTOS_CLIRegisterCommand(&GetInputIo_cmd);
	FreeRTOS_CLIRegisterCommand(&ReadDDRAddr_cmd);
	FreeRTOS_CLIRegisterCommand(&ReadDDRAddrIndex_cmd);
	FreeRTOS_CLIRegisterCommand(&UartReadDDRAddrIndex_cmd);
	FreeRTOS_CLIRegisterCommand(&SetuAlgorithmRunFlag_cmd);
}
