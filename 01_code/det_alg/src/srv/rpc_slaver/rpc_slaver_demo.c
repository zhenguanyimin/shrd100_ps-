/**
 * rpc slaver
 */
#include <string.h>
#include "rpc_api.h"
#include "rpc_slaver.h"
#include "core_pipe.h"
#include "rpc_pipe.h"
#include "rpc_common.h"

static core_pipe_t pipe;
static rpc_pipe_ops_t ops;
static rpc_slaver_t rpc_slaver;
#define CORE_ID (0)



//demo api
struct my_rpc_api_param_s
{
	struct rpc_param_header_s param_header; //MUST be first

	//IN: same to the function parameter
	char *in_data;
	int in_size;
	int out_size;

	//OUT: same to the function parameter
	char *out_data;

	//API return value
	int *ret;
};

static struct my_rpc_api_param_s my_api_param;

int my_api(const char *in_data, int in_size, char *out_data, int out_size)
{
	///xil_printf("[%d]%s\r\n", in_size, in_data);
	memcpy(out_data, "world", 5);

	return 5;
}

static void rpc_my_api(rpc_api_no_t api_no, void *api_param)
{
	struct my_rpc_api_param_s *param = (struct my_rpc_api_param_s *)api_param;
	rpc_slaver_t *slaver = &rpc_slaver;

	*param->ret = my_api(param->in_data, param->in_size, param->out_data, param->out_size);
	rpc_slaver_send_complete(slaver, api_no);
}
//====================================================//
#if 1
//void droneSniffer(struct droneResult *droneInfo, int *nDrone, float ***specMat, float cenFreq, float fs, float bw, float gain)

#include "../../app/alg/det_alg/droneSniffer.h"

struct rpc_droneSniffer_param_s
{
	struct rpc_param_header_s param_header; //MUST be the first

	//IN: same to the function parameter
	float ***specMat;
	float ***upMat;
	float cenFreq;
	float fs;
	float bw;
	float gain;
	int useRow;

	//OUT: same to my_rpc_api function parameter
	struct droneResult *droneInfo;
	int *nDrone;
	float SNR;
	float *tmpAmp;
	//API return value
};


static struct rpc_droneSniffer_param_s droneSniffer_param;

static void rpc_droneSniffer(rpc_api_no_t api_no, void *api_param)
{
	struct rpc_droneSniffer_param_s *param = (struct rpc_droneSniffer_param_s *)api_param;
	rpc_slaver_t *slaver = &rpc_slaver;
	int useRows = 1500;
	// 计算测幅频率范围
	float tmpF = 0;
	float tmpBW = 0;
//	float dtF = fs / 128;
	float dtF;
	float tmpUp = 0;
	float tmpDown = 0;
	float tmpML;
	float tmpMW;
	float tmpAm;

	//xil_printf("%s[%d]in, spcMat add:%08X\r\n", __func__, __LINE__, (int)in->specMat);
	SetUAVtypesSNR(param->SNR);
//	droneSniffer(param->droneInfo, param->nDrone, param->specMat, param->cenFreq, param->fs, param->bw, param->gain);
//	droneSniffer_noFilt(param->droneInfo, param->nDrone, param->specMat, param->upMat, param->cenFreq, param->fs, param->bw, param->gain);
//	sfDroneSnifferV1(param->droneInfo, param->nDrone, param->specMat, param->upMat, param->cenFreq, param->fs, param->bw, param->gain, useRows);
	sfDroneSnifferV1(param->droneInfo, param->nDrone, param->specMat, param->upMat, param->cenFreq, param->fs, param->bw, param->gain, param->useRow);
	dtF = param->fs / 128;
	if ((*(param->nDrone)) > 0)
	{
//		tmpF = droneInfo[0].freq[0];
		tmpF = param->droneInfo[0].freq[0];
//		tmpBW = droneInfo[0].bw;
		tmpBW = param->droneInfo[0].bw;
//		tmpML = 64 + ((tmpF - cenFreq) / dtF);
		tmpML = 64 + ((tmpF - (param->cenFreq)) / dtF);
		tmpMW = ((tmpBW / dtF) / 2);
		tmpDown = (tmpML - tmpMW);
		tmpUp = (tmpML + tmpMW);

		tmpAm = sfDxCalCulateAm(param->specMat, (int)(tmpDown+2), (int)(tmpUp-2), param->droneInfo[0].flag, useRows);
		*(param->tmpAmp) = tmpAm;
	}
	else
	{
		*(param->tmpAmp) = 0;
	}



//	xil_printf("%s[%d]droneSniffer run over\r\n", __func__, __LINE__);

	rpc_slaver_send_complete(slaver, api_no);

	//xil_printf("%s[%d]out\r\n", __func__, __LINE__);
}
//====================================================//
#endif

const const rpc_slaver_func_item_t list[] = 
{
	//add your function 
	{RPC_API__MY_RPC_API__NUM, rpc_my_api, &my_api_param, sizeof(my_api_param)},
	{RPC_API__DRONE_SNIFFER__NUM, rpc_droneSniffer, &droneSniffer_param, sizeof(droneSniffer_param)},
};

void rpc_salver_demo(void)
{
	core_pipe_init(&pipe, CORE_ID, 0);
	rpc_pipe_bind_with_core_pipe(&ops, &pipe);
	rpc_slaver_init(&rpc_slaver, &ops);

	rpc_slaver_register_api(&rpc_slaver, list, sizeof(list)/sizeof(list[0]));
}

