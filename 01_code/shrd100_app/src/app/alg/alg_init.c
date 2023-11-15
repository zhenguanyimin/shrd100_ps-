#include "alg_init.h"
/*
 *initialize algorithm
 */
void algorithm_init()
{
}

int32_t algorithm_Run(eDATA_PATH_INTR_FLAG flag, uint64_t *inputData, uint32_t dataLength, DroneID_ParseAllInfo_t *outList)
{
	return DroneID_main(flag, inputData, dataLength, outList);;
}

uint8_t AlgorithmStopFlag = 0;
void SetAlgorithmStopFlag(uint8_t value)
{
	AlgorithmStopFlag = value;
}

uint8_t GetAlgorithmStopFlag()
{
	return AlgorithmStopFlag;
}
