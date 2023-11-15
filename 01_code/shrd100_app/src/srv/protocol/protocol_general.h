
#ifndef PROTOCOL_GENERAL_H
#define PROTOCOL_GENERAL_H

#include "protocol_common.h"

#define PROTOCOL_DEV_INFO_VER (0x00)

#pragma pack(1)
typedef struct protocol_dev_info
{
	protocol_info_head_t stInfoHeader;

	char serialNum[64];
	char hardwareVersion[64];
	char bootVersion[32];
	char psSoftwareVersion[64];
	char plSoftwareVersion[64];
	char detectionVersion[64];
	char trackVersion[64];
	char reserved[64];

	protocol_info_tail_t stInfoTail;
} protocol_dev_info_t;
#pragma pack()

ret_code_t protocol_send_dev_info(protocol_dev_info_t *dev_info);

#endif /* PROTOCOL_GENERAL_H */
