/*
 * sd_drv.h
 *
 *  Created on: 2022Äê12ÔÂ27ÈÕ
 *      Author: A22745
 */

#ifndef DRV_SD_DRV_H_
#define DRV_SD_DRV_H_

int SdDrvInit(int num);
int SdDrvReadSingleBlock(uint32_t addr, unsigned char *buf, int singleBlockBufLen);
int SdDrvReadMultBlock(uint32_t addr, unsigned char *buf, int blockNum);
int SdDrvWriteSingleBlock(uint32_t addr, unsigned char *buf, int singleBlockBufLen);
int SdDrvWriteMultBlock(uint32_t addr, unsigned char *buf, int blockNum);

#endif /* DRV_SD_DRV_H_ */
