/**
 * rpc pipe
 * portable
 *
 */

#include "rpc_pipe.h"





int rpc_pipe_send(rpc_pipe_ops_t *ops, void *data, int size)
{
	if (ops)
	{
		return ops->send(ops->param, data, size);
	}
	else
	{
		xil_printf("%s[%d] no pipe for RCP\r\n", __func__, __LINE__);
		return -1;
	}
}
int rpc_pipe_set_recv_cb(rpc_pipe_ops_t *ops, rpc_pipe_recv_cb_t cb, void *cb_param)
{
	if (ops)
	{
		return ops->set_recv_cb(ops->param, cb, cb_param);
	}
	else
	{
		xil_printf("%s[%d] no pipe for RCP\r\n", __func__, __LINE__);
		return -1;
	}
}





