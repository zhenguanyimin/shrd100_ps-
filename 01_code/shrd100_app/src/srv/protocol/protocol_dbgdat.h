
#ifndef PROTOCOL_DBGDAT_H
#define PROTOCOL_DBGDAT_H

#include "protocol_common.h"

#define TX_CNT_MAX (32)
#define RAW_ADC_DATA_LEN_PER_FRAME (256 * 1024)

#define PROTOCOL_ADC_DATA_VER (0x00)

#pragma pack(1)
typedef struct protocol_adc_data
{
	protocol_info_head_t stInfoHeader;

	uint32_t length;	   // total length of the ADC data
	uint32_t localOscFreq; // localOscFreq
	union
	{
		uint32_t rawData[RAW_ADC_DATA_LEN_PER_FRAME / sizeof(uint32_t) * TX_CNT_MAX]; // the ADC raw data
		uint8_t rawBuf[RAW_ADC_DATA_LEN_PER_FRAME];
	};

	protocol_info_tail_t stInfoTail;
} protocol_adc_data_t;
#pragma pack()

protocol_adc_data_t *protocol_adcpkg_get_default_buffer(void);

ret_code_t protocol_send_adc_data(protocol_adc_data_t *adc_data);

ret_code_t protocol_adcpkg_set_head(protocol_adc_data_t *adc_data, uint32_t frameId, uint32_t length);

#endif /* PROTOCOL_DBGDAT_H */
