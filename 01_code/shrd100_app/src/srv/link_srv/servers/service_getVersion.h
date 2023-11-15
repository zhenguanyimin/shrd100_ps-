/*
 * service_getVersion.h
 *
 *  Created on: 2023年2月28日
 *      Author: A22745
 */

#ifndef SRC_SRV_LINK_SRV_SERVERS_SERVICE_GETVERSION_H_
#define SRC_SRV_LINK_SRV_SERVERS_SERVICE_GETVERSION_H_

#include "service_manager.h"

#pragma pack(1)
	typedef struct upgrade_version_ack
	{
		uint32_t 	uRunVersion;
		char		strAppVersion[64];
		char		strBootVersion[32];
		char		strHwVersion[32];
		char		strProtocolVersion[32];
	} alink_version_data_t;

#pragma pack()

#endif /* SRC_SRV_LINK_SRV_SERVERS_SERVICE_GETVERSION_H_ */
