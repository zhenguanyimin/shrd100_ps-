
#include "ut_protocol.h"

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

#include "../../srv/log/log.h"

/* Service module includes. */
#include "../../srv/protocol/protocol_if.h"

static BaseType_t adc_data_test_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);

protocol_adc_data_t g_adc_data_test;
static uint32_t s_mockFrameId = 0;

static const CLI_Command_Definition_t adc_data_test_cmd =
	{
		"AdcDataTest",
		"\r\nAdcDataTest [<LoopCnt> <InervalTime> <FixVal>]:\r\n Send mock adc data by udp\r\n",
		adc_data_test_cmd_handler,
		-1};

static void adc_data_test_fill_data(uint32_t fixVal)
{
	uint16_t crc = 0;
	uint32_t i = 0;
	TickType_t startTime = 0, endTime = 0, costTime = 0;
	uint32_t localOscFreq = 0;

	startTime = xTaskGetTickCount();
	g_adc_data_test.stInfoHeader.infoSync = htonl(INFO_HEAD_FLAG);
	g_adc_data_test.stInfoHeader.infoLength = htonl(sizeof(g_adc_data_test));
	// g_adc_data_test.stInfoHeader.frameID = htonl(s_mockFrameId++);
	g_adc_data_test.stInfoHeader.timestamp = htonl(xTaskGetTickCount() / portTICK_PERIOD_MS);
	g_adc_data_test.stInfoHeader.infoType = htons(PIT_ADC_DATA);
	g_adc_data_test.stInfoHeader.terminalID = htons(1);
	g_adc_data_test.stInfoHeader.terminalType = 0x01;
	g_adc_data_test.stInfoHeader.subTerminalType = 0;
	g_adc_data_test.stInfoHeader.infoVersion = PROTOCOL_ADC_DATA_VER;
	endTime = xTaskGetTickCount();
	costTime = endTime - startTime;
	LOG_PRINTF("[%s:%d] startTime=%lu, endTime=%lu, costTime=%lu\r\n", __FUNCTION__, __LINE__, startTime, endTime, costTime);

	g_adc_data_test.length = htonl(sizeof(g_adc_data_test.rawData));
	localOscFreq = aeagCfg_GetPlSignalHandlingLocalOscFreq();
	g_adc_data_test.localOscFreq = htonl(localOscFreq);
	startTime = xTaskGetTickCount();
	for (i = 0; i < ARRAY_SIZE(g_adc_data_test.rawData); i++)
	{
		g_adc_data_test.rawData[i] = fixVal > 0 ? fixVal : (i + (i << 16));
	}
	endTime = xTaskGetTickCount();
	costTime = endTime - startTime;
	LOG_PRINTF("[%s:%d] startTime=%lu, endTime=%lu, costTime=%lu\r\n", __FUNCTION__, __LINE__, startTime, endTime, costTime);

	startTime = xTaskGetTickCount();
	crc = crc_16bits_compute((uint8_t *)&g_adc_data_test, sizeof(g_adc_data_test) - sizeof(g_adc_data_test.stInfoTail));
	endTime = xTaskGetTickCount();
	costTime = endTime - startTime;
	LOG_PRINTF("[%s:%d] startTime=%lu, endTime=%lu, costTime=%lu\r\n", __FUNCTION__, __LINE__, startTime, endTime, costTime);

	g_adc_data_test.stInfoTail.crc = htons(crc);
}

static BaseType_t adc_data_test_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0, len2 = 0, len3 = 0;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
	const char *param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &len2);
	const char *param3 = FreeRTOS_CLIGetParameter(pcCommandString, 3, &len3);
	uint32_t loopCnt = 1, inervalTime = 8;
	uint32_t fixVal = 0;
	TickType_t startTime = 0, endTime = 0, costTime = 0, costMs = 0;
	TickType_t costMsMin = 0, costMsMax = 0, costMsAvr = 0;
	uint64_t costMsTotal = 0;
	ret_code_t ret = RET_OK;
	uint32_t i = 0;

#define LOOP_CNT_MAX (10000)
#define INTERVAL_TIME_MIN (3)

	if (len1 > 0)
	{
		loopCnt = strtoul(param1, NULL, 0);
		loopCnt = loopCnt > LOOP_CNT_MAX ? LOOP_CNT_MAX : loopCnt;
	}

	if (len2 > 0)
	{
		inervalTime = strtoul(param2, NULL, 0);
		inervalTime = inervalTime < INTERVAL_TIME_MIN ? INTERVAL_TIME_MIN : inervalTime;
	}

	if (len3 > 0)
	{
		fixVal = strtoul(param3, NULL, 0);
	}

	adc_data_test_fill_data(fixVal);
	for (i = 0; i < loopCnt; i++)
	{
		g_adc_data_test.stInfoHeader.frameID = htonl(s_mockFrameId++);
		startTime = xTaskGetTickCount();
		ret = protocol_send_adc_data(&g_adc_data_test);
		if (ret != RET_OK)
		{
			if (i > 1)
			{
				i--;
			}
			break;
		}
		endTime = xTaskGetTickCount();
		costTime = endTime - startTime;
		costMs = costTime / portTICK_PERIOD_MS;
		if (i == 0)
		{
			costMsMin = costMs;
			costMsMax = costMs;
		}
		else
		{
			costMsMin = costMsMin > costMs ? costMs : costMsMin;
			costMsMax = costMsMax < costMs ? costMs : costMsMax;
		}
		costMsTotal += costMs;
		vTaskDelay(inervalTime);
	}

	if (i > 0)
	{
		costMsAvr = (TickType_t)(costMsTotal / i);
	}

	if (ret == RET_OK)
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "OK\r\ncostMsMin=%lu, costMsMax=%lu, costMsAvr=%lu",
				 costMsMin, costMsMax, costMsAvr);
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return pdFALSE;
}

void ut_protocol_init(void)
{
	FreeRTOS_CLIRegisterCommand(&adc_data_test_cmd);
}
