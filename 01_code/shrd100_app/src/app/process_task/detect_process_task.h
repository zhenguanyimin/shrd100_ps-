/*
 * detect_process_task.h
 *
 *  Created on: 2022-08-24
 *      Author: A19199
 */
#ifndef DETECT_PROCESS_TASK_H
#define DETECT_PROCESS_TASK_H
#include <stdio.h>
#include "../../inc/common_define.h"
#include "../alg/det_alg/droneSniffer.h"

#define MaxUAV 30

typedef struct DET_DetectionDataAddr
{
	uint32_t FrameNo;

	float ***pSpecMat;
	float ***pUpMat;
}DET_DetectionDataAddr_t;

void SYS_SetFrqRang( int frq_rang );
void printResult2(struct droneResult *droneInfo, int nDrone);
void DetectProcessTaskCore1_Post(uint32_t TotalFrameNo, float ***pSpecMat, float ***pUpMat);

int32_t InitDetectProcess_Task();
int32_t StartTimerWaringTimeout(void);
void ClearUAVWarnSlideWinInfo(void);
uint8_t GetLastDetFreqItem(void);
int32_t DET_CfgNextDetection(void);

#endif /* DETECT_PROCESS_TASK_H */
