/**
 * rpc master api
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rpc_master.h"
#include "rpc_pipe.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include <assert.h>
#include "../../srv/log/log.h"

#if 0
#define RPC_MAX_API_ITEMS (10)

typedef struct 
{
	int used;
	rpc_api_no_t no;
	void *out_param;
	int out_size;
}api_items_t;

static api_items_t api_items[RPC_MAX_API_ITEMS];
#endif


int rpc_master_init(rpc_master_t *master, rpc_pipe_ops_t *ops)
{
	if (NULL == master)
	{
		return RPC_MASTER_RET_ERR;
	}

	if (NULL == ops)
	{
		return RPC_MASTER_RET_ERR;
	}

	master->api_lock = xSemaphoreCreateMutex();
	if (NULL == master->api_lock)
	{
		return RPC_MASTER_RET_ERR;
	}
	
	master->ops = ops;
	master->api_no = -1;
	master->out_param = NULL;
	master->out_size = 0;
	master->sem = NULL;
	
	return RPC_MASTER_RET_OK;
}

int rpc_master_deinit(rpc_master_t *master)
{
	if (NULL == master)
	{
		return RPC_MASTER_RET_ERR;
	}

	vSemaphoreDelete(master->api_lock);
	master->api_lock = NULL;
	master->ops = NULL;
	return RPC_MASTER_RET_OK;
}

int rpc_master_lock(rpc_master_t *master)
{
	assert(master);

	xSemaphoreTake(master->api_lock, portMAX_DELAY);
	return RPC_MASTER_RET_OK; 
}

int rpc_master_unlock(rpc_master_t *master)
{
	assert(master);

	xSemaphoreGive(master->api_lock);
	return RPC_MASTER_RET_OK; 
}

int rpc_master_call(rpc_master_t *master, rpc_api_no_t api_no, void *api_param, int param_size)
{
	int ret;
	struct rpc_param_header_s *header = (struct rpc_param_header_s *)api_param;

	assert(master);
	assert(api_param);
	assert(param_size >= sizeof(struct rpc_param_header_s));
	assert(master->ops);

	if (api_no != master->api_no)
	{
		return RPC_MASTER_RET_API_NO_REGISTER;
	}

	header->api_no = api_no;
	header->token = rand();
	
	ret = rpc_pipe_send(master->ops, api_param, param_size);
	if (ret)
	{
		ret = RPC_MASTER_RET_SEND_FAIL;
	}
	else
	{
		ret = RPC_MASTER_RET_OK;
	}

	return ret;
}


static int recv_cb(void *param, void *data, int size)
{
	rpc_master_t *master = (rpc_master_t *)param;
	struct rpc_param_header_s *param_header = (struct rpc_param_header_s *)data;
	BaseType_t xHigherPriorityTaskWoken;

	if (master->api_no == param_header->api_no)
	{
		xSemaphoreGiveFromISR(master->sem, &xHigherPriorityTaskWoken);
		if (xHigherPriorityTaskWoken == pdTRUE)
		{
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
		}

		return 0;
	}
	else
	{
//		LOG_ERROR("%s[%d]got unknown API complete signal, NO:%d\r\n", __func__, __LINE__, param_header->api_no);
		return -1;
	}
}

int rpc_master_register_api(rpc_master_t *master, rpc_api_no_t api_no)
{
	rpc_pipe_ops_t *ops;
	int ret = RPC_MASTER_RET_OK;
	
	ops = master->ops;

	master->api_no = api_no;
	master->out_param = NULL;
	master->out_size = 0;

	if (NULL == master->sem)
	{
		master->sem = xSemaphoreCreateBinary();
		if (NULL == master->sem)
		{
			ret = RPC_MASTER_RET_ERR;
			goto exit;
		}
	}
	
	ret = rpc_pipe_set_recv_cb(ops, recv_cb, master);
	if (ret)
	{
		ret = RPC_MASTER_RET_ERR;
		goto exit;
	}
	else
	{
		ret = RPC_MASTER_RET_OK;
	}

	exit:
	if (RPC_MASTER_RET_OK != ret)
	{
		if (master->sem)
		{
			vSemaphoreDelete(master->sem);
			master->sem = NULL;
		}
	}

	return ret;
}

int rpc_master_unregister_api(rpc_master_t *master, rpc_api_no_t api_no)
{
	rpc_pipe_ops_t *ops;
	
	if (NULL == master)
	{
		return RPC_MASTER_RET_OK;
	}

	ops = master->ops;
	
	rpc_pipe_set_recv_cb(ops, NULL, NULL);
	vSemaphoreDelete(master->sem);
	
	master->api_no = -1;
	master->out_param = NULL;
	master->out_size = 0;
	master->sem = NULL;

	return RPC_MASTER_RET_OK;
}


int rpc_master_wait_complete(rpc_master_t *master, rpc_api_no_t api_no, unsigned int time_out_ms)
{
	int ret;
	BaseType_t wait_ret;
	TickType_t block_time;

	if (time_out_ms == portMAX_DELAY)
	{
		block_time = portMAX_DELAY;
	}
	else
	{
		block_time = time_out_ms/portTICK_PERIOD_MS;
	}
	wait_ret = xSemaphoreTake(master->sem, block_time);
	if (pdTRUE == wait_ret)
	{
		ret = RPC_MASTER_RET_OK;
	}
	else
	{
		LOG_ERROR("%s[%d] sem take error:%d\r\n", __func__, __LINE__, wait_ret);
		ret = RPC_MASTER_RET_WAIT_TIMEROUT;
	}

	return ret;
}







