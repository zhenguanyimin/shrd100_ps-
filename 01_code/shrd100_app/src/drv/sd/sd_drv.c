/*
 * sd_drv.c
 *
 *  Created on: 2022��12��27��
 *      Author: A22745
 */

/***************************** Include Files *********************************/

#include "xsdps.h"		/* SD device driver */
#include "sd_drv.h"
#include "stdio.h"
/************************** Constant Definitions *****************************/

//#define SD_BLK_SIZE_512_MASK	0x200U	/**< Blk Size 512 */
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


static XSdPs SdInstance0;
//static XSdPs SdInstance1;
static XSdPs *SdInstance;

/*
 * Sd Init
 */
int SdDrvInit(int num)
{
	XSdPs_Config *SdConfig;

	int deviceId;
	int Status;

	if (num == 0)
	{
		deviceId = XPAR_XSDPS_0_DEVICE_ID;
		SdInstance = &SdInstance0;
	}
//	else if (num == 1)
//	{
//		deviceId = XPAR_XSDPS_1_DEVICE_ID;
//		SdInstance = &SdInstance1;
//	}
	else
	{
		return XST_FAILURE;
	}

	/*
	 * Initialize the host controller
	 */
	SdConfig = XSdPs_LookupConfig(deviceId);
	if (NULL == SdConfig)
	{
		return XST_FAILURE;
	}

	Status = XSdPs_CfgInitialize(SdInstance, SdConfig,
					SdConfig->BaseAddress);
	if (Status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}

	Status = XSdPs_CardInitialize(SdInstance);
	if (Status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}



/*
 * sd single block read
 */
int SdDrvReadSingleBlock(uint32_t addr, unsigned char *buf, int singleBlockBufLen)
{
//	u32 sectorNum;
	int Status;

	if(buf == NULL)
	{
		return XST_FAILURE;
	}

	Status = XSdPs_ReadPolled(SdInstance, addr, 1, buf);

	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	return XST_SUCCESS;
}

/*
 * sd mult block read
 */
int SdDrvReadMultBlock(uint32_t addr, unsigned char *buf, int blockNum)
{
//	u32 sectorNum;
	int Status;

	if(buf == NULL)
	{
		return XST_FAILURE;
	}

	Status = XSdPs_ReadPolled(SdInstance, addr, blockNum, buf);

	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	return XST_SUCCESS;
}

/*
 * sd single block write
 */
int SdDrvWriteSingleBlock(uint32_t addr, unsigned char *buf, int singleBlockBufLen)
{
//	u32 sectorNum;
	int Status;

	if(buf == NULL)
	{
		return XST_FAILURE;
	}

	Status = XSdPs_WritePolled(SdInstance, addr, 1, buf);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	return XST_SUCCESS;
}

/*
 * sd Mult block write
 */
int SdDrvWriteMultBlock(uint32_t addr, unsigned char *buf, int blockNum)
{
//	u32 sectorNum;
	int Status;

	if(buf == NULL)
	{
		return XST_FAILURE;
	}

	Status = XSdPs_WritePolled(SdInstance, addr, blockNum, buf);

	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	return XST_SUCCESS;
}


