#include "service_manager.h"

uint16_t Service_SystemReset_fun(alink_msg_t *request_msg, alink_msg_t *response_msg)
{
	uint8_t buf[20] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0x11, 0x22, 0x33,
					   0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0x88, 0x77};

	memcpy(response_msg->buffer, buf, 20);

	return 20;
}

__attribute__((section (".SERVICE_ENTRY")))
const SERVICE_ENTRY Service_SystemReset =
	{
		.id = CMD_SYSTEM_RESET,
		.index = 0x5555AAAA,
		.entry = Service_SystemReset_fun,
};
