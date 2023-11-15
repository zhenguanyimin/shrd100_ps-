/*
 * hit_process_task.c
 *
 *  Created on: 2022-08-24
 *      Author: A19199
 */
/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include <queue.h>
#include <timers.h>

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xil_printf.h>
#include "key_handle.h"
#include "../../hal/input/input.h"
#include "../../cfg/shrd_config.h"

uint8_t Get2SwitchKey()
{
	uint8_t Status;
	Status = Input_GetValue(MIO_MUTE_SWITCH);

	return Status;
}

uint8_t Get4SwitchKey()
{
	uint8_t Status = 0;

	for( int i = 0  ; i < 4 ; i++ )
	{
		Status = Input_GetValue(MIO_SWITCH_MODE0+i);
		if( Status == 0x01 )
		{
			Status = i ;
			break;
		}
	}

	return Status;
}

