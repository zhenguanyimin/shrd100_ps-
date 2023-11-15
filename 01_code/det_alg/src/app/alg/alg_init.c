#include "alg_init.h"
/*
 *initialize algorithm
 */
void algorithm_init()
{
}

int32_t algorithm_Run(sSignalDescriptionList *pSignalFeatureList, output_UAV_List *pOutputList)
{

	return 0;
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
