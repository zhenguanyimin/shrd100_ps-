/*
 * sd_hal.c
 *
 *  Created on: 2022��12��27��
 *      Author: A22745
 */
/***************************** Include Files *********************************/
#include "xsdps.h"		/* SD device driver */
#include "sd_hal.h"
#include "../../drv/sd/sd_drv.h"
#include "stdio.h"
/************************** Constant Definitions *****************************/

#define SD_BLK_SIZE_512_MASK	0x200U	/**< Blk Size 512 */
#define SD_BLK_SIZE_512_LEN		0x200U	/**< Blk Size 512 */
//#define SD_BLK_SIZE_512			(0x200U)	/**< Blk Size 512 */
//
//#define SD_SIZE_GB				(32U)
//#define SD_MAX_SIZE_BYTE		(SD_SIZE_GB * 1024 * 1024 * 1024)
//#define SD_MAX_BLK				((SD_MAX_SIZE_BYTE) / (SD_BLK_SIZE_512))
/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/


/*
 * Sd hal init
 */
uint8_t SdHalInit()
{
	uint8_t ret = 0;

	ret = SdDrvInit(SDPS_DEVICE_ID);

	return ret;
}

/*
 * check add is valid
 */
//int IsBlockaddValid(int blockAdd)
int IsBlockaddValid(uint64_t blockAdd)
{
//	if ((blockAdd >= 0) && (blockAdd < SD_MAX_BLK))
	if ((blockAdd >= 0) && (blockAdd < SD_MAX_SIZE_BYTE))
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

/*
 * sd single block read
 */
int SdHalReadSingleBlock(uint64_t addr, unsigned char *buf, int singleBlockBufLen)
{
	uint32_t sectorNum;
	int Status;

	if(buf == NULL || (singleBlockBufLen < SD_BLK_SIZE_512_LEN) || (IsBlockaddValid(addr) == 1))
	{
		return XST_FAILURE;
	}

	sectorNum = addr / SD_BLK_SIZE_512;
	Status = SdDrvReadSingleBlock(sectorNum, buf, 1);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	return XST_SUCCESS;
}

/*
 * sd mult block read
 */
int SdHalReadMultBlock(uint64_t addr, unsigned char *buf, int blockNum)
{
	uint32_t sectorNum;
	int Status;

	if(buf == NULL || (IsBlockaddValid(addr) == 1))
	{
		return XST_FAILURE;
	}

	sectorNum = addr / SD_BLK_SIZE_512;
	Status = SdDrvReadMultBlock(sectorNum, buf, blockNum);

	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	return XST_SUCCESS;
}

/*
 * sd single block write
 */
int SdHalWriteSingleBlock(uint64_t addr, unsigned char *buf, int singleBlockBufLen)
{
	uint32_t sectorNum;
	int Status;

	if(buf == NULL || (singleBlockBufLen < SD_BLK_SIZE_512_LEN) || (IsBlockaddValid(addr) == 1))
	{
		return XST_FAILURE;
	}

	sectorNum = addr / SD_BLK_SIZE_512;
	Status = SdDrvWriteSingleBlock(sectorNum, buf, 1);

	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	return XST_SUCCESS;
}

/*
 * sd Mult block write
 */
int SdHalWriteMultBlock(uint64_t addr, unsigned char *buf, int blockNum)
{
	uint32_t sectorNum;
	int Status;

	if(buf == NULL || (IsBlockaddValid(addr) == 1) )
	{
		return XST_FAILURE;
	}

	sectorNum = addr / SD_BLK_SIZE_512;
	Status = SdDrvWriteMultBlock(sectorNum, buf, blockNum);

	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	return XST_SUCCESS;
}


