
#ifndef __DATA_STRANSFER_H__
#define __DATA_STRANSFER_H__

#include "xil_types.h"
#include "xaxidma.h"
#include "xscugic.h"

#define NO_DATA 1
#define DIR_DATA 1
#define RFFT_DATA 2
#define WIFI_DATA 3
#define DET_DATA 4
#define FFT_DATA 5
#define DATA_TYPE_MAX 6

#define ADC_PACKET_LEN (2 * 4096) // must be keep the same with pl


#define DET_DMA_PACKET_LEN (50 * 128 * 4 * 2)
#define FFT_DMA_PACKET_LEN (8 * 2048)
//#define DET_DMA_PACKET_LEN (6248*8)
#define MAT_PACKET_LEN DET_DMA_PACKET_LEN / 2

typedef void (*DataTransCallback)(s32 type, s32 len);

s32 data_trans_init(s32 type); // set buffer & init dma

void data_trans_deInit(s32 type);

s32 data_trans_start(s32 type, u8 *pBuf, s32 len); // for one time transfer

s32 data_trans_stop(s32 type);

s32 data_trans_set_callback(s32 type, DataTransCallback cb);

void data_trans_CacheInvalidate(u8 *pBuf, u32 len);

void data_trans_CacheFlush(u8 *pBuf, s32 len);

s32 data_trans_status(s32 type);

s32 data_trans_reset(s32 type);

#endif
