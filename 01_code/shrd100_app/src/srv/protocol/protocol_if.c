
#include "protocol_if.h"

ret_code_t protocol_init(void)
{
	ret_code_t ret = RET_OK;

	ret = protocol_common_init();

	return ret;
}
