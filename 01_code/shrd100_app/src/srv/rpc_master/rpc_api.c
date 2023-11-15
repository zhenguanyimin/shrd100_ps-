/**
 * rpc api
 *
 */
#if 0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#include "rpc_api.h"
#include "core_pipe.h"
#include "rpc_pipe_on_core_pipe.h"
#include "rpc_master.h"

static core_pipe_t pipe;
static rpc_pipe_ops_t ops;
static rpc_master_t rpc_master;

#define API_LOCK()   rpc_master_lock(&rpc_master);
#define API_UNLOCK() rpc_master_unlock(&rpc_master);

//always return the rpc function result
static int rpc_my_api(int *api_ret, const char *in_data, int in_size, char *out_data, int out_size)
{
	rpc_master_t *master;
	struct my_rpc_api_in_param_s in;
	struct my_rpc_api_out_param_s out;
	int ret;

	master = &rpc_master;
	
	in.in_data = in_data;
	in.in_size = in_size;
	in.out_size = out_size;
	
	ret = rpc_master_register_api(master, RPC_API__MY_RPC_API__NUM, &out, sizeof(out));
	if (ret)
	{
		return ret;
	}
	
	ret = rpc_master_call(master, RPC_API__MY_RPC_API__NUM, &in, sizeof(in));
	if (ret)
	{
		goto exit;
	}
	
	ret = rpc_master_wait_complete(master, RPC_API__MY_RPC_API__NUM, 1000);
	if (0 == ret) //ok, copy out param
	{
		*api_ret = out.ret;

		if (*api_ret > 0) //success comdition
		{
			memcpy(out_data, out.out_data, out.ret);
		}
	}
	else
	{
		xil_printf("%s[%d] rpc api wait complete error:%d\r\n", __func__, __LINE__, ret);
	}

exit:
	rpc_master_unregister_api(master, RPC_API__MY_RPC_API__NUM);
	
	return ret;
}

//return the real out data size;
int my_api(const char *in_data, int in_size, char *out_data, int out_size)
{
	int ret = -1; //NOTE:
	int rpc_ret;

	API_LOCK();
	rpc_ret = rpc_my_api(&ret, in_data, in_size, out_data, out_size);
	API_UNLOCK();
	
	if (rpc_ret) 
	{
		//rpc call error
		xil_printf("%s[%d]rpc call error:%d\r\n", __func__, __LINE__, rpc_ret);
		return -12345678;
	}
	else
	{
		return ret;
	}
}


void rpc_apis_init(void)
{
	#define CORE_ID (1)
	int ret;

	ret = core_pipe_init(&pipe, CORE_ID, 0);
	xil_printf("core pipe init:%d\r\n", ret);

	rpc_pipe_bind_with_core_pipe(&ops, &pipe);

	ret = rpc_master_init(&rpc_master, &ops);
	xil_printf("rpc_master_init:%d\r\n", ret);
}

void rpc_test(void)
{
	char buffer[64];
	int ret;
	static int once = 1;

	if (once)
	{
		once = 0;
		rpc_apis_init();
	}

	memset(buffer, 0, sizeof(buffer));
	ret = my_api("hello", 5, buffer, sizeof(buffer));
	xil_printf("my api:%d, buffer:%s\r\n", ret, buffer);
}

//api demo
void rpc_api_demo(void)
{
	char buffer[64];
	#define CORE_ID (1)
	int ret;
	
	core_pipe_init(&pipe, CORE_ID, 0);
	rpc_pipe_bind_with_core_pipe(&ops, &pipe);
	rpc_master_init(&rpc_master, &ops);

	vTaskDelay(100);

	while(1)
	{
		memset(buffer, 0, sizeof(buffer));
		ret = my_api("hello", 5, buffer, sizeof(buffer));
		xil_printf("my api:%d, buffer:%s\r\n", ret, buffer);

		vTaskDelay(100);
	}
	
	rpc_master_deinit(&rpc_master);
	core_pipe_deinit(&pipe);
}

#endif
 
