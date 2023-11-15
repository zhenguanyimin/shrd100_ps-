/**
 * rpc api
 * base on rpc master
 */

#ifndef __RPC_API_H__
#define __RPC_API_H__


#if 0
#include "rpc_common.h"

enum
{
	RPC_API__MY_RPC_API__NUM = 1000,
};

//demo api
struct my_rpc_api_in_param_s
{
	struct rpc_param_header_s param_header; //MUST be the first

	char *in_data; //same to my_rpc_api function param
	int in_size;
	int out_size;
};

#define MY_RPC_API_OUT_DATA_MAX (512)
struct my_rpc_api_out_param_s
{
	struct rpc_param_header_s param_header; //MUST be the first
	
	int ret;      //the function my_rpc_api return code
	char out_data[MY_RPC_API_OUT_DATA_MAX]; //same to my_rpc_api function param, but set to max
};

//api with: ret code and in param


//api with: ret code

//api with: ret code and out param

//api with: nothing

//api with: in param

//api with: out param

//api with: in and out param



#endif

#endif

