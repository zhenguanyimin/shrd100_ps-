/*
 * service_getVersion.c
 *
 *  Created on: 2023年2月28日
 *      Author: A22745
 */

#include "service_getVersion.h"
#include "../../../cfg/shrd_config.h"
#include "../../log/log.h"


uint16_t Service_GetUpgradeVersion_fun(alink_msg_t *request_msg, alink_msg_t *response_msg)
{
	uint8_t len;
	alink_version_data_t *versionInfo = {0};
	versionInfo = (alink_version_data_t*)(response_msg->buffer);

	versionInfo->uRunVersion = 1; // in app
	strncpy( versionInfo->strBootVersion, "N/A", sizeof(versionInfo->strBootVersion) );
	strncpy( versionInfo->strAppVersion, get_embed_software_ps_version_string(), (size_t)sizeof(versionInfo->strAppVersion) );
	strncpy( versionInfo->strHwVersion, "N/A", sizeof(versionInfo->strHwVersion) );
	strncpy( versionInfo->strProtocolVersion, get_embed_alink_version_string(), sizeof(versionInfo->strProtocolVersion) );
	len= sizeof(alink_version_data_t);

	LOG_DEBUG("sizeof(versionInfo)=%d\r\n", sizeof(versionInfo));
	LOG_DEBUG("[%s:%d] app_version:%s,protocal_version:%s\r\n", __FUNCTION__, __LINE__, \
			versionInfo->strAppVersion, versionInfo->strProtocolVersion);
	return len;
}


__attribute__((section (".service_entry")))
const SERVICE_ENTRY Service_GetUpgradeVersion =
{
		.id = CMD_GET_UPGRADE_VERSION,
		.index = 0x5555AAAA,
		.entry = Service_GetUpgradeVersion_fun,
};
