/**
 * rpc master api
 */

#ifndef __RPC_MASTER_API_H_
#define __RPC_MASTER_API_H_


#include "../../srv/rpc_master/core_pipe.h"
#include "../../srv/rpc_master/rpc_pipe_on_core_pipe.h"
#include "../../srv/rpc_master/rpc_master.h"

extern rpc_master_t rpc_master;
extern rpc_master_t rpc_dir_master;
extern rpc_master_t rpc_wifi_master;
extern rpc_master_t rpc_master2;

#define RPC_API_LOCK()   rpc_master_lock(master);
#define RPC_API_UNLOCK() rpc_master_unlock(master);

/************************** API NUMBER DEFINE ********************/

enum
{
	RPC_API__MY_RPC_API__NUM = 1000,
	RPC_API__DRONE_SNIFFER__NUM = 1001,
};




/**
 * a demo api
 * in: "hello" string
 * out: "world" string, return code is 5
 */
extern int my_api(const char *in_data, int in_size, char *out_data, int out_size);

void rpc_mastser_api_init(void);
/**
 * alg apis
 */
#include "../alg/det_alg/droneSniffer.h"
extern void rpc_droneSniffer(struct droneResult *droneInfo, int *nDrone, float ***specMat, float cenFreq, float fs, float bw, float gain, int useRow, float *amp);
extern void rpc_droneSniffer_noFilt(struct droneResult *droneInfo, int *nDrone, float ***downMat, float ***upMat, float cenFreq, float fs, float bw, float gain, int useRow, float *amp);
extern void rpc_droneSniffer_noFilt2(struct droneResult *droneInfo, int *nDrone, float ***downMat, float ***upMat, float cenFreq, float fs, float bw, float gain, int useRow, float *amp);
extern void rpc_droneSniffer_noFilt3(struct droneResult *droneInfo, int *nDrone, float ***downMat, float ***upMat, float cenFreq, float fs, float bw, float gain, int useRow, float *amp);
#endif
