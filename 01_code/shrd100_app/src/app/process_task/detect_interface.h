/*
 * detect_interface.h
 *
 *  Created on: 2023年2月20日
 *      Author: A21001
 */

#ifndef SRC_APP_PROCESS_TASK_DETECT_INTERFACE_H_
#define SRC_APP_PROCESS_TASK_DETECT_INTERFACE_H_

#include "../sys_status_data/sys_status.h"
#include "../sys_status_data/detection_param.h"
#include "detect_process_task.h"

/* other module interface for detection module to call */
typedef struct DET_OutsideInterface
{
	int32_t (*pGetSysStatus)(SYS_SystemStatusMode_t *pSysStaus);
	void (*pSetSysStatus)(SYS_SystemStatusMode_t setSysStaus);

	int32_t (*pSetDetectionResult)(struct droneResult *pDroneResult, int nDrone, uint8_t FreqItem,  uint32_t FirstTagetUpdate);

	int32_t (*pGetDetectionParam)(SYS_DetectionParam_t *pDetParam);
	int32_t (*pSetDetectionParam)(const SYS_DetectionParam_t *pDetParam);

	void (*pSetDetectionAlgStatus)(uint32_t DetRunCoreID, uint32_t SetDetecionAlgStaus);
	uint32_t (*pGetDetectionAlgStatus)(uint32_t DetRunCoreID);

	uint8_t (*pGetDroneTargetCnt)(void);
	uint8_t (*pGetDetectFreqItem)(void);
} DET_OutsideInterface_t;


/* detection interface for other module module to call */
typedef struct DET_Interface
{
	int32_t (*pInit)(void);
}DET_Interface_t;

DET_OutsideInterface_t* DET_GetOutsideInterface(void);
DET_Interface_t* DET_GetDetInterface(void);

#endif /* SRC_APP_PROCESS_TASK_DETECT_INTERFACE_H_ */
