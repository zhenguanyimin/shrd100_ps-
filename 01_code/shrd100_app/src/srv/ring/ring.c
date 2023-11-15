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

/* includes */
#include "ring.h"

/* defines */

/* functions */
void ring_init( ring_t* psRing, void* pvBuffer, uint32_t uSize )
{
	psRing->pvFront   = pvBuffer;
	psRing->pvRear    = pvBuffer + uSize;
	psRing->pvInpt    = pvBuffer;
	psRing->pvOupt    = pvBuffer;
}

void ring_reset( ring_t* psRing )
{
	psRing->pvOupt	= psRing->pvInpt;
}

bool ring_inpt_byte( ring_t* psRing, uint8_t byByte )
{
	register void* pvIndex;
	pvIndex = psRing->pvInpt;
	if( psRing->pvRear <= ++pvIndex )
		pvIndex = psRing->pvFront;
	if( psRing->pvOupt == pvIndex )
		return false;
	*(uint8_t*)(psRing->pvInpt) = (uint8_t)byByte;
	psRing->pvInpt = pvIndex;
	return true;
}

bool ring_inpt_data( ring_t* psRing, void* pvData, uint32_t uLength )
{
	register uint32_t uTemp;
	register uint32_t uCount;
	uCount = ring_get_empty( psRing );
	if( uLength > uCount )
		return false;
	uCount = uLength;
	uTemp = psRing->pvRear - psRing->pvInpt;
	if( uTemp > uCount )
	{
		memcpy( psRing->pvInpt, pvData, uCount );
		psRing->pvInpt += uCount;
	}
	else
	{
		memcpy( psRing->pvInpt, pvData, uTemp );
		pvData += uTemp;
		uTemp = uCount - uTemp;
		if( uTemp )
			memcpy( psRing->pvFront, pvData, uTemp );
		psRing->pvInpt = psRing->pvFront + uTemp;
	}
	return true;
}

bool ring_oupt_byte( ring_t* psRing, uint8_t *pbyByte )
{
	if( psRing->pvInpt == psRing->pvOupt )
		return false;
	*pbyByte = *(uint8_t*)(psRing->pvOupt);
	if( psRing->pvRear <= ++psRing->pvOupt )
		psRing->pvOupt = psRing->pvFront;
	return true;
}

uint32_t ring_oupt_data( ring_t* psRing, void* pvBuf, uint32_t uSize )
{
	register uint32_t uTemp;
	register uint32_t uCount;
	register void* pvInpt;
	pvInpt = psRing->pvInpt;
	if( pvInpt == psRing->pvOupt )
		return 0;
	else if( pvInpt > psRing->pvOupt )
		uCount = pvInpt - psRing->pvOupt;
	else
		uCount = psRing->pvRear - psRing->pvOupt + pvInpt - psRing->pvFront;
	if( uCount > uSize )
		uCount = uSize;
	uTemp = psRing->pvRear - psRing->pvOupt;
	if( uTemp > uCount )
	{
		memcpy( pvBuf, psRing->pvOupt, uCount );
		psRing->pvOupt += uCount;
	}
	else
	{
		memcpy( pvBuf, psRing->pvOupt, uTemp );
		pvBuf += uTemp;
		uTemp = uCount - uTemp;
		if( uTemp )
			memcpy( psRing->pvFront, pvBuf, uTemp );
		psRing->pvOupt = psRing->pvFront + uTemp;
	}
	return uCount;
}

bool ring_get_byte( ring_t* psRing, uint32_t uPos, uint8_t *pbyByte )
{
	register uint32_t uCount;
	uCount = ring_get_number( psRing );
	if( uCount <= uPos )
		return false;
	if( psRing->pvRear > psRing->pvOupt + uPos )
		*pbyByte = *(uint8_t*)(psRing->pvOupt + uPos);
	else
		*pbyByte = *(uint8_t*)(psRing->pvOupt + uPos - psRing->pvRear);
	return true;
}

bool ring_get_data( ring_t* psRing, uint32_t uPos, void* pvData, uint32_t uLength )
{
	register uint32_t uTemp;
	register uint32_t uCount;
	register void* pvOupt;
	uCount = ring_get_number( psRing );
	if( uCount < uPos + uLength )
		return false;
	uTemp = psRing->pvRear - psRing->pvOupt;
	if( uTemp > uPos )
		pvOupt = psRing->pvOupt + uPos;
	else
		pvOupt = psRing->pvFront + uPos - uTemp;
	uTemp = psRing->pvRear - pvOupt;
	if( uTemp > uLength )
	{
		memcpy( pvData, pvOupt, uLength );
	}
	else
	{
		memcpy( pvData, pvOupt, uTemp );
		uTemp = uLength - uTemp;
		if( uTemp )
			memcpy( pvData, psRing->pvFront, uTemp );
	}
	return true;
}

bool ring_delete_data( ring_t* psRing, uint32_t uLength )
{
	register uint32_t uTemp;
	register uint32_t uCount;
	uCount = ring_get_number( psRing );
	if( uLength > uCount )
		return false;
	uTemp = psRing->pvRear - psRing->pvOupt;
	if( uTemp > uLength )
		psRing->pvOupt += uLength;
	else
		psRing->pvOupt = psRing->pvFront + uLength - uTemp;
	return true;
}

uint32_t ring_get_number( ring_t* psRing )
{
	register uint32_t uCount;
	register void* pvInpt;
	pvInpt = psRing->pvInpt;
	if( pvInpt >= psRing->pvOupt )
		uCount = pvInpt - psRing->pvOupt;
	else
		uCount = psRing->pvRear - psRing->pvOupt + pvInpt - psRing->pvFront;
	return uCount;
}

uint32_t ring_get_empty( ring_t* psRing )
{
	register uint32_t uCount;
	register void* pvOupt;
	pvOupt = psRing->pvOupt;
	if( pvOupt > psRing->pvInpt )
		uCount = pvOupt - psRing->pvInpt - 1;
	else
		uCount = psRing->pvRear - psRing->pvInpt + pvOupt - psRing->pvFront - 1;
	return uCount;
}
