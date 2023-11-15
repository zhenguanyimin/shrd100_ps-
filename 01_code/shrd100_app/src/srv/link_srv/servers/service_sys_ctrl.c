/*
 * service_getBoardSN.c
 *
 *  Created on: 2022骞�11鏈�2鏃�
 *      Author: A22745
 */

#include "service_sys_ctrl.h"
#include "../../log/log.h"
#include "../../../app/sys_status_data/detection_param.h"
#include "../../../app/process_task/orientation.h"

#define SET_ENTER_DIR_VALID_DATA_LEN	(31)

uint16_t Service_GetWorkMode_fun(alink_msg_t *request_msg, alink_msg_t *response_msg)
{

	response_msg->buffer[0] = GetEligibleEntryOrientationFlag();

	return 1;
}
uint16_t Service_EnterDir_fun(alink_msg_t *request_msg, alink_msg_t *response_msg)
{
	uint8_t retLen = 0;
	uint32_t index = 0x00 ;
	uint8_t switchState;
	uint8_t uav_number;
	char l_droneName[32] = {0} ;
	uint32_t ufreq;

	retLen = (request_msg->msg_head.len_hi << 8) | request_msg->msg_head.len_lo;

	if (retLen != SET_ENTER_DIR_VALID_DATA_LEN)
	{
		retLen = 1;
		response_msg->buffer[0] = false;
		return retLen;
	}


	switchState = (uint32_t)request_msg->buffer[index++];
	uav_number  = (uint32_t)request_msg->buffer[index++];

	memcpy( l_droneName, &request_msg->buffer[index], 25);
	index += 20;

	ufreq =(uint32_t)request_msg->buffer[index]
			+ ((uint32_t)request_msg->buffer[index+1] << 8)
			+ ((uint32_t)request_msg->buffer[index+2] << 16)
			+ ((uint32_t)request_msg->buffer[index+3] << 24);

    if (switchState == 1) // 进入定向
    {
    	SetEligibleEntryOrientationFlag(1);
    	SetUavNumber(uav_number-1);

    }
    else if (switchState == 0)  // 退出定向
    {
    	SetEligibleEntryOrientationFlag(0);
    	SetUavNumber(0);
    }
    else
    {
		retLen = 1;
		response_msg->buffer[0] = false;
		return retLen;
    }

	LOG_DEBUG("set enter or exit ok ,state=%d(0:exit,1:enter)\r\n",switchState);

	response_msg->buffer[0] = true;
	retLen = 1;
	return retLen;
}

__attribute__((section (".service_entry")))
const SERVICE_ENTRY Service_SetWorkMode =
	{
		.id = CMD_GET_WORK_MODE,
		.index = 0x5555AAAA,
		.entry = Service_GetWorkMode_fun,
};

__attribute__((section (".service_entry")))
const SERVICE_ENTRY Service_EnterDir=
	{
		.id = CMD_ENTER_DIR_SET,
		.index = 0x5555AAAA,
		.entry = Service_EnterDir_fun,
};

