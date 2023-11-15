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

void algorithm_init();
//int32_t algorithm_Run(sSignalDescriptionList *pSignalFeatureList, output_UAV_List *pOutputList);
//int32_t algorithm_Run1(uint8_t *pBuf, uint32_t len, pjieguoInfo *p_pwjieguoInfo);

extern void SetAlgorithmStopFlag(uint8_t value);
extern uint8_t GetAlgorithmStopFlag();

#endif /* ALG_INIT_H */
