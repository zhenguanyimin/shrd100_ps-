#include <stdint.h>

#include "xqspipsu.h"
#include "xil_printf.h"
#include "flash.h"
#include "flash_communication.h"

static XQspiPsu QspiPsuInstance;
static XQspiPsu *QspiPsuInstanceAppPtr;
static uint8_t CmdBfr[8];
static uint32_t flash_page_size;

/*******************************************************************************
 * API - Implementation
 ******************************************************************************/
int32_t QspiInstanceInit(void)
{
	int32_t Status;

	QspiPsuInstanceAppPtr = &QspiPsuInstance;
	Status = QspiInstanceRegister(&QspiPsuInstance, &flash_page_size);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

/******************************************************************************
*
* This function reads from the serial FLASH connected to the QSPI interface.
*
* @param	Address contains the address to read data from in the FLASH.
* @param	ByteCount contains the number of bytes to read.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
int32_t flash_memory_readb(uint32_t Address, uint8_t *data, uint32_t ByteCount)
{
	int32_t Status;
	uint8_t ReadCmd;

	if (QspiPsuInstanceAppPtr->Config.BusWidth == BUSWIDTH_SINGLE)
		ReadCmd = FAST_READ_CMD;
	else if (QspiPsuInstanceAppPtr->Config.BusWidth == BUSWIDTH_DOUBLE)
		ReadCmd = DUAL_READ_CMD;
	else
		ReadCmd = QUAD_READ_CMD;

	Status = FlashRead(QspiPsuInstanceAppPtr, Address, ByteCount, ReadCmd, CmdBfr, data);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

/******************************************************************************
*
* This function erases 4k bytes data of flash.
*
* @param	Address contains the address to read data from in the FLASH.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
int32_t flash_memory_erase(uint32_t eraseAddr, uint32_t ByteCount)
{
	int32_t Status;

	Status = FlashErase(QspiPsuInstanceAppPtr, eraseAddr, ByteCount, CmdBfr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}


/******************************************************************************
*
* This function erases all data of flash.
*
* @param	none.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
int32_t bulkEraseQspiFlash(void)
{
	int32_t Status;

	Status = BulkErase(QspiPsuInstanceAppPtr, CmdBfr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

/******************************************************************************
*
* This function writes to the serial FLASH connected to the QSPI interface.
*
* @param	Address contains the address to read data from in the FLASH.
* @param	ByteCount contains the number of bytes to write.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
int32_t flash_memory_writeb(uint32_t Address, uint8_t *flashData, uint32_t ByteCount)
{
	// Setup the write command with the specified address and data for the FLASH
	int32_t Status;
	uint32_t singleByteCount;
	uint32_t page_cnt;
	uint8_t WriteCmd;

	WriteCmd = WRITE_CMD;
	page_cnt = ((ByteCount - 1) / flash_page_size) + 1;
	singleByteCount = flash_page_size;
	for (int i = 0; i < page_cnt; i++)
	{
		if (i == (page_cnt - 1) && (ByteCount % flash_page_size != 0))
		{
			singleByteCount = ByteCount % flash_page_size;
		}

		Status = FlashWrite(QspiPsuInstanceAppPtr, Address + flash_page_size * i, singleByteCount, WriteCmd, &flashData[flash_page_size * i]);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}
	}

	return Status;
}

/******************************************************************************
*
* This function reads from the serial FLASH connected to the QSPI interface.
*
* @param	None.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
int32_t flash_read_ID(void)
{
	int Status;

	Status = FlashReadID(QspiPsuInstanceAppPtr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}
