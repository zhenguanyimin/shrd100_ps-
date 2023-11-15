/**
 * core pipe
 *
 */

#ifndef __CORE__PIPE__H__
#define __CORE__PIPE__H__

#include "core_mailbox.h"

/** error code ***/
enum
{
	CORE_PIPE_OK = 0,
	CORE_PIPE_ERR = CORE_MAILBOX_ERROR,
	CORE_PIPE_WRITE_TIMEOUT = CORE_MAILBOX_SEND_TIMEOUT,
	CORE_PIPE_WRITE_FAIL = CORE_MAILBOX_RSP_ERROR,
	CORE_PIPE_NO_READY = CORE_MAILBOX_DEST_CORE_NO_READY,
};

typedef struct
{
	uint32_t init;

	core_mailbox_t writer;
	core_mailbox_t reader;
}core_pipe_t;

typedef core_mailbox_cb_t core_pipe_cb_t;


//core id: 0~3
//pipe id: 0~7
int core_pipe_init(core_pipe_t *pipe, int core_id, int pipe_id);

int core_pipe_write(core_pipe_t *pipe, const char *data, uint32_t size, unsigned int time_out_ms);

int core_pipe_set_read_cb(core_pipe_t *pipe, core_pipe_cb_t cb, void *cb_param);

void core_pipe_deinit(core_pipe_t *pipe);

int core_pipe_is_ready(core_pipe_t *pipe);







#endif


