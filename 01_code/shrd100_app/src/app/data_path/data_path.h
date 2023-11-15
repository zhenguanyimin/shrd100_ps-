
#ifndef __DATA_PATH_H__
#define __DATA_PATH_H__

/*========================================================================================
*                                     INCLUDE FILES
=========================================================================================*/
#include "xil_types.h"

#include "../../hal/data_transfer/data_transfer.h"

/*========================================================================================
*                                      LOCAL MACROS
=========================================================================================*/
#define POINT_PACK_NUM 2
#define DMA_PACK_NUM 60

#define NET_MAX_NUM 30
#define LINE_NUM 3000
#define COLUMN_NUM 128
#define REV_DET_PACK_LEN (COLUMN_NUM * LINE_NUM * 4 * POINT_PACK_NUM)

#define RAW_ADC_LEN (256 * 1024)
#define RDMAP_DATA_LEN (8 * 4096) //(8 * 2048) /* length of parameter estimate result from PL per dma interrupt, origin radar rdmap length is (256 * 1024) */
#define DDR_BASE_ADDR (0x10000000)
#define ADC_BASE_ADDR (DDR_BASE_ADDR + 0x6400000)
#define RDM_BASE_ADDR (ADC_BASE_ADDR + 2 * (RAW_ADC_LEN))

/*========================================================================================
*                       LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
=========================================================================================*/
typedef struct
{
	u32 flag; // 1 for in using ,0 for idle
	u32 type;
	u32 ts;
	u32 length; // length of the pData
	u8 *pData;
} buffer_t;

typedef struct
{
	buffer_t buff1;
	buffer_t buff2;
	volatile u32 revLen;
	u32 usedId;
} DataPingPong_t;

enum
{
	BUFF_STAT_START,
	BUFF_EMPTY,
	BUFF_IN_USED,
	BUFF_FULL,
	BUFF_STAT_MAX
};

/*========================================================================================
*                                GLOBAL VARIABLE DECLARATIONS
=========================================================================================*/

/*========================================================================================
*                                    FUNCTIONS
=========================================================================================*/
typedef void (*DataPathEvCb)(u8 *pBuf, s32 len);

void InitDataPath_Task();

s32 data_path_init(void);

void data_path_out_en(s32 type);

void dma_transfer_adc_init();

void SetOutputSpecMat(uint8_t value);

void SetLastDetFreqItem(uint8_t value);

s32 GetDataPathOutEn(void);

void ClearDetDmaBuf(void);

int32_t InitWifiDataPath_Task();
int32_t WifiMiniSEDataPath_Post(uint64_t flag);
void NetDataOutput(uint32_t framenum, uint8_t nDrone, uint8_t FreqItem);
#endif
