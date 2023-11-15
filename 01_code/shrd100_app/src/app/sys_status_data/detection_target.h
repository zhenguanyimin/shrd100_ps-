/*
 * detection_target.h
 *
 *  Created on: 2023年4月20日
 *      Author: A21001
 */

#ifndef SRC_APP_SYS_STATUS_DATA_DETECTION_TARGET_H_
#define SRC_APP_SYS_STATUS_DATA_DETECTION_TARGET_H_

#include "../alg/det_alg/droneSniffer.h"
#include "../../hal/hal_ad9361/ad9361_config.h"

#define SlideWinCntMAX 30
#define TimeoutCntMAX Freq_MAX_CNT * 3

typedef struct
{
	struct droneResult DroneResult;
	uint8_t RemainingTimes;
	uint8_t DetFreqItem;
} ReportResult_t;

typedef struct
{
	ReportResult_t ReportResult[SlideWinCntMAX];
	ReportResult_t DirDroneInfo;
	uint8_t DroneCnt;
} DroneReportResult_t;

void SpectrumSeamphoreCreate(void);
void SpectrumSeamphoreTake(void);
void SpectrumSeamphoreGive(void);
void ClearDroneInfoSlideWin(void);
int32_t SYS_GetDroneTargetInfo(DroneReportResult_t *pDroneReportResult_t);
int32_t SYS_SetDetectionResult(struct droneResult *pDroneResult, int nDrone, uint8_t FreqItem,  uint32_t FirstTagetUpdate);
uint8_t SYS_GetDroneTargetCnt(void);
void SetDirDroneInfo(void);
DroneReportResult_t* SYS_GetDroneInfo(void);

#endif /* SRC_APP_SYS_STATUS_DATA_DETECTION_TARGET_H_ */
