/**
 * rpc slaver
 *
 * install functions
 */

#ifndef __RPC_SLAVER_H_
#define __RPC_SLAVER_H_

#include "rpc_pipe.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "rpc_common.h"

#define CONFIG_RPC_SLAVER_TASK_STACK_SIZE (8192)
#define CONFIG_RPC_SLAVER_TASK_PRIORITY   (1)


enum
{
	RPC_SLAVER_RET_OK = 0,
	RPC_SLAVER_RET_ERR = -1,
};

typedef void(*rpc_slaver_func_t)(rpc_api_no_t api_no, void *in_param);

typedef struct 
{
	rpc_api_no_t api_no;
	rpc_slaver_func_t func;
	void *in_param;
	int in_param_size;
}rpc_slaver_func_item_t;


typedef struct 
{
	rpc_pipe_ops_t *ops;

	rpc_slaver_func_item_t *func_list;
	int func_counter;
	QueueHandle_t queue;

	TaskHandle_t task;
	
}rpc_slaver_t;



int rpc_slaver_init(rpc_slaver_t *rpc_slaver, rpc_pipe_ops_t *pipe_ops);

int rpc_slaver_register_api(rpc_slaver_t *rpc_slaver, const rpc_slaver_func_item_t *lists, int list_number);

int rpc_slaver_send_complete(rpc_slaver_t *rpc_slaver, rpc_api_no_t api_no);


int rpc_slaver_deinit(rpc_slaver_t *rpc_slaver);





















#endif


