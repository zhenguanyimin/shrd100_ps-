/*
 * sys_status.h
 *
 *  Created on: 2023年1月19日
 *      Author: A21001
 */

#ifndef SRC_APP_SYS_STATUS_DATA_SYS_STATUS_H_
#define SRC_APP_SYS_STATUS_DATA_SYS_STATUS_H_

#include "xil_types.h"

typedef enum SYS_SystemStatusMode
{
	SYS_IDLE_STATUS = 0,
	SYS_DETECTION_TARGET_STATUS,
	SYS_HIT_TARGET_STATUS,
	SYS_SIMULATION_SIGAL_TO_TARGET_STATUS,
	SYS_MALFUNCTION_STATUS
} SYS_SystemStatusMode_t;

int32_t SYS_GetSysStatus(SYS_SystemStatusMode_t *pSysStaus);
void SYS_SetSysStatus(SYS_SystemStatusMode_t SysStaus);

#endif /* SRC_APP_SYS_STATUS_DATA_SYS_STATUS_H_ */
