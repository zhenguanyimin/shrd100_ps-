/**
 * core pipe, write and read
 * Communication between cores
 * base core mailbox
 */

#include "core_mailbox.h"
#include "core_pipe.h"

#define CURRENT_CORE_ID XScuGic_GetCpuID()

int core_pipe_init(core_pipe_t *pipe, int core_id, int pipe_id)
{
	int ret;
	uint32_t writer_source_int_id, writer_dest_int_id;
	uint32_t reader_source_int_id, reader_dest_int_id;

	if (NULL == pipe)
	{
		return CORE_PIPE_ERR;
	}

	if (CURRENT_CORE_ID == core_id)
	{
		return CORE_PIPE_ERR;
	}

	if (CURRENT_CORE_ID < core_id)
	{
		writer_source_int_id = pipe_id * 2;
		writer_dest_int_id = writer_source_int_id;

		reader_source_int_id = pipe_id * 2 + 1;
		reader_dest_int_id = reader_source_int_id;
	}
	else
	{
		writer_source_int_id = pipe_id * 2 + 1;
		writer_dest_int_id = writer_source_int_id;

		reader_source_int_id = pipe_id * 2;
		reader_dest_int_id = reader_source_int_id;
	}

	ret = core_mailbox_init(&pipe->writer, core_id, writer_source_int_id, writer_dest_int_id, CORE_MAILBOX_TYPE_WRITE);
	if (ret)
	{
		return CORE_PIPE_ERR;
	}

	ret = core_mailbox_init(&pipe->reader, core_id, reader_source_int_id, reader_dest_int_id, CORE_MAILBOX_TYPE_READ);
	if (ret)
	{
		core_mailbox_deinit(&pipe->writer);
		return CORE_PIPE_ERR;
	}

	pipe->init = 1;
	return CORE_PIPE_OK;
}

int core_pipe_write(core_pipe_t *pipe, const char *data, uint32_t size, unsigned int time_out_ms)
{
	if (NULL == pipe)
	{
		return CORE_PIPE_ERR;
	}

	if (!pipe->init)
	{
		return CORE_PIPE_ERR;
	}

	return core_mailbox_send(&pipe->writer, data, size, time_out_ms);
};

int core_pipe_set_read_cb(core_pipe_t *pipe, core_pipe_cb_t cb, void *cb_param)
{
	if (NULL == pipe)
	{
		return CORE_PIPE_ERR;
	}

	if (!pipe->init)
	{
		return CORE_PIPE_ERR;
	}

	return core_mailbox_set_recv_cb(&pipe->reader, cb, cb_param);
}

void core_pipe_deinit(core_pipe_t *pipe)
{
	if (NULL == pipe)
	{
		return;
	}

	if (!pipe->init)
	{
		return;
	}

	core_mailbox_deinit(&pipe->writer);
	core_mailbox_deinit(&pipe->reader);
	pipe->init = 0;
}

int core_pipe_is_ready(core_pipe_t *pipe)
{
	if (NULL == pipe)
	{
		return CORE_PIPE_ERR;
	}

	return (core_mailbox_core_is_ready(pipe->writer.dest_core_id, pipe->writer.dest_int_id)
			&& core_mailbox_core_is_ready(pipe->reader.dest_core_id, pipe->reader.dest_int_id));
}

//==================demo==========================
static int read_cb(void *param, const char *data, uint32_t size)
{
	xil_printf("read[%d]%s\r\n", size, data);
	return 0;
}

void core_pipe_demo(void)
{
	int ret;
	core_pipe_t pipe;

	ret = core_pipe_init(&pipe, 1, 0);
	xil_printf("pipe init:%d\r\n", ret);

	core_pipe_set_read_cb(&pipe, read_cb, NULL);

	while(1)
	{
		ret = core_pipe_write(&pipe, "app0 pipe", 9, 1000);
		xil_printf("core pipe write:%d\r\n", ret);

		vTaskDelay(100);
	}

	core_pipe_deinit(&pipe);
}

