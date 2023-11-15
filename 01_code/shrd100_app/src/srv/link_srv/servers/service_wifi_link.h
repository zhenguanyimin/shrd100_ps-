#ifndef SERVICE_WIFI_LINK_H
#define SERVICE_WIFI_LINK_H

#include <stdint.h>
#include "../../alink/alink_msg_proc.h"
#include "service_manager.h"
#include "../../../drv/fc41d/bt_wifi.h"


uint16_t Service_UdpBroardcast_fun(alink_msg_t *request_msg, alink_msg_t *response_msg);
uint16_t Service_DeviceInfo_fun(alink_msg_t *request_msg, alink_msg_t *response_msg);

#endif
