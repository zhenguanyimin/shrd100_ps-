/**
 **********************************************************************************************************************************************************
 * @file    : ring.c
 * @author  : liuxuhong
 * @version : V1.0
 * @date    : 2023-01-31
 * @brief   : ring
 **********************************************************************************************************************************************************
 * @note    :
 **********************************************************************************************************************************************************
 */

#ifndef __RING_H__
#define __RING_H__

/* includes */
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

/* defines */

/**
 * @brief: queue struct
 */
typedef struct ring
{
    volatile    uint32_t    uCount;     ///data count
                uint32_t    uSize;      ///buffer size
                void*    	pvInpt;    	///input index
                void*    	pvOupt;    	///output index
                void*    	pvFront;   	///buffer front
                void*    	pvRear;    	///buffer rear
}ring_t;

/* function */

void ring_init( ring_t* psRing, void* pvBuffer, uint32_t uSize );

void ring_reset( ring_t* psRing );

bool ring_inpt_byte( ring_t* psRing, uint8_t byByte );

bool ring_inpt_data( ring_t* psRing, void* pvData, uint32_t uLength );

bool ring_oupt_byte( ring_t* psRing, uint8_t *pbyByte );

uint32_t ring_oupt_data( ring_t* psRing, void* pvBuf, uint32_t uSize );

bool ring_get_byte( ring_t* psRing, uint32_t uPos, uint8_t *pbyByte );

bool ring_get_data( ring_t* psRing, uint32_t uPos, void* pvData, uint32_t uLength );

bool ring_delete_data( ring_t* psRing, uint32_t uLength );

uint32_t ring_get_number( ring_t* psRing );

uint32_t ring_get_empty( ring_t* psRing );

#endif
