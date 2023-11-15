/**
 * rpc master api define
 */


#include "../../srv/rpc_master/core_pipe.h"
#include "../../srv/rpc_master/rpc_pipe_on_core_pipe.h"
#include "../../srv/rpc_master/rpc_master.h"

#define CORE_ID (1)
#define DIR_CORE_ID (2)
#define WIFI_CORE_ID (3)

static core_pipe_t pipe;
static rpc_pipe_ops_t ops;
rpc_master_t rpc_master;

static core_pipe_t dir_pipe;
static rpc_pipe_ops_t dir_ops;
rpc_master_t rpc_dir_master;

static core_pipe_t wifi_pipe;
static rpc_pipe_ops_t wifi_ops;
rpc_master_t rpc_wifi_master;

//only call once
void rpc_mastser_api_init(void)
{

	core_pipe_init(&pipe, CORE_ID, 0);
	rpc_pipe_bind_with_core_pipe(&ops, &pipe);
	rpc_master_init(&rpc_master, &ops);

	core_pipe_init(&dir_pipe, DIR_CORE_ID, 3);
	rpc_pipe_bind_with_core_pipe(&dir_ops, &dir_pipe);
	rpc_master_init(&rpc_dir_master, &dir_ops);

	core_pipe_init(&wifi_pipe, WIFI_CORE_ID, 6);
	rpc_pipe_bind_with_core_pipe(&wifi_ops, &wifi_pipe);
	rpc_master_init(&rpc_wifi_master, &wifi_ops);
}



