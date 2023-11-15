/*
 * module_interface_init.c
 *
 *  Created on: 2023年2月1日
 *      Author: A21001
 */

#include "xil_types.h"
#include "../sys_status_data/detection_param.h"
#include "../sys_status_data/detection_target.h"
#include "../../app/process_task/detect_interface.h"

int32_t SYS_OutsideInterfaceForDetInit(void)
{
	int32_t Result = 0;
	DET_OutsideInterface_t *pOutsideInterfaceInit = NULL;

	pOutsideInterfaceInit = DET_GetOutsideInterface();

	if (pOutsideInterfaceInit ==NULL)
	{
		Result = 1;
	}
	else
	{
		pOutsideInterfaceInit->pGetSysStatus = SYS_GetSysStatus;
		pOutsideInterfaceInit->pSetSysStatus = SYS_SetSysStatus;

		pOutsideInterfaceInit->pGetDetectionParam = SYS_GetDetectionParam;
		pOutsideInterfaceInit->pSetDetectionParam = SYS_SetDetectionParam;

		pOutsideInterfaceInit->pSetDetectionResult = SYS_SetDetectionResult;

//		pOutsideInterfaceInit->pSetDetectionAlgStatus = SYS_SetDetectionAlgStatus;
//		pOutsideInterfaceInit->pGetDetectionAlgStatus = SYS_GetDetectionAlgStatus;

		pOutsideInterfaceInit->pGetDetectFreqItem = SYS_GetDetectFreqItem;
		pOutsideInterfaceInit->pGetDroneTargetCnt = SYS_GetDroneTargetCnt;
	}

	return Result;
}

