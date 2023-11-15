
#include "FreeRTOS.h"
#include "task.h"
#include "protocol_dbgdat.h"

static protocol_adc_data_t g_adc_data_dft;

protocol_adc_data_t *protocol_adcpkg_get_default_buffer(void)
{
	return &g_adc_data_dft;
}

ret_code_t protocol_send_adc_data(protocol_adc_data_t *adc_data)
{
	ret_code_t ret = RET_OK;

	if (adc_data == NULL)
	{
		ret = RET_INVALID_PARAM;
		goto out;
	}

	ret = protocol_udp_send_data_nonblocking(adc_data, sizeof(*adc_data), NULL);

out:
	LOG_DEBUG("[%s:%d] ret=%d\r\n", __FUNCTION__, __LINE__, ret);
	return ret;
}

ret_code_t protocol_adcpkg_set_head(protocol_adc_data_t *adc_data, uint32_t frameId, uint32_t length)
{
	ret_code_t ret = RET_OK;
	uint32_t localOscFreq = 0;

	if ((adc_data == NULL) || (length != RAW_ADC_DATA_LEN_PER_FRAME))
	{
		ret = RET_INVALID_PARAM;
		goto out;
	}

	adc_data->stInfoHeader.infoSync = htonl(INFO_HEAD_FLAG);
	adc_data->stInfoHeader.infoLength = htonl(sizeof(*adc_data));
	adc_data->stInfoHeader.frameID = htonl(frameId);
	adc_data->stInfoHeader.timestamp = htonl(xTaskGetTickCount() / portTICK_PERIOD_MS);
	adc_data->stInfoHeader.infoType = htons(PIT_ADC_DATA);
	adc_data->stInfoHeader.terminalID = htons(0);
	adc_data->stInfoHeader.terminalType = 0x01;
	adc_data->stInfoHeader.subTerminalType = 0;
	adc_data->stInfoHeader.infoVersion = PROTOCOL_ADC_DATA_VER;

	adc_data->length = htonl(sizeof(adc_data->rawData));
	localOscFreq = aeagCfg_GetPlSignalHandlingLocalOscFreq();
	adc_data->localOscFreq = htonl(localOscFreq);

	// note: CRC is not used to save time here

out:
	return ret;
}
