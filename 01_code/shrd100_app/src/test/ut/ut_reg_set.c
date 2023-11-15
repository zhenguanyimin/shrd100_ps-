
#include "ut_reg_set.h"

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
#include "../../hal/output/output.h"
#include "../../hal/smbus_driver/smbus_driver.h"
#include "../../hal/flash_hal/flash_hal.h"
#include "../../inc/common.h"
#include "../../srv/flash_info/cali_para_config.h"
#include "../../srv/log/log.h"


static BaseType_t WriteAxiAddr_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0, len2 = 0;
	uint32_t reg_name = 0;
	uint32_t value = 0;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
	const char *param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &len2);

	if ((len1 > 0) && (len2 > 0))
	{
		reg_name = strtoul(param1, NULL, 0);
		value = strtoul(param2, NULL, 0);
		axi_write(reg_name, value);
		snprintf(pcWriteBuffer, xWriteBufferLen, "OK");
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return pdFALSE;
}

static BaseType_t ReadAxiAddr_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	uint32_t addr = 0;
	uint32_t reg_val = 0;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if (len1 > 0)
	{
		addr = strtoul(param1, NULL, 0);
		reg_val = axi_read(addr);
		snprintf(pcWriteBuffer, xWriteBufferLen, "The Addr value is 0x%X", reg_val);
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return pdFALSE;
}


static BaseType_t get_frequency_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	uint16_t reg_num = 0;
	uint32_t reg_val = 0;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if (len1 > 0)
	{
		reg_num = strtoul(param1, NULL, 0);
		reg_val = axi_read_data(reg_num);
		snprintf(pcWriteBuffer, xWriteBufferLen, "The register value is 0x%lx", reg_val);
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return pdFALSE;
}

static BaseType_t set_frequency_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0, len2 = 0;
	uint16_t reg_name = 0;
	uint32_t value = 0;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
	const char *param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &len2);

	if ((len1 > 0) && (len2 > 0))
	{
		reg_name = strtoul(param1, NULL, 0);
		value = strtoul(param2, NULL, 0);
		axi_write_data(reg_name, value);
		snprintf(pcWriteBuffer, xWriteBufferLen, "OK");
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return pdFALSE;
}


static const CLI_Command_Definition_t ReadAxiAddr_command =
{
		"ReadAxiAddr",
		"\r\nReadAxiAddr <Addr>:\r\n Read the value of the axi address\r\n",
		ReadAxiAddr_handler,
		1
};

static const CLI_Command_Definition_t WriteAxiAddr_command =
{
		"WriteAxiAddr",
		"\r\nWriteAxiAddr <Addr> <Value>:\r\n Write the axi address value\r\n",
		WriteAxiAddr_handler,
		2
};
static const CLI_Command_Definition_t get_fre_command =
	{
		"getFreCmd",
		"\r\ngetFreCmd <register_ID>:\r\n Get single register value of frequency from flash\r\n",
		get_frequency_command_handler,
		1};

static const CLI_Command_Definition_t set_fre_command =
	{
		"setFreCmd",
		"\r\nsetFreCmd <register> <value>:\r\n Set register value of frequency\r\n",
		set_frequency_command_handler,
		2};

int ut_set_reg_init(void)
{
	int ret = RET_OK;
//
	FreeRTOS_CLIRegisterCommand(&get_fre_command);
	FreeRTOS_CLIRegisterCommand(&set_fre_command);
	FreeRTOS_CLIRegisterCommand(&ReadAxiAddr_command);
	FreeRTOS_CLIRegisterCommand(&WriteAxiAddr_command);

	return ret;
}
