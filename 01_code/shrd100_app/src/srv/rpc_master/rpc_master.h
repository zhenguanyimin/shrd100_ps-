/**
 * rpc master api
 *
 */

#ifndef __RPC_MASTER__H__
#define __RPC_MASTER__H__

#include "FreeRTOS.h"
#include "semphr.h"
#include "rpc_pipe.h"
#include "rpc_common.h"

enum
{
	RPC_MASTER_RET_OK = 0,
	RPC_MASTER_RET_ERR = -1,
	RPC_MASTER_RET_WAIT_TIMEROUT = -2,
	RPC_MASTER_RET_API_NO_REGISTER = -3,
	RPC_MASTER_RET_SEND_FAIL = -4, //pipe error or slaver has not this function
};



typedef struct 
{
	rpc_pipe_ops_t *ops;

	SemaphoreHandle_t sem;

	SemaphoreHandle_t api_lock;
	rpc_api_no_t api_no;
	void *out_param;
	int out_size;
	
}rpc_master_t;


int rpc_master_init(rpc_master_t *master, rpc_pipe_ops_t *ops);

int rpc_master_deinit(rpc_master_t *master);

int rpc_master_lock(rpc_master_t *master);

int rpc_master_unlock(rpc_master_t *master);

int rpc_master_call(rpc_master_t *master, rpc_api_no_t api_no, void *api_param, int param_size);

int rpc_master_register_api(rpc_master_t *master, rpc_api_no_t api_no);

int rpc_master_unregister_api(rpc_master_t *master, rpc_api_no_t api_no);


int rpc_master_wait_complete(rpc_master_t *master, rpc_api_no_t api_no, unsigned int time_out_ms);








#endif

