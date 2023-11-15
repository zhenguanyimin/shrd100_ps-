
#ifndef PROTOCOL_SYSTEM_H
#define PROTOCOL_SYSTEM_H

#include "protocol_common.h"

#define PROTOCOL_RADAR_PLATFROM_INFO_VER (0x00)
#define PROTOCOL_RADAR_STATUS_VER (0x00)

#pragma pack(1)
typedef struct protocol_radar_platfrom_info
{
	protocol_info_head_t stInfoHeader;

	int32_t heading;			  // ����Ƕȣ� [0,360)�� INV_ONE15_SCALE
	int32_t pitching;			  // ��ҡ�Ƕȣ�[-30,+30]�� INV_ONE15_SCALE
	int32_t rolling;			  // ����Ƕȣ�[-45,+45]�� INV_ONE15_SCALE
	int32_t longitude;			  // ���ȣ�[-180,+180]�� INV_ONE15_SCALE
	int32_t latitude;			  // ά�ȣ�[-90,+90]�� INV_ONE15_SCALE
	int32_t altitude;			  // ���θ߶ȣ�m�� INV_ONE7_SCALE
	int32_t velocityNavi;		  // �����ٶȣ�m/s�� INV_ONE7_SCALE
	uint16_t targetTimeMark[6];	  // Ŀ��ʱ�꣨����ʱ�䣩�����ꡢ�¡��ա�ʱ���֡��롢���롢΢�룬��11���ֽڣ������У���Ϊ�����ֽڡ�����ָ������1ms������Ϊ�����ļ���ֵ,2���ֽڱ�ʾ���룬2���ֽڱ�ʾ΢�룬�����Ϊ1���ֽڡ��źŴ������Ա�����������������Ϊ��ʱͬ���ź�
	uint32_t sigProcRelativeTime; // �Ŵ������ʱ�䣨��ȷ��1ms������PS���ʱ
	uint16_t reserve[4];		  // ���ã�Ĭ��Ϊ0

	protocol_info_tail_t stInfoTail;
} protocol_radar_platfrom_info_t;

typedef struct protocol_radar_status
{
	protocol_info_head_t stInfoHeader;

	uint16_t isFailFlag;   // �������ޱ�־��1���й��ϣ�0���޹���
	uint16_t failBitData1; // ��������1����Ƶ���������
	uint16_t failBitData2; // ��������2
	uint16_t batteryPower; // ��ص������ٷֱ� INV_ONE7_SCALE

	uint16_t reserve[4]; // ���ã�Ĭ��Ϊ0

	protocol_info_tail_t stInfoTail;
} protocol_radar_status_t;
#pragma pack()

ret_code_t protocol_send_radar_platfrom_info(protocol_radar_platfrom_info_t *platfrom_info);
ret_code_t protocol_send_radar_status(protocol_radar_status_t *status);

#endif /* PROTOCOL_SYSTEM_H */
