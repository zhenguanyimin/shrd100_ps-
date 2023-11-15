#include "service_wifi_link.h"
#include "../../../app/btAndWifi/app_bt_wifi.h"
#include "../../../cfg/shrd_config.h"
#include "../../../drv/fc41d/bt_wifi.h"

uint16_t Service_Bleinfo_fun(alink_msg_t *request_msg, alink_msg_t *response_msg)
{
	uint16_t len = 1;
	char name[32] = {0};
	char password[32] = {0};
	uint16_t type = 0x00 ;
	uint16_t authType = 0x00 ;

	memcpy( name, &request_msg->buffer[0], strlen(&request_msg->buffer[0]));
	memcpy( password, &request_msg->buffer[32], strlen(&request_msg->buffer[32]));

	type = (request_msg->buffer[65]<<8)|(request_msg->buffer[64]<<0);
	type = (request_msg->buffer[67]<<8)|(request_msg->buffer[66]<<0);

	response_msg->buffer[0] = 0x00;
	update_wifi_info( name , password );

	return len;
}

uint16_t Service_QueryBroadcast_fun(alink_msg_t *request_msg, alink_msg_t *response_msg)
{
	uint16_t l_sn , len = 0x01;
	uint32_t l_protocol = 0x00 ;

	response_msg->buffer[0] = 0x01;
	l_protocol = (request_msg->buffer[3]<<24)|(request_msg->buffer[2]<<16)|(request_msg->buffer[1]<<8)|(request_msg->buffer[0]<<0);
	l_sn = (request_msg->buffer[5]<<8)|(request_msg->buffer[4]<<0);

//	if( l_sn != 0x00 )
//	{
		update_broadcastInfo( l_protocol );
//	}

	return len;
}


uint16_t Service_DeviceInfo_fun(alink_msg_t *request_msg, alink_msg_t *response_msg)
{

	uint16_t len = 1;
	char sn[32] = {0};
	uint32_t l_ip_info ;
	uint16_t l_local_port;
	uint16_t l_type;
	char service_type[10] = {0};

	response_msg->buffer[0] = 0x01;

	memcpy( sn, &request_msg->buffer[0], strlen(&request_msg->buffer[0]));

	l_ip_info = (request_msg->buffer[35]<<24)|(request_msg->buffer[34]<<16)|(request_msg->buffer[33]<<8)|(request_msg->buffer[32]<<0);

	l_local_port = (request_msg->buffer[37]<<8)|(request_msg->buffer[36]<<0);

	l_type = (request_msg->buffer[23]<<8)|(request_msg->buffer[22]<<0);

	update_tcp_ip_port_info( l_ip_info , l_local_port );

	return len;
}
uint16_t Service_C2HeartBeatPacket_fun(alink_msg_t *request_msg, alink_msg_t *response_msg)
{

	uint16_t len = 1;

	response_msg->buffer[0] = 0x01;

    update_c2_heartbeat_time();
	return len;
}
uint16_t Service_GetSnCode_fun(alink_msg_t *request_msg, alink_msg_t *response_msg)
{

	uint16_t len = 15;

	response_msg->buffer[0] = 0x01;

	memcpy( &response_msg->buffer[1], l_sn_code, strlen(l_sn_code) );

	return len;
}



__attribute__((section (".service_entry")))
const SERVICE_ENTRY Service_BleInfo =
	{
		.id = CMD_TCP_WIFI_INFO,
		.index = 0x5555AAAA,
		.entry = Service_Bleinfo_fun,
};

__attribute__((section (".service_entry")))
const SERVICE_ENTRY Service_Broadcast =
	{
		.id = CMD_DUP_QUERY_BROADCAST,
		.index = 0x5555AAAA,
		.entry = Service_QueryBroadcast_fun,
};


__attribute__((section (".service_entry")))
const SERVICE_ENTRY Service_DeviceInfo =
	{
		.id = CMD_TCP_LINK_INFO,
		.index = 0x5555AAAA,
		.entry = Service_DeviceInfo_fun,
};


__attribute__((section (".service_entry")))
const SERVICE_ENTRY Service_C2HeartBeatPacket =
	{
		.id = C2_HEARTBEAT_PACKET,
		.index = 0x5555AAAA,
		.entry = Service_C2HeartBeatPacket_fun,
};


__attribute__((section (".service_entry")))
const SERVICE_ENTRY Service_GetSnCode =
	{
		.id = C2_CMD_GET_SN_CODE,
		.index = 0x5555AAAA,
		.entry = Service_GetSnCode_fun,
};


