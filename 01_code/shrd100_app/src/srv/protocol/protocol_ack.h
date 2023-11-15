
#ifndef PROTOCOL_ACK_H
#define PROTOCOL_ACK_H

#include "protocol_common.h"

#define PROTOCOL_ACK_VER (0x00)

#pragma pack(1)
typedef struct protocol_ack
{
	protocol_info_head_t stInfoHeader;

	uint32_t revResult;		  // 信息接收结果：0 成功，1 失败，2 UDP存在丢包，其他待定义
	uint16_t revUdpPacketNo;  // revResult为2时，0表示开始新的信息发送，其他值表示最新接收UDP包的序号；revResult为其他值时，该值无意义
	uint16_t lostUdpPacketNo; // revResult为2时，该值表示当前信息第一个未接收到的UDP包的序号；revResult为其他值时，该值无意义
	uint32_t rsv;			  // 保留

	protocol_info_tail_t stInfoTail;
} protocol_ack_t;
#pragma pack()

ret_code_t protocol_send_ack(int32_t sockFd, protocol_ack_t *ack);

#endif /* PROTOCOL_ACK_H */
