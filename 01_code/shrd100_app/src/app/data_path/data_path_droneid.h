
#ifndef __DATA_PATH_DRONEID_H__
#define __DATA_PATH_DRONEID_H__

/*========================================================================================
*                                     INCLUDE FILES
=========================================================================================*/
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "netif/xadapter.h"
#include "xtime_l.h"
#include "xil_cache.h"
#include "xil_types.h"
#include "timers.h"

/*========================================================================================
*                                      LOCAL MACROS
=========================================================================================*/
#define READ_DDR_0_256M_ADDR 		0x50000000
#define READ_DDR_256_512M_ADDR 		0x60000000
#define READ_DDR_BURST_ADDR 		0x60000000
#define VALID_ADC_LEN 				19960 // 9980*2
#define ADC_MAX_LEN_DRONEID 		(1024 * 1024 * 512) //512MB, adc data acquire max length used for droneid
/*========================================================================================
*                       LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
=========================================================================================*/
typedef enum
{
	eDATA_PATH_DDR_0_256M = 0x00,
	eDATA_PATH_DDR_256_512M = 0x01,
	eDATA_PATH_DDR_BURST = 0x02,
	eDATA_PATH_TIMER = 0x03,
} eDATA_PATH_INTR_FLAG;
/*========================================================================================
*                                GLOBAL VARIABLE DECLARATIONS
=========================================================================================*/

/*========================================================================================
*                                    FUNCTIONS
=========================================================================================*/
typedef void (*DataPathEvCb)(u8 *pBuf, s32 len);

extern TimerHandle_t droneid_timer ;
void SetBurstCheckEn( uint32_t para);
int32_t InitDataPathDroneID_Task(void);

void DataPath_Post(uint32_t bufaddr);
uint8_t *GetAdcBufDroneid(void);
eDATA_PATH_INTR_FLAG GetDataPathIntrFlag();

#endif
