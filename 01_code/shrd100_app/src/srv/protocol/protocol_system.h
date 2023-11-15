
#ifndef PROTOCOL_SYSTEM_H
#define PROTOCOL_SYSTEM_H

#include "protocol_common.h"

#define PROTOCOL_RADAR_PLATFROM_INFO_VER (0x00)
#define PROTOCOL_RADAR_STATUS_VER (0x00)

#pragma pack(1)
typedef struct protocol_radar_platfrom_info
{
	protocol_info_head_t stInfoHeader;

	int32_t heading;			  // 航向角度， [0,360)度 INV_ONE15_SCALE
	int32_t pitching;			  // 纵摇角度，[-30,+30]度 INV_ONE15_SCALE
	int32_t rolling;			  // 横滚角度，[-45,+45]度 INV_ONE15_SCALE
	int32_t longitude;			  // 经度，[-180,+180]度 INV_ONE15_SCALE
	int32_t latitude;			  // 维度，[-90,+90]度 INV_ONE15_SCALE
	int32_t altitude;			  // 海拔高度（m） INV_ONE7_SCALE
	int32_t velocityNavi;		  // 导航速度（m/s） INV_ONE7_SCALE
	uint16_t targetTimeMark[6];	  // 目标时标（绝对时间）：（年、月、日、时、分、秒、毫秒、微秒，共11个字节），其中，年为两个字节、毫秒指的是以1ms以周期为步进的计数值,2个字节表示毫秒，2个字节表示微秒，其余均为1个字节。信号处理器以北斗送来的秒脉冲作为授时同步信号
	uint32_t sigProcRelativeTime; // 信处的相对时间（精确到1ms），由PS侧计时
	uint16_t reserve[4];		  // 备用，默认为0

	protocol_info_tail_t stInfoTail;
} protocol_radar_platfrom_info_t;

typedef struct protocol_radar_status
{
	protocol_info_head_t stInfoHeader;

	uint16_t isFailFlag;   // 故障有无标志，1：有故障，0：无故障
	uint16_t failBitData1; // 故障数据1，射频板故障数据
	uint16_t failBitData2; // 故障数据2
	uint16_t batteryPower; // 电池电量，百分比 INV_ONE7_SCALE

	uint16_t reserve[4]; // 备用，默认为0

	protocol_info_tail_t stInfoTail;
} protocol_radar_status_t;
#pragma pack()

ret_code_t protocol_send_radar_platfrom_info(protocol_radar_platfrom_info_t *platfrom_info);
ret_code_t protocol_send_radar_status(protocol_radar_status_t *status);

#endif /* PROTOCOL_SYSTEM_H */
