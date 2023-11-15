
#include "protocol_system.h"

ret_code_t protocol_send_radar_platfrom_info(protocol_radar_platfrom_info_t *platfrom_info)
{
	ret_code_t ret = RET_OK;

	if (platfrom_info == NULL)
	{
		ret = RET_INVALID_PARAM;
		goto out;
	}

	ret = protocol_udp_send_data_blocking(platfrom_info, sizeof(*platfrom_info));

out:
	LOG_DEBUG("[%s:%d] ret=%d\r\n", __FUNCTION__, __LINE__, ret);
	return ret;
}

ret_code_t protocol_send_radar_status(protocol_radar_status_t *status)
{
	ret_code_t ret = RET_OK;

	if (status == NULL)
	{
		ret = RET_INVALID_PARAM;
		goto out;
	}

	ret = protocol_udp_send_data_blocking(status, sizeof(*status));

out:
	LOG_DEBUG("[%s:%d] ret=%d\r\n", __FUNCTION__, __LINE__, ret);
	return ret;
}
