/**
 * rpc pipe
 *
 */

#ifndef __RPC_PIPE_H_
#define __RPC_PIPE_H_


typedef int (*rpc_pipe_recv_cb_t)(void *param, void *data, int size);

typedef int(*rpc_pipe_send_t)(void *param, void *data, int size);
typedef int(*rpc_pipe_set_recv_cb_t)(void *pipe_param, rpc_pipe_recv_cb_t cb, void *cb_param);

typedef struct 
{
	void *param;
	rpc_pipe_send_t send;
	rpc_pipe_set_recv_cb_t set_recv_cb;

	rpc_pipe_recv_cb_t recv_cb;
	void *cb_param;
}rpc_pipe_ops_t;


int rpc_pipe_send(rpc_pipe_ops_t *ops, void *data, int size);
int rpc_pipe_set_recv_cb(rpc_pipe_ops_t *ops, rpc_pipe_recv_cb_t cb, void *cb_param);




#endif


