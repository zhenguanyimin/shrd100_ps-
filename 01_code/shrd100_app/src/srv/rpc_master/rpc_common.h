/**
 * rpc common define
 *
 */

#ifndef __RPC_COMMON_H_
#define __RPC_COMMON_H_

#include <stdio.h>
#include <stdlib.h>

struct rpc_param_header_s
{
	uint32_t api_no;
	uint32_t token; //is a random number for ervery time.
};

typedef uint32_t rpc_api_no_t;




#endif

