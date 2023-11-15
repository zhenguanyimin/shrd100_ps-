/**
 * RPC slaver api
 */
 
#include "rpc_pipe.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "rpc_slaver.h"
#include <assert.h>

typedef struct 
{
	rpc_slaver_func_item_t *item;
}api_item_t;


static int pipe_recv_cb(void *param, void *data, int size)
{
	rpc_slaver_t *slaver = (rpc_slaver_t *)param;
	struct rpc_param_header_s *header = (struct rpc_param_header_s *)data;
	rpc_slaver_func_item_t *func_item;
	api_item_t api_item;
	BaseType_t xHigherPriorityTaskWoken;
	int i;
	int ret = -1;

	for (func_item = slaver->func_list, i = 0; i < slaver->func_counter; i++, func_item++)
	{
		if ((header->api_no == func_item->api_no) && (size <= func_item->in_param_size))
		{
			memcpy(func_item->in_param, data, size);
			api_item.item = func_item;
			
			xQueueSendFromISR(slaver->queue, &api_item, &xHigherPriorityTaskWoken);
			if (xHigherPriorityTaskWoken == pdTRUE)
			{
				portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
			}
			
			ret = 0;
			break;
		}
	}
	
	return ret;
}

static void slaver_task(void *param)
{
	rpc_slaver_t *slaver = (rpc_slaver_t *)param;
	api_item_t api_item;
	BaseType_t ret;
	
	xil_printf("%s[%d]run\r\n", __func__, __LINE__);
	while(1)
	{
		ret = xQueueReceive(slaver->queue, &api_item, portMAX_DELAY);
		
		///xil_printf("%s[%d]run function no:%d\r\n", __func__, __LINE__, api_item.item->api_no);
		api_item.item->func(api_item.item->api_no, api_item.item->in_param);
	}
}

int rpc_slaver_init(rpc_slaver_t *rpc_slaver, rpc_pipe_ops_t *pipe_ops)
{
	int ret = RPC_SLAVER_RET_OK;

	if (NULL == rpc_slaver)
	{
		return RPC_SLAVER_RET_ERR;
	}

	if (NULL == pipe_ops)
	{
		return RPC_SLAVER_RET_ERR;
	}
	
	rpc_slaver->ops = pipe_ops;
	rpc_slaver->func_list = NULL;
	rpc_slaver->func_counter = 0;
	rpc_slaver->task = NULL;
	rpc_slaver->queue = NULL;

	ret = rpc_pipe_set_recv_cb(pipe_ops, pipe_recv_cb, rpc_slaver);
	if (ret)
	{
		ret = RPC_SLAVER_RET_ERR;
		goto exit;
	}
	
	rpc_slaver->queue = xQueueCreate(1, sizeof(api_item_t));
	if (NULL == rpc_slaver->queue)
	{
		ret = RPC_SLAVER_RET_ERR;
		goto exit;
	}
	
	ret = xTaskCreate(slaver_task, "slave_task", CONFIG_RPC_SLAVER_TASK_STACK_SIZE, rpc_slaver, CONFIG_RPC_SLAVER_TASK_PRIORITY, 
				&rpc_slaver->task);
	if (pdPASS != ret)
	{
		///xil_printf("%s[%d]create slaver task error:%d\r\n", __func__, __LINE__, ret);
		ret = RPC_SLAVER_RET_ERR;
		goto exit;
	}
	else
	{
		ret = RPC_SLAVER_RET_OK;
	}

exit:
	if (ret != RPC_SLAVER_RET_OK)
	{
		if (rpc_slaver->queue)
		{
			vQueueDelete(rpc_slaver->queue);
			rpc_slaver->queue = NULL;
		}

		if (rpc_slaver->task)
		{
			vTaskDelete(rpc_slaver->task);
			rpc_slaver->task = NULL;
			
		}
	}

	return ret;
}

int rpc_slaver_register_api(rpc_slaver_t *rpc_slaver, const	rpc_slaver_func_item_t *lists, int list_number)
{
	assert(rpc_slaver);
	assert(lists);
	assert(list_number > 0);
	
	rpc_slaver->func_list = lists;
	rpc_slaver->func_counter = list_number;

	return 0;
}

int rpc_slaver_send_complete(rpc_slaver_t *rpc_slaver, rpc_api_no_t api_no)
{
	int ret;
	struct rpc_param_header_s header;

	assert(rpc_slaver);

	header.api_no = api_no;
	ret = rpc_pipe_send(rpc_slaver->ops, &header, sizeof(header));
	
	return ret;
}


int rpc_slaver_deinit(rpc_slaver_t *rpc_slaver)
{
	if (NULL == rpc_slaver)
	{
		return RPC_SLAVER_RET_ERR;
	}
	
	vTaskDelete(rpc_slaver->task);
	vQueueDelete(rpc_slaver->queue);

	rpc_slaver->task = NULL;
	rpc_slaver->queue = NULL;
	rpc_slaver->func_list = NULL;
	rpc_slaver->func_counter = 0;
	rpc_slaver->ops = NULL;

	return RPC_SLAVER_RET_OK;
}

