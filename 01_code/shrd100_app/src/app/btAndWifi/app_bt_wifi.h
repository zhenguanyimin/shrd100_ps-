
#ifndef SRC_APP_BTANDWIFI_APP_BT_WIFI_H_
#define SRC_APP_BTANDWIFI_APP_BT_WIFI_H_

#include "xscugic.h"
#include "../alg/det_alg/droneSniffer.h"


#define BT_WIFI_MAX_PAYLOAD_LEN 	1024 ///< Maximum payload length


typedef struct bt_wifi_msg_head
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
} bt_wifi_msg_head_t;


void update_broadcastflag();
void update_broadcastInfo( uint32_t version );
int32_t InitBtWifiProcess_Task(void);
int get_bt_wifi_connect_status();
void fc41d_create_response_ble(char* at_buff,uint8_t res_result);

#endif /* SRC_APP_BTANDWIFI_APP_BT_WIFI_H_ */
