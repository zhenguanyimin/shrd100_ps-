#ifndef ALINK_MSG_COMMON_H
#define ALINK_MSG_COMMON_H
#include <stdbool.h>
#include <stdint.h>

#define ALINK_MSG_MAGIC_VAL (0XFD)
#define ALINK_MSG_LEN_TOTAL(boby_len) ((boby_len) + ALINK_NUM_NON_PAYLOAD_BYTES)
#define ALINK_MSG_EXT_CRC_DTF 0
#define ALINK_MSG_RSP_OK 0
#define ALINK_MSG_RSP_ERR 1
#define ALINK_NUM_CHECKSUM_BYTES 2
#define ALINK_CORE_HEADER_LEN 8
#define ALINK_MAX_PAYLOAD_LEN 4096 ///< Maximum payload length
#pragma pack(1)
typedef struct alink_msg_head
{
    uint8_t magic;    ///< protocol magic marker
    uint8_t len_lo;   ///< Low byte of length of payload
    uint8_t len_hi;   ///< Hight byte of length of payload
    uint8_t seq;      ///< Sequence of packet
    uint8_t destid;   ///< ID of message destination
    uint8_t sourceid; ///< ID of the message sounrce
    uint8_t msgid;    ///< ID of message in payload
    uint8_t ans;      ///< Whether the command requires an answer 0:no  1:yes
    uint8_t checksum; ///< Checksum of message header
} alink_msg_head_t;
#pragma pack()
typedef enum MAV_COMPONENT
{
    DEV_NONE = 0, /*  | */
    DEV_AEAG = 0x01,
    DEV_TYPEC = 0x02,
    DEV_RADAR_C2 = 0x03,
    DEV_PC = 0x04,
 	DEV_C2_BLE = 0x05,
	DEV_C2 = 0x06,
	DEV_DRONEID = 0x07,
	BROARDCAST = 0xff,
    MAV_COMPONENT_ENUM_END = 256, /*  | */
} MAV_COMPONENT;

bool AlinkMsg_IsCrcValid(uint8_t *msg_buf, uint16_t msg_len, uint16_t crc_extend);

#endif /* ALINK_MSG_COMMON_H */
