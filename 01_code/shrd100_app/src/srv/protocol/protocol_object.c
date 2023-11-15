
#include "protocol_object.h"

ret_code_t protocol_send_object_list_detected(protocol_object_list_detected_t *obj_list)
{
	ret_code_t ret = RET_OK;

	if (obj_list == NULL)
	{
		ret = RET_INVALID_PARAM;
		goto out;
	}

	ret = protocol_udp_send_data_blocking(obj_list, sizeof(*obj_list));

out:
	LOG_DEBUG("[%s:%d] ret=%d\r\n", __FUNCTION__, __LINE__, ret);
	return ret;
}

ret_code_t protocol_send_object_list_tracked(protocol_object_list_tracked_t *obj_list)
{
	ret_code_t ret = RET_OK;

	if (obj_list == NULL)
	{
		ret = RET_INVALID_PARAM;
		goto out;
	}

	ret = protocol_udp_send_data_blocking(obj_list, sizeof(*obj_list));

out:
	LOG_DEBUG("[%s:%d] ret=%d\r\n", __FUNCTION__, __LINE__, ret);
	return ret;
}
