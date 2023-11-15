/*
 * alg_init.h
 *
 *  Created on: 2022-08-24
 *      Author: A19199
 */
#ifndef ALG_INIT_H
#define ALG_INIT_H
#include <stdio.h>
#include "../../inc/common_define.h"
#include "droneID/droneID_main.h"

void algorithm_init();
int32_t algorithm_Run(eDATA_PATH_INTR_FLAG flag, uint64_t *inputData, uint32_t dataLength, DroneID_ParseAllInfo_t *outList);

extern void SetAlgorithmStopFlag(uint8_t value);
extern uint8_t GetAlgorithmStopFlag();

#endif /* ALG_INIT_H */
