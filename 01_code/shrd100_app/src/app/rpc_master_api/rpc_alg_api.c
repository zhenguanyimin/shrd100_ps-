/**
 * rpc master api for alg
 */
#include <assert.h>

#include "../../srv/log/log.h"
#include "rpc_master_api.h"
#include "../../srv/rpc_master/rpc_common.h"

#if 1
//void droneSniffer(struct droneResult *droneInfo, int *nDrone, float ***specMat, float cenFreq, float fs, float bw, float gain)

#include "../alg/det_alg/droneSniffer.h"

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

float RpcUAVtypesSNR = 0.0f;
void SetRpcUAVtypesSNR(uint8_t value)
{
	RpcUAVtypesSNR = value;
}
static int rpc_call_droneSniffer(rpc_master_t *master, struct droneResult *droneInfo, int *nDrone, float ***specMat, float cenFreq, float fs, float bw, float gain, int useRow, float *amp)
{
	struct rpc_droneSniffer_param_s api_param;
	int ret;

	#define RPC_API_NUM (RPC_API__DRONE_SNIFFER__NUM)

	assert(master);

	api_param.specMat = specMat;
	api_param.cenFreq = cenFreq;
	api_param.fs = fs;
	api_param.bw = bw;
	api_param.gain = gain;
	api_param.droneInfo = droneInfo;
	api_param.nDrone = nDrone;
	api_param.SNR = RpcUAVtypesSNR;
	api_param.useRow = useRow;
	api_param.tmpAmp = amp;

	ret = rpc_master_register_api(master, RPC_API_NUM);
	if (ret)
	{
		return ret;
	}

	ret = rpc_master_call(master, RPC_API_NUM, &api_param, sizeof(api_param));
	if (ret)
	{
		goto exit;
	}

	ret = rpc_master_wait_complete(master, RPC_API_NUM, 1000*5);
	if (0 == ret)
	{
		LOG_DEBUG("%s[%d] rpc api invoke complete\r\n", __func__, __LINE__);
	}
	else
	{
		LOG_ERROR("%s[%d] rpc api wait complete error:%d\r\n", __func__, __LINE__, ret);
	}

exit:
	rpc_master_unregister_api(master, RPC_API__MY_RPC_API__NUM);

	return ret;
}

void rpc_droneSniffer(struct droneResult *droneInfo, int *nDrone, float ***specMat, float cenFreq, float fs, float bw, float gain, int useRow, float *amp)
{
	int rpc_ret;
	rpc_master_t *master = &rpc_master;

	RPC_API_LOCK();
	rpc_ret = rpc_call_droneSniffer(master, droneInfo, nDrone, specMat, cenFreq, fs, bw, gain, useRow, amp);
	RPC_API_UNLOCK();

	if (rpc_ret)
	{
		//rpc call error
		LOG_ERROR("%s[%d]rpc call error:%d\r\n", __func__, __LINE__, rpc_ret);
	}
}


static int rpc_call_droneSniffer_noFilt(rpc_master_t *master, struct droneResult *droneInfo, int *nDrone, float ***downMat, float ***upMat, float cenFreq, float fs, float bw, float gain, int useRow, float *amp)
{
	struct rpc_droneSniffer_param_s api_param;
	int ret;

	#define RPC_API_NUM (RPC_API__DRONE_SNIFFER__NUM)

	assert(master);

	api_param.specMat = downMat;
	api_param.upMat = upMat;
	api_param.cenFreq = cenFreq;
	api_param.fs = fs;
	api_param.bw = bw;
	api_param.gain = gain;
	api_param.droneInfo = droneInfo;
	api_param.nDrone = nDrone;
	api_param.useRow = useRow;
	api_param.tmpAmp = amp;

	ret = rpc_master_register_api(master, RPC_API_NUM);
	if (ret)
	{
		return ret;
	}

	ret = rpc_master_call(master, RPC_API_NUM, &api_param, sizeof(api_param));
	if (ret)
	{
		goto exit;
	}

	ret = rpc_master_wait_complete(master, RPC_API_NUM, 1000*5);
	if (0 == ret)
	{
//		LOG_DEBUG("%s[%d] rpc api invoke complete\r\n", __func__, __LINE__);
	}
	else
	{
//		LOG_DEBUG("%s[%d] rpc api wait complete error:%d\r\n", __func__, __LINE__, ret);
	}

exit:
	rpc_master_unregister_api(master, RPC_API__MY_RPC_API__NUM);

	return ret;
}

void rpc_droneSniffer_noFilt(struct droneResult *droneInfo, int *nDrone, float ***downMat, float ***upMat, float cenFreq, float fs, float bw, float gain, int useRow, float *amp)
{
	int rpc_ret;
	rpc_master_t *master = &rpc_master;

	RPC_API_LOCK();
	rpc_ret = rpc_call_droneSniffer_noFilt(master, droneInfo, nDrone, downMat, upMat, cenFreq, fs, bw, gain, useRow, amp);
	RPC_API_UNLOCK();

	if (rpc_ret)
	{
		//rpc call error
		LOG_ERROR("%s[%d]rpc call error:%d\r\n", __func__, __LINE__, rpc_ret);
	}
}

void rpc_droneSniffer_noFilt2(struct droneResult *droneInfo, int *nDrone, float ***downMat, float ***upMat, float cenFreq, float fs, float bw, float gain, int useRow, float *amp)
{
	int rpc_ret;
	rpc_master_t *master = &rpc_dir_master;

	RPC_API_LOCK();
	rpc_ret = rpc_call_droneSniffer_noFilt(master, droneInfo, nDrone, downMat, upMat, cenFreq, fs, bw, gain, useRow, amp);
	RPC_API_UNLOCK();

	if (rpc_ret)
	{
		//rpc call error
		LOG_DEBUG("%s[%d]rpc call error:%d\r\n", __func__, __LINE__, rpc_ret);
	}
}

void rpc_droneSniffer_noFilt3(struct droneResult *droneInfo, int *nDrone, float ***downMat, float ***upMat, float cenFreq, float fs, float bw, float gain, int useRow, float *amp)
{
	int rpc_ret;
	rpc_master_t *master = &rpc_wifi_master;

	RPC_API_LOCK();
	rpc_ret = rpc_call_droneSniffer_noFilt(master, droneInfo, nDrone, downMat, upMat, cenFreq, fs, bw, gain, useRow, amp);
	RPC_API_UNLOCK();

	if (rpc_ret)
	{
		//rpc call error
		LOG_DEBUG("%s[%d]rpc call error:%d\r\n", __func__, __LINE__, rpc_ret);
	}
}
#endif

