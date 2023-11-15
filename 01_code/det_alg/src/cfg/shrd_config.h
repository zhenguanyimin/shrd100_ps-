
#ifndef AEAG_CONFIG_H
#define AEAG_CONFIG_H

#include <stdint.h>
#include "../inc/common_define.h"
/* version information */
#define EMBED_SOFTWARE_PS_VERSION_STR "aeag100_app_v1.00.28_rc2"

// note: radar board IP address is configured in network_thread

/* UDP/TCP communication configuration */
#define IP_ADDR_LOCAL "192.168.235.66"
#define IP_ADDR_REMOTE "192.168.235.88"

#define UDP_COM_PORT_LOCAL (6000)  // the udp port of radar
#define UDP_COM_PORT_REMOTE (8000) // the udp port of host
#define UDP_PACKET_LEN_MAX (1472)
#define UDP_PACKET_HEAD_LEN (20)
#define UDP_INFO_HEAD_LEN (24)
#define UDP_INFO_TAIL_LEN (4) // reserved(2B)+CRC16(2B)
#define UDP_INFO_LEN_MAX ((UDP_PACKET_LEN_MAX) - (UDP_PACKET_HEAD_LEN))
#define TCP_INFO_HEAD_LEN (24)
#define TCP_INFO_TAIL_LEN (4) // reserved(2B)+CRC16(2B)

#define TCP_COM_PORT_REMOTE (7000) // the tcp port of host

//#define	WARNING_LEVAL1_DIST  600
#define	WARNING_LEVAL2_DIST  600
#define	WARNING_LEVAL3_DIST  200

typedef enum task_pri
{
	TASK_PRI_APP_INIT = 2,
	TASK_PRI_NETWORK_CFG = 1,
	TASK_PRI_CLI_SERVER = 8,
	TASK_PRI_ETH_RCV = 4,
	TASK_PRI_PTOTOCOL_UDP = 3,
	TASK_PRI_PTOTOCOL_TCP = 3,

	TASK_PRI_DATA_PATH = 5,//7,
	TASK_PRI_DISPLAYPROCESS = 3,
	TASK_PRI_KEYHANDLE = 8,
	TASK_PRI_DETECTPROCESS = 6,
	TASK_PRI_HITPROCESS = 8,
	TASK_PRI_ALINKMSGPROC = 3,
	TASK_PRI_RF_CONFIG = 7,
	TASK_PRI_LOGGING = 5,
} radar_task_pri_t;

typedef enum task_stack_size
{
	TASK_STACK_SIZE_APP_INIT = 1024 * 30,//1024 * 10,
	TASK_STACK_SIZE_NETWORK_CFG = 1024,
	TASK_STACK_SIZE_CLI_SERVER = 2048,
	TASK_STACK_SIZE_ETH_RCV = 1024,
	TASK_STACK_SIZE_PTOTOCOL_UDP = 1024,
	TASK_STACK_SIZE_PTOTOCOL_TCP = 1024,

	TASK_STACK_SIZE_DATA_PATH = 1024 * 20,//1024 * 8,//2048,
	TASK_STACK_SIZE_DISPLAYPROCESS = 256,
	TASK_STACK_SIZE_KEYHANDLE = 256,
	TASK_STACK_SIZE_DETECTPROCESS = 2048,
	TASK_STACK_SIZE_HITPROCESS = 2048,
	TASK_STACK_SIZE_ALINKMSGPROC = 1024,
	TASK_STACK_SIZE_RF_CONFIG = 1024 * 8,
	TASK_STACK_SIZE_LOGGINGPROCESS = 2048,
} radar_task_stack_size_t;

uint32_t aeagCfg_GetPlSignalHandlingLocalOscFreq(void);

sPL_SignalHandlingCfg *aeagCfg_GetPlSignalHandlingCfg(void);

#endif /* AEAG_CONFIG_H */
