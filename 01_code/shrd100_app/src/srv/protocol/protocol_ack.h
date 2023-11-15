
#ifndef PROTOCOL_ACK_H
#define PROTOCOL_ACK_H

#include "protocol_common.h"

#define PROTOCOL_ACK_VER (0x00)

#pragma pack(1)
typedef struct protocol_ack
{
	protocol_info_head_t stInfoHeader;

	uint32_t revResult;		  // ��Ϣ���ս����0 �ɹ���1 ʧ�ܣ�2 UDP���ڶ���������������
	uint16_t revUdpPacketNo;  // revResultΪ2ʱ��0��ʾ��ʼ�µ���Ϣ���ͣ�����ֵ��ʾ���½���UDP������ţ�revResultΪ����ֵʱ����ֵ������
	uint16_t lostUdpPacketNo; // revResultΪ2ʱ����ֵ��ʾ��ǰ��Ϣ��һ��δ���յ���UDP������ţ�revResultΪ����ֵʱ����ֵ������
	uint32_t rsv;			  // ����

	protocol_info_tail_t stInfoTail;
} protocol_ack_t;
#pragma pack()

ret_code_t protocol_send_ack(int32_t sockFd, protocol_ack_t *ack);

#endif /* PROTOCOL_ACK_H */
