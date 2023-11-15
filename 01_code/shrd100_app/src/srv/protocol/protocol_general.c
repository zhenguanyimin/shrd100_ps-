
#include "protocol_general.h"

ret_code_t protocol_send_dev_info(protocol_dev_info_t *dev_info)
{
	ret_code_t ret = RET_OK;

	if (dev_info == NULL)
	{
		ret = RET_INVALID_PARAM;
		goto out;
	}

	ret = protocol_udp_send_data_blocking(dev_info, sizeof(*dev_info));

out:
	LOG_DEBUG("[%s:%d] ret=%d\r\n", __FUNCTION__, __LINE__, ret);
	return ret;
}
