/**
 * rpc_pipe_on_core_pipe
 *
 */

#ifndef __RPC_PIPE_ON_CORE_PIPE_H_
#define __RPC_PIPE_ON_CORE_PIPE_H_



#include "rpc_pipe.h"



//init the rpc pipe as core_pipe
int rpc_pipe_bind_with_core_pipe(rpc_pipe_ops_t *ops, void *pipe_param);







#endif

