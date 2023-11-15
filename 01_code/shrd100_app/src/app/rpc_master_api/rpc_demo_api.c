/**
 * a rpc master demo api
 */
#include <string.h>
#include <assert.h>
#include "xil_printf.h"
#include "rpc_master_api.h"
#include "../../srv/rpc_master/rpc_common.h"
#include "../../srv/log/log.h"


struct my_rpc_api_param_s
{
	struct rpc_param_header_s param_header; //MUST be first

	//IN: same to the function parameter
	char *in_data;
	int in_size;
	int out_size;

	//OUT: same to the function parameter
	char *out_data;

	//API return value
	int *ret;
};


//always return the rpc function result
static int rpc_my_api(rpc_master_t *master, int *api_ret, const char *in_data, int in_size, char *out_data, int out_size)
{
	struct my_rpc_api_param_s api_param;
	int ret;

	#define RPC_API_NUM (RPC_API__MY_RPC_API__NUM)

	assert(master);

	api_param.in_data = in_data;
	api_param.in_size = in_size;
	api_param.out_size = out_size;
	api_param.out_data = out_data;
	api_param.ret = api_ret;

	ret = rpc_master_register_api(master, RPC_API_NUM);
	if (ret)
	{
		return ret;
	}

	ret = rpc_master_call(master, RPC_API_NUM, &api_param, sizeof(api_param));
	if (ret)
	{
		goto exit;
	}

	ret = rpc_master_wait_complete(master, RPC_API_NUM, 1000);
	if (0 == ret)
	{
		///xil_printf("%s[%d] rpc api invoke complete\r\n", __func__, __LINE__);
	}
	else
	{
		LOG_ERROR("%s[%d] rpc api wait complete error:%d\r\n", __func__, __LINE__, ret);
	}

exit:
	rpc_master_unregister_api(master, RPC_API_NUM);

	return ret;
}

//return the real out data size;
int my_api(const char *in_data, int in_size, char *out_data, int out_size)
{
	int ret = -1; //NOTE:
	int rpc_ret;
	rpc_master_t *master = &rpc_master;

	//xil_printf("%s[%d]call \r\n", __func__, __LINE__);

	RPC_API_LOCK();
	rpc_ret = rpc_my_api(master, &ret, in_data, in_size, out_data, out_size);
	RPC_API_UNLOCK();

	if (rpc_ret)
	{
		//rpc call error
		LOG_ERROR("%s[%d]rpc call error:%d\r\n", __func__, __LINE__, rpc_ret);
		return -12345678;
	}
	else
	{
		return ret;
	}
}

void rpc_test(void)
{
	char buffer[64];
	int ret;
	TickType_t tick;

#if 0
	static int once = 1;

	if (once)
	{
		once = 0;
		rpc_mastser_api_init();
	}
#endif

	memset(buffer, 0, sizeof(buffer));
	tick = xTaskGetTickCount();
	ret = my_api("hello", 5, buffer, sizeof(buffer));
	if (ret < 0)
	{
		LOG_DEBUG("my api:%d,take:%d ms, buffer:%s\r\n", ret, xTaskGetTickCount() - tick, buffer);
	}
	else
	{
		///xil_printf("%s:%s\r\n", __func__, buffer);
	}
}

