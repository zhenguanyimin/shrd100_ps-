/*
 * UDS_Service.h
 *
 * Created on: 2021-7-10
 *      Author: A19199
 */

#ifndef UDS_SERVICE_H_
#define UDS_SERVICE_H_

#include <stdint.h>

#define SERVICE_ENTRY_START                     (uint8_t *)&__service_entry_start__
#define SERVICE_ENTRY_END                       (uint8_t *)&__service_entry_end__
extern uint32_t __service_entry_start__         __attribute__ ((section ("data")));
extern uint32_t __service_entry_end__           __attribute__ ((section ("data")));

#include "../../alink/alink_msg_proc.h"

typedef enum UDS_ServiceID
{
	// C2 ble -> aeag
	C2_CMD_GET_SN_CODE = 0x00,
	C2_CMD_SET_WIFI_INFO = 0x02,
	C2_CMD_DISCONNCTNETWORK = 0x03,
	C2_CMD_SET_SOURCE_ID = 0x04,
	C2_CMD_UPLOAD = 0x05,

	// C2 WIFI <-> aeag
	C2_CMD_UPLOAD_IP_PORT_AND_GET_CHANNE = 0x10,
	C2_CMD_UPLOAD_HEARTBEAT = 0x11,
	C2_GET_SOFTWARE_VERSION = 0x20,
	CMD_ENTER_DIR_SET = 0x21,
	C2_HEARTBEAT_PACKET = 0x25,
	CMD_GET_LOG_LIST = 0x28,
	CMD_GET_LOG_DATA = 0x29,
	CMD_DELETE_LOG = 0x2A,

	APP_GET_SOFTWARE_VERSION = 0xE0,

	//	eaeg -> Watch
	CMD_ON_OFF_SCREEN = 0xE0,
	CMD_USER_STRING = 0xE1,
	CMD_SPECI_LOCATION_STR = 0xE2,
	CMD_C2_TIME = 0xE3,
	CMD_ELECTRIC = 0xE4,
	CMD_WARNING = 0xE5,
	CMD_C2_LOCATION_INFO = 0xE6,
	CMD_SPIN_BUTTON = 0xE7,
	CMD_SET_BUTTON = 0xE8,
	CMD_SCREEN_DHEARBEAT = 0xEF,

	//	Watch -> eaeg
	CMD_ADD_CFG_FPR = 0xC0,
	CMD_UPDATE_LIB = 0xC1,
	CMD_GET_LIB = 0xC2,
	CMD_SYSTEM_RESET = 0xC3,
	CMD_FACTORY_RESET = 0xC4,
	CMD_GET_WORK_MODE = 0xC9,


	//	Watch -> eaeg Firmware upgrade
	CMD_HEARBEAT = 0xD0,
	CMD_SET_SN = 0xD4,
	CMD_GET_SN = 0xD5,
	CMD_GET_APP_FLASHSIZE = 0xD6,
	CMD_REQUEST_UPGRADE = 0xD7,
	CMD_ERASE_APP_FLASH = 0xD8,
	CMD_DOWNLOADING = 0xD9,
	CMD_GET_VERSIONS = 0xDB,
	CMD_CRC_CHECK = 0xDC,

	// pc -> eaeg
	CMD_SET_WORKMODE = 0x60,
	CMD_GET_WORKMODE = 0x61,
	CMD_GET_SWITCHSTATE = 0x62,
	CMD_CTR_GNSS_HIT_START = 0x63,
	CMD_SET_HIT_FREQ = 0x64,

	CMD_SET_CALIBVALUE = 0x65,
	CMD_GET_CALIBVALUE = 0x66,

	CMD_GET_HIT_FREQ = 0x68,
	CMD_GET_GNSS_STATE = 0x69,
	CMD_PC_GET_SELFTEST_INFO = 0x73,

	CMD_SET_BOARD_SN = 0x80,
	CMD_GET_BOARD_SN = 0x81,
	CMD_GET_SOFTWARE_VERSION = 0x82,

	// usb upgrade
	CMD_SET_GOTO_BOOTLOAD = 0xA1,
	CMD_GET_UPGRADE_VERSION = 0xAB,

	//	pc -> eaeg
	CMD_PC_GET_LOG_LIST = 0xA2,
	CMD_PC_GET_LOG_DATA = 0xA3,
	CMD_PC_DELETE_LOG = 0xA4,
	
	CMD_DUP_QUERY_BROADCAST = 0xBA,
	CMD_DEVICE_STATUS_INFO = 0xBB,
	CMD_TCP_LINK_INFO = 0xBC,
	CMD_TCP_WIFI_INFO = 0xBD,

}UDS_ServiceID_e;

/**
 *
 */
typedef struct SERVICE_ENTRY_
{
	uint8_t id;
	uint32_t index;
	/**
	 * 服务入口
	 * @param recvBuffer 请求APD
	 * @param recvLength APD 长度
	 * @param sendBuffer 响应APD缓存
	 * @return response length
	 */
	uint16_t (*entry)(alink_msg_t *request_msg, alink_msg_t *response_msg);
} SERVICE_ENTRY;

SERVICE_ENTRY *ServiceManager_GetEntry(uint8_t service_id);

#endif /* UDS_SERVICE_H_ */
