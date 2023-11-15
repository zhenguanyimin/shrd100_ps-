/*
 * detect_interface.c
 *
 *  Created on: 2023年2月20日
 *      Author: A21001
 */

#include "detect_interface.h"
#include "detect_process_task.h"

static DET_OutsideInterface_t stOutsideInterface = {NULL};
static DET_Interface_t DetInterface = {NULL};

DET_OutsideInterface_t* DET_GetOutsideInterface(void)
{
	return &stOutsideInterface;
}

DET_Interface_t* DET_GetDetInterface(void)
{
	return &DetInterface;
}
