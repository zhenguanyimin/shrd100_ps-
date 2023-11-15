/*
 * service_getBoardSN.h
 *
 *  Created on: 2022年11月2日
 *      Author: A22745
 */

#ifndef SERVERS_SERVICE_DEV_INFO_H_
#define SERVERS_SERVICE_DEV_INFO_H_
#include "service_manager.h"

uint16_t Service_GetSoftwareVersion_fun(alink_msg_t *request_msg, alink_msg_t *response_msg);
uint16_t Service_GetBoardSN_fun(alink_msg_t *request_msg, alink_msg_t *response_msg);

#endif /* SERVERS_SERVICE_GETBOARDSN_H_ */
