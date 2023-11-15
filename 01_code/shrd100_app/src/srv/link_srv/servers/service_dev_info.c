/*
 * service_getBoardSN.c
 *
 *  Created on: 2022骞�11鏈�2鏃�
 *      Author: A22745
 */

#include "../../../srv/flash_nv/flash_nv.h"
#include "service_dev_info.h"
#include "../../../cfg/shrd_config.h"
#include "../../../cfg/version.h"
#include "../../../hal/hal.h"
#include "../../../drv/fc41d/bt_wifi.h"


#define SN_LENGTH 25

uint16_t Service_GetBoardSN_fun(alink_msg_t *request_msg, alink_msg_t *response_msg)
{
	uint8_t len = 0;
	size_t retLent;
	uint8_t ret = 0;
	uint8_t getBoardSN[SN_LENGTH + 1] = {0};

	ret = FlashNV_Get(FLASH_NV_ID_SN_BOARD, getBoardSN, SN_LENGTH+1, &retLent);
	if (ret == 0)
	{
		response_msg->buffer[0] = 0;
		len = 1;
	}
	else
	{
		memcpy(response_msg->buffer, (uint8_t *)&getBoardSN[1], getBoardSN[0]);
		len = getBoardSN[0];
	}


	return len;
}
uint16_t Service_SetBoardSN_fun(alink_msg_t *request_msg, alink_msg_t *response_msg)
{
	uint32_t len = 0;
	uint8_t retLen = 0;
	int8_t ret = 0;
	uint8_t sn_data[SN_LENGTH+1] = {0};

	len = (request_msg->msg_head.len_hi << 8) | request_msg->msg_head.len_lo;
	if ((len <= 1) || (len > SN_LENGTH) )
	{
		retLen = 1;
		response_msg->buffer[0] = 0;
		return retLen;
	}
	sn_data[0] = (uint8_t)len ;
	memcpy( &sn_data[1] , request_msg->buffer , len );
	ret = FlashNV_Set(FLASH_NV_ID_SN_BOARD, sn_data, SN_LENGTH+1 );
	if (ret == 0)
	{
		set_device_sn_code(&sn_data[1]);
		response_msg->buffer[0] = true;
	}
	else
	{
		response_msg->buffer[0] = false;
	}

	retLen = 1;
	return retLen;
}

uint16_t Service_GetSoftwareVersion_fun(alink_msg_t *request_msg, alink_msg_t *response_msg)
{
	int8_t ret = 0;
	uint64_t PL_Version_date;
	uint8_t getBoardSN[SN_LENGTH + 1] = {0};
	char PL_Version_string[32] = {0};

	uint32_t  data_len = 0x00 , len = 0x00 ;

	PL_Version_date = axi_read_data(PL_VOERSION_VP0);
	PL_Version_date = ( PL_Version_date << 32 ) ;
	PL_Version_date |= axi_read_data(PL_VOERSION_VP1);

	//update company name
	len = strlen(EMBED_COMPANY_NAME);
	memcpy(&response_msg->buffer[data_len], EMBED_COMPANY_NAME, len);
	data_len += 16;

	//update device name
	len = strlen(EMBED_DEVICE_NAME);
	memcpy(&response_msg->buffer[data_len], EMBED_DEVICE_NAME, len);
	data_len += 16;

	//update ps version
	len = strlen(EMBED_SOFTWARE_PS_VERSION_STR);
	memcpy(&response_msg->buffer[data_len], EMBED_SOFTWARE_PS_VERSION_STR, len);
	data_len += 32;

	//update pl version
	itoa( PL_Version_date , PL_Version_string , 10 );
	len = strlen(PL_Version_string);
	memcpy(&response_msg->buffer[data_len], &PL_Version_string, len);
	data_len += 32;

	//update SN code
	ret = FlashNV_Get(FLASH_NV_ID_SN_BOARD, getBoardSN, SN_LENGTH+1, &len);
	if (ret == 0)
	{
		len = strlen(EMBED_SN_CODE);
		memcpy(&response_msg->buffer[data_len], &EMBED_SN_CODE, len);
	}
	else
	{
		len = getBoardSN[0];
		memcpy(&response_msg->buffer[data_len], (uint8_t *)&getBoardSN[1], len);
	}
	data_len += 32;

	//update TCP IP
	get_tcp_ip( &response_msg->buffer[data_len] );
	data_len += 16;

	return data_len;
}

__attribute__((section (".service_entry")))
const SERVICE_ENTRY Service_GetBoardSN =
	{
		.id = CMD_GET_BOARD_SN,
		.index = 0x5555AAAA,
		.entry = Service_GetBoardSN_fun,
};

__attribute__((section (".service_entry")))
const SERVICE_ENTRY Service_SetBoardSN =
	{
		.id = CMD_SET_BOARD_SN,
		.index = 0x5555AAAA,
		.entry = Service_SetBoardSN_fun,
};

__attribute__((section (".service_entry")))
const SERVICE_ENTRY Service_C2GetSoftwareVersion =
	{
		.id = C2_GET_SOFTWARE_VERSION,
		.index = 0x5555AAAA,
		.entry = Service_GetSoftwareVersion_fun,
};

__attribute__((section (".service_entry")))
const SERVICE_ENTRY Service_APPGetSoftwareVersion =
{
	.id = APP_GET_SOFTWARE_VERSION,
	.index = 0x5555AAAA,
	.entry = Service_GetSoftwareVersion_fun,
};
