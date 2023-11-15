/*
 * sd_hal.h
 *
 *  Created on: 2022��12��27��
 *      Author: A22745
 */

#ifndef HAL_SD_HAL_H_
#define HAL_SD_HAL_H_

#define SDPS_DEVICE_ID	0
#define SD_BLK_SIZE_512			(0x200U)	/**< Blk Size 512 */

#define SD_SIZE_GB				(32)
#define SD_MAX_SIZE_BYTE		(SD_SIZE_GB*1024*1024*1024LL)
#define SD_MAX_BLK				((SD_MAX_SIZE_BYTE) / (SD_BLK_SIZE_512))


uint8_t SdHalInit();
int SdHalReadSingleBlock(uint64_t addr, unsigned char *buf, int singleBlockBufLen);
int SdHalReadMultBlock(uint64_t addr, unsigned char *buf, int blockNum);
int SdHalWriteSingleBlock(uint64_t addr, unsigned char *buf, int singleBlockBufLen);
int SdHalWriteMultBlock(uint64_t addr, unsigned char *buf, int blockNum);

#endif /* HAL_SD_HAL_H_ */
