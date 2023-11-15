
#ifndef PROTOCOL_COMMON_H
#define PROTOCOL_COMMON_H

#include "lwip/def.h"
#include "../../inc/common.h"
#include "../../cfg/shrd_config.h"

#include "../log/log.h"

#include "protocol_crc.h"

#define ONE_TENTH 0.1f
#define ONE_PERCENT 0.01f
#define INV_ONE6_SCALE 0.015625f		   // 1/2^6
#define INV_ONE7_SCALE 0.0078125f		   // 1/2^7
#define INV_ONE8_SCALE 0.00390625f		   // 1/2^8
#define INV_ONE15_SCALE (3.0517578125e-05) // 1/2^15
#define MUL_ONE6_SCALE 64				   // 2^6
#define MUL_ONE7_SCALE 128				   // 2^7
#define MUL_ONE8_SCALE 256				   // 2^8
#define MUL_ONE15_SCALE 32768			   // 2^15

#define PACKET_HEAD_FLAG 0xF0F1F2F3U
#define INFO_HEAD_FLAG 0xA5A5A5A5U

typedef enum protocol_info_type
{
	/* general information */
	PIT_DEV_INFO = 0x0000, // basic information of the device
	PIT_ACK_INFO = 0x0100, // acknowledgment information (reply) of a request

	/* object information */
	PIT_DET_OBJ = 0x1000, // object(s) detected
	PIT_TRK_OBJ = 0x1001, // object(s) tracked

	/* parameter information */
	PIT_BEAM_SCHEDULING = 0x2000, // beam scheduling
	PIT_CFG_PARAM = 0x2001,		  // debugging / configuration parameter information

	/* system information */
	PIT_RADAR_PLATFROM = 0x3000, // radar mounted platform (container) information
	PIT_RADAR_STATUS = 0x3001,	 // radar status / fault information

	/* debug data information */
	PIT_ADC_DATA = 0x4000, // ADC data
} protocol_info_type_t;

typedef enum protocol_wave_code
{
	WAVE_CODE_1 = 1,
	WAVE_CODE_2 = 2,
	WAVE_CODE_3 = 3,
	WAVE_CODE_4 = 4,
	WAVE_CODE_5 = 5
} protocol_wave_code_t;

#pragma pack(1)
typedef struct protocol_packet_head
{
	uint32_t packetFlag;
	uint16_t totalPacketNum;
	uint16_t packetLength;
	uint16_t sendInfoCount;
	uint8_t terminalType;
	uint8_t subTerminalType;
	uint16_t infoType; // ref protocol_info_type_t
	uint16_t infoPacketNum;
	uint16_t curInfoPacketOrder;
	uint16_t reserved;
} protocol_packet_head_t;

typedef struct protocol_info_head
{
	uint32_t infoSync;
	uint32_t infoLength;
	uint32_t frameID;
	uint32_t timestamp;
	uint16_t infoType;
	uint16_t terminalID;
	uint8_t terminalType;
	uint8_t subTerminalType;
	uint8_t infoVersion;
	uint8_t reserved[1];
} protocol_info_head_t;

typedef struct protocol_info_tail
{
	uint16_t reserved;
	uint16_t crc;
} protocol_info_tail_t;
#pragma pack()

typedef void (*send_data_cb_fp)(void *data, uint32_t len, uint32_t result);

/* inner APIs used by other sub-modules */
ret_code_t protocol_common_init(void);
ret_code_t protocol_udp_send_data_blocking(void *data, uint32_t len);
ret_code_t protocol_udp_send_data_nonblocking(void *data, uint32_t len, send_data_cb_fp cb_fp);

/* inner APIs implemented by other sub-modules */
ret_code_t protocol_cfg_param_process_data(void *data, uint32_t len);
ret_code_t protocol_udp_send_data2(void *data, uint32_t len);
#endif /* PROTOCOL_COMMON_H */
