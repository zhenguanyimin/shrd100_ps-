#include "stddef.h"
#include "string.h"
#include "service_manager.h"
#include "../../../app/sd_app/sd_app.h"
#include "../../../srv/alink/alink_msg_proc.h"



uint16_t Service_GetC2Time_fun(alink_msg_t *request_msg, alink_msg_t *response_msg)
{

	SetC2SystemTime( &request_msg->buffer[0] );

	return 0;


}
uint16_t Service_Get_Log_List_fun(alink_msg_t *request_msg, alink_msg_t *response_msg)
{

	PacketLogListData( request_msg->msg_head.sourceid , request_msg->msg_head.msgid );

	return 0;
}

uint16_t Service_Get_Log_Data_fun(alink_msg_t *request_msg, alink_msg_t *response_msg)
{
	uint32_t log_num , log_id ;
	char Log_name[30] = {0};
	uint32_t index = 0x00 ;
	size_t Log1_name_len ;

	log_num = (uint32_t)request_msg->buffer[index]
				+ ((uint32_t)request_msg->buffer[index+1] << 8)
				+ ((uint32_t)request_msg->buffer[index+2] << 16)
				+ ((uint32_t)request_msg->buffer[index+3] << 24);
	index += 4 ;
	for( uint32_t i = 0 ; i < log_num ; i++ )
	{
		log_id = (uint32_t)request_msg->buffer[index]
					+ ((uint32_t)request_msg->buffer[index+1] << 8)
					+ ((uint32_t)request_msg->buffer[index+2] << 16)
					+ ((uint32_t)request_msg->buffer[index+3] << 24);
		index += 4 ;
		Log1_name_len = (uint32_t)request_msg->buffer[index]
						+ ((uint32_t)request_msg->buffer[index+1] << 8)
						+ ((uint32_t)request_msg->buffer[index+2] << 16)
						+ ((uint32_t)request_msg->buffer[index+3] << 24);
		index += 4 ;
		memcpy( &Log_name[0] , &request_msg->buffer[index] , Log1_name_len );
		index += Log1_name_len ;
		PacketLogData( log_id , Log_name , request_msg->msg_head.sourceid , request_msg->msg_head.msgid );
	}

	return 0;
}

uint16_t Service_Delete_Log_fun(alink_msg_t *request_msg, alink_msg_t *response_msg)
{

	DeleteAllLogFile();
	response_msg->buffer[0] = 0x01 ;

	return 1;
}

__attribute__((section (".service_entry")))
const SERVICE_ENTRY Service_Get_Log_List =
	{
		.id = CMD_GET_LOG_LIST,
		.index = 0x5555AAAA,
		.entry = Service_Get_Log_List_fun,
};
__attribute__((section (".service_entry")))
const SERVICE_ENTRY Service_Get_Log_Data =
	{
		.id = CMD_GET_LOG_DATA,
		.index = 0x5555AAAA,
		.entry = Service_Get_Log_Data_fun,
};
__attribute__((section (".service_entry")))
const SERVICE_ENTRY Service_Delete_Log =
	{
		.id = CMD_DELETE_LOG,
		.index = 0x5555AAAA,
		.entry = Service_Delete_Log_fun,
};
__attribute__((section (".service_entry")))
const SERVICE_ENTRY Service_pc_Get_Log_List =
	{
		.id = CMD_PC_GET_LOG_LIST,
		.index = 0x5555AAAA,
		.entry = Service_Get_Log_List_fun,
};
__attribute__((section (".service_entry")))
const SERVICE_ENTRY Service_pc_Get_Log_Data =
	{
		.id = CMD_PC_GET_LOG_DATA,
		.index = 0x5555AAAA,
		.entry = Service_Get_Log_Data_fun,
};
__attribute__((section (".service_entry")))
const SERVICE_ENTRY Service_pc_Delete_Log =
	{
		.id = CMD_PC_DELETE_LOG,
		.index = 0x5555AAAA,
		.entry = Service_Delete_Log_fun,
};

__attribute__((section (".service_entry")))
const SERVICE_ENTRY Service_GetC2Time =
{
	.id = CMD_C2_TIME,
	.index = 0x5555AAAA,
	.entry = Service_GetC2Time_fun,
};
