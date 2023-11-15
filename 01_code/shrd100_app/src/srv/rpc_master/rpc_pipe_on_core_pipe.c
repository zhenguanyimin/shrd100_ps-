/**
 * rpc_pipe_on_core_pipe
 *
 */

#include "core_pipe.h"
#include "rpc_pipe.h"
#include "../../srv/log/log.h"

static int send_to_core_pipe(void *param, void *data, int size)
{
	core_pipe_t *pipe;
	int ret;

	pipe = (core_pipe_t *)param;
	ret = core_pipe_write(pipe, data, size, 1000);
	if (0 == ret)
	{
		return 0;
	}
	else
	{
		LOG_ERROR("%s[%d]send to core pipe error:%d\r\n", __func__, __LINE__, ret);
		return -1;
	}
}

static int set_core_pipe_recv_cb(void *pipe_param, rpc_pipe_recv_cb_t cb, void *cb_param)
{
	core_pipe_t *pipe;
	int ret;

	pipe = (core_pipe_t *)pipe_param;
	ret = core_pipe_set_read_cb(pipe, cb, cb_param);
	if (0 == ret)
	{
		return 0;
	}
	else
	{
		LOG_ERROR("%s[%d]set read cb to core pipe error:%d\r\n", __func__, __LINE__, ret);
		return -1;
	}
}



int rpc_pipe_bind_with_core_pipe(rpc_pipe_ops_t *ops, void *pipe_param)
{
	ops->param = pipe_param;
	ops->send = send_to_core_pipe;
	ops->set_recv_cb = set_core_pipe_recv_cb;

	return 0;
}

