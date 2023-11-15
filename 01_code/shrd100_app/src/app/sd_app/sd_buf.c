/*
 * sd_app_write.c
 *
 *  Created on: 2022��12��27��
 *      Author: A22745
 */

/***************************** Include Files *********************************/

#include "sd_buf.h"
#include "sd_app.h"
#include "../../hal/sd/sd_hal.h"
#include "../../hal/sd/sd_ff_hal.h"
#include "../../drv/sd/sd_drv.h"
#include "../../srv/log/log.h"
#include "../../srv/ring/ring.h"

/************************** Constant Definitions *****************************/

#define BUFF_LEN	2048

/**************************** Type Definitions *******************************/


/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/

static uint8_t	log_buff[BUFF_LEN];
static uint8_t	data_buff[BUFF_LEN];
static uint8_t	debug_buff[BUFF_LEN];
static ring_t	log_Ring = { 0 };
static ring_t	data_Ring = { 0 };
static ring_t	debug_Ring = { 0 };

int32_t ResetLogBuf()
{
	ring_reset( &log_Ring );
	ring_reset( &data_Ring );
	ring_reset( &debug_Ring );
	return 0;
}
void SdBufInit(void)
{

	ring_init( &log_Ring, log_buff, sizeof(log_buff) );
	ring_init( &data_Ring, data_buff, sizeof(data_buff) );
	ring_init( &debug_Ring, debug_buff, sizeof(debug_buff) );
}

int32_t LogStoreToBuf(char *buf, uint8_t len)
{
	uint32_t ret = 0x00 ;
	BaseType_t status = pdTRUE;

	if( log_Ring.pvFront == 0x00 )
	{
		ring_init( &log_Ring, log_buff, sizeof(log_buff) );
	}

	ret = ring_inpt_data( &log_Ring , buf , len );

	return ret;
}

int32_t DataStoreToBuf(char *buf, uint16_t len)
{
	uint32_t ret = 0x00 ;
	BaseType_t status = pdTRUE;

	if( data_Ring.pvFront == 0x00 )
	{
		ring_init( &data_Ring, data_buff, sizeof(data_buff) );
	}

	ret = ring_inpt_data( &data_Ring , buf , len );


	return ret;
}

int32_t DebugStoreToBuf(char *buf, uint8_t len)
{
	uint32_t ret = 0x00 ;
	BaseType_t status = pdTRUE;

	if( debug_Ring.pvFront == 0x00 )
	{
		ring_init( &debug_Ring, debug_buff, sizeof(debug_buff) );
	}

	ret = ring_inpt_data( &debug_Ring , buf , len );

	return ret;
}

int BufSaveToSd( FIL *file , ring_t *Ring )
{

	XTime tick,tick1,tick2 ;
	uint32_t bw , data_len;
	uint8_t	temp_buff[SD_BLK_SIZE_512];

	XTime_GetTime(&tick1);

	data_len = ring_get_number(Ring);

	if( data_len >= SD_BLK_SIZE_512 )
	{

		ring_oupt_data( Ring , temp_buff , SD_BLK_SIZE_512);
		FsWriteFile( file , temp_buff , SD_BLK_SIZE_512 , &bw );
	}

	XTime_GetTime(&tick2);
	tick = (tick2 - tick1) * 1000000 / COUNTS_PER_SECOND ;

	return 0;

}


void CheckInfoBufSaveToSd( FIL *file )
{

	BufSaveToSd( file , &log_Ring );
}

void CheckDataBufSaveToSd( FIL *file )
{

	BufSaveToSd( file , &data_Ring );
}

void CheckDebugBufSaveToSd( FIL *file )
{

	BufSaveToSd( file , &debug_Ring );
}



