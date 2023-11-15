/*
 * bt_wifi.h
 *
 *  Created on: 2022年11月24日
 *      Author: A22745
 */

#ifndef SRC_APP_BTANDWIFI_BT_WIFI_H_
#define SRC_APP_BTANDWIFI_BT_WIFI_H_
#include "string.h"
#include <stdint.h>
#include "../../srv/alink/alink_msg_common.h"

#define BOARD_SN_LENGTH				17
#define RECV_MAX_BUFF 				1024

#define BT_WIFI_MSG_MAGIC_VAL 		(0XFD)
#define WIFI_NAME_LENGTH			128
#define WIFI_KEY_LENGTH				50
#define IP_LENGTH					4
#define PORT_LENGTH					2

typedef enum {
	BT_WIFI_OK = 0,
	BT_WIFI_INVALID_PARAM,
	BT_WIFI_UNSUCCESS,
	BT_WIFI_UNSUPORT,
	BT_WIFI_TIMEOUT,
	BT_WIFI_RETURN_ERROR,
}btWifiErrorState_e;

typedef enum {
	RETURN_DATA_UNCKECK,
	RETURN_DATA_CKECK,
} retDatCheck_e;


typedef enum tcpipProxy{
	PROXY_TCP =1,
	PROXY_UPD,
	PROXY_HTTP
} tcpipProxy_e;



typedef enum {

	BT_CONFIG_STEP_STATE_PH_INIT,
	BT_CONFIG_STEP_REC_WIFIINFO,
	BT_CONFIG_STEP_STATE_CONNECT_WIFI,
	BT_CONFIG_STEP_STATE_CONNECT_UDP,
	BT_CONFIG_STEP_STATE_CONNECT_TCP,
	BT_STATE_CHECK,

} btConfigStepState_e;

typedef struct applychainInfo
{
	char device_sn[32];
	char company[32];
	char devicename[32];
	uint16_t devicetype;
	char version[16];
} applychainInfo_t;

typedef struct replychainInfo
{
	char device_sn[32];
	char company[32];
	char devicenae[32];
	uint16_t devicetype;
	char version[16];
} replychainInfo;

typedef struct wifiTcpInfo
{
	uint8_t wifiName[WIFI_NAME_LENGTH];
	uint8_t wifiNameLenght;
	uint8_t wifiKey[WIFI_KEY_LENGTH];
	uint8_t wifiKeyLenght;
	uint8_t ip[IP_LENGTH];
	uint16_t port;
} wifiTcpInfo_t;


//注意空间对齐，不要轻易改变以下两个结构体，改变以下两个结构体空间不对齐容易引起发送数据量多出预期
typedef struct tcpipInfo
{
	uint8_t ip[IP_LENGTH];
//	uint16_t port;
	uint8_t port[PORT_LENGTH];
	uint8_t proxy;
} tcpipInfo_t;

typedef struct snTcpipInfo
{
	uint8_t SNStr[BOARD_SN_LENGTH];
	tcpipInfo_t tcpIpPortInfo;
} snTcpipInfo_t;



typedef struct wifi_info
{
    uint32_t update;
    char name[20];
    char password[20];
}wifi_info_t;


typedef struct socket_info
{
	uint32_t update;
    char socketID;
    char service_type[12];
    char remote_addr[16];
    uint32_t remote_port;
    char local_port;
    char state;
}socket_info_t;

extern char AT_QIOPEN_[256];
extern char AT_QIOPEN_OTHER_[256];
extern char AT_QSTAAPINFO_[256];
uint32_t wifi_info_status();
char* get_tcp_ip();
void wifi_send_data(char* wifi_Data,uint32_t DataLen);
void update_wifi_info( char *name , char * password );
void update_tcp_ip_port_info( uint32_t l_ip_info , uint16_t remote_port );
void init_fc41d_ack_buff();
void fc41d_ack_process();
int32_t fc41d_ble_peripherals_init();
int32_t fc41d_wifi_connect();
int32_t fc41d_tcp_connect();
void fc41d_change_listen_port( socket_info_t *socket_info );
int32_t fc41d_udp_connect();
int32_t fc41d_udp_socket_close();
int32_t fc41d_tcp_socket_close();
void fc41d_fill_ip(char* atIP);
btConfigStepState_e fc41d_state_check();
void BtSendData_(char socket, char *buff, uint32_t data_len , char *remote_ip , uint32_t remote_port );


#endif /* SRC_APP_BTANDWIFI_BT_WIFI_H_ */
