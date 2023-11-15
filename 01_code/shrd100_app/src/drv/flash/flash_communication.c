#include <stdint.h>
#include "xqspipsu.h"
#include "xil_printf.h"
#include "flash_communication.h"
#include "../../srv/log/log.h"

#define QSPIPSU_DEVICE_ID		XPAR_XQSPIPSU_0_DEVICE_ID
/* Number of flash pages to be written. */
#define PAGE_COUNT				32
/* Max page size to initialize write and read buffer */
#define MAX_PAGE_SIZE 			1024
#define SECTOR_SIZE_4K 			4096

#define ENTER_4B				1
#define EXIT_4B					0

/*
 * The following constants define the offsets within a FlashBuffer data
 * type for each kind of data.  Note that the read data offset is not the
 * same as the write data because the QSPIPSU driver is designed to allow full
 * duplex transfers such that the number of bytes received is the number
 * sent and received.
 */
#define COMMAND_OFFSET		0 /* Flash instruction */
#define ADDRESS_1_OFFSET	1 /* MSB byte of address to read or write */
#define ADDRESS_2_OFFSET	2 /* Middle byte of address to read or write */
#define ADDRESS_3_OFFSET	3 /* LSB byte of address to read or write */
#define ADDRESS_4_OFFSET	4 /* LSB byte of address to read or write */
#define DATA_OFFSET			5 /* Start of Data for Read/Write */
#define DUMMY_OFFSET		4 /* Dummy byte offset for fast, dual and quad
				   * reads
				   */
#define DUMMY_SIZE			1 /* Number of dummy bytes for fast, dual and
				   * quad reads
				   */
#define DUMMY_CLOCKS		8 /* Number of dummy bytes for fast, dual and
				   * quad reads
				   */
#define RD_ID_SIZE			4 /* Read ID command + 3 bytes ID response */
#define BULK_ERASE_SIZE		1 /* Bulk Erase command size */
#define SEC_ERASE_SIZE		4 /* Sector Erase command + Sector address */
#define BANK_SEL_SIZE		2 /* BRWR or EARWR command + 1 byte bank
				   * value
				   */
#define RD_CFG_SIZE			2 /* 1 byte Configuration register + RD CFG
				   * command
				   */
#define WR_CFG_SIZE			3 /* WRR command + 1 byte each Status and
				   * Config Reg
				   */
#define DIE_ERASE_SIZE		4	/* Die Erase command + Die address */

/*
 * The following constants specify the extra bytes which are sent to the
 * Flash on the QSPIPSu interface, that are not data, but control information
 * which includes the command and address
 */
#define OVERHEAD_SIZE		4

static XQspiPsu_Msg FlashMsg[5];
static u8 TxBfrPtr;
static u8 ReadBfrPtr[3];
static u32 FlashMake;
static u32 FCTIndex;	/* Flash configuration table index */

static u8 StatusCmd;
//static u8 SectorEraseCmd;
static u8 FSRFlag;

/* The following constants specify the max amount of data and the size of the
* the buffer required to hold the data and overhead to transfer the data to
* and from the Flash. Initialized to single flash page size.
*/
u32 MaxData = PAGE_COUNT*256;

/************************** Variable Definitions *****************************/
FlashInfo Flash_Config_Table[] = {
	/************************** Spansion **************************/
	/*s25fl064l*/
	{0x016017, SECTOR_SIZE_64K, NUM_OF_SECTORS128, BYTES256_PER_PAGE,
		0x8000, 0x800000, 0xFFFF0000, 1},
	/*s25fl128l*/
	{0x016018, SECTOR_SIZE_64K, NUM_OF_SECTORS256, BYTES256_PER_PAGE,
		0x10000, 0x1000000, 0xFFFF0000, 1},
	/*s25fl256l*/
	{0x016019, SECTOR_SIZE_64K, NUM_OF_SECTORS512, BYTES256_PER_PAGE,
		0x20000, 0x2000000, 0xFFFF0000, 1},
	/*s25fl512s*/
	{0x010220, SECTOR_SIZE_256K, NUM_OF_SECTORS256, BYTES512_PER_PAGE,
		0x20000, 0x4000000, 0xFFFC0000, 1},
	/* Spansion 1Gbit is handled as 512Mbit stacked */
	/*************************** Micron ***************************/
	/*n25q128a11*/
	{0x20bb18, SECTOR_SIZE_64K, NUM_OF_SECTORS256, BYTES256_PER_PAGE,
		0x10000, 0x1000000, 0xFFFF0000, 1},
	/*n25q128a13*/
	{0x20ba18, SECTOR_SIZE_64K, NUM_OF_SECTORS256, BYTES256_PER_PAGE,
		0x10000, 0x1000000, 0xFFFF0000, 1},
	/*n25q256ax1*/
	{0x20bb19, SECTOR_SIZE_64K, NUM_OF_SECTORS512, BYTES256_PER_PAGE,
		0x20000, 0x2000000, 0xFFFF0000, 1},
	/*n25q256a*/
	{0x20ba19, SECTOR_SIZE_64K, NUM_OF_SECTORS512, BYTES256_PER_PAGE,
		0x20000, 0x2000000, 0xFFFF0000, 1},
	/*mt25qu512a*/
	{0x20bb20, SECTOR_SIZE_64K, NUM_OF_SECTORS1024, BYTES256_PER_PAGE,
		0x40000, 0x4000000, 0xFFFF0000, 2},
	/*n25q512ax3*/
	{0x20ba20, SECTOR_SIZE_64K, NUM_OF_SECTORS1024, BYTES256_PER_PAGE,
		0x40000, 0x4000000, 0xFFFF0000, 2},
	/*n25q00a*/
	{0x20bb21, SECTOR_SIZE_64K, NUM_OF_SECTORS2048, BYTES256_PER_PAGE,
		0x80000, 0x8000000, 0xFFFF0000, 4},
	/*n25q00*/
	{0x20ba21, SECTOR_SIZE_64K, NUM_OF_SECTORS2048, BYTES256_PER_PAGE,
		0x80000, 0x8000000, 0xFFFF0000, 4},
	/*mt25qu02g*/
	{0x20bb22, SECTOR_SIZE_64K, NUM_OF_SECTORS4096, BYTES256_PER_PAGE,
		0x100000, 0x10000000, 0xFFFF0000, 4},
	/*mt25ql02g*/
	{0x20ba22, SECTOR_SIZE_64K, NUM_OF_SECTORS4096, BYTES256_PER_PAGE,
		0x100000, 0x10000000, 0xFFFF0000, 4},
	/*************************** Winbond ***************************/
	/*w25q128fw*/
	{0xef6018, SECTOR_SIZE_64K, NUM_OF_SECTORS256, BYTES256_PER_PAGE,
		0x10000, 0x1000000, 0xFFFF0000, 1},
	/*w25q128jv*/
	{0xef7018, SECTOR_SIZE_64K, NUM_OF_SECTORS256, BYTES256_PER_PAGE,
		0x10000, 0x1000000, 0xFFFF0000, 1},
	/* Macronix */
	/*mx66l1g45g*/
	{0xc2201b, SECTOR_SIZE_64K, NUM_OF_SECTORS2048, BYTES256_PER_PAGE,
		0x80000, 0x8000000, 0xFFFF0000, 4},
	/*mx66l1g55g*/
	{0xc2261b, SECTOR_SIZE_64K, NUM_OF_SECTORS2048, BYTES256_PER_PAGE,
		0x80000, 0x8000000, 0xFFFF0000, 4},
	/*mx66u1g45g*/
	{0xc2253b, SECTOR_SIZE_64K, NUM_OF_SECTORS2048, BYTES256_PER_PAGE,
		0x80000, 0x8000000, 0xFFFF0000, 4},
	/*mx66l2g45g*/
	{0xc2201c, SECTOR_SIZE_64K, NUM_OF_SECTORS4096, BYTES256_PER_PAGE,
		0x100000, 0x10000000, 0xFFFF0000, 1},
	/*mx66u2g45g*/
	{0xc2253c, SECTOR_SIZE_64K, NUM_OF_SECTORS4096, BYTES256_PER_PAGE,
		0x100000, 0x10000000, 0xFFFF0000, 1},
	/**************************** ISSI ****************************/
	/*is25wp080d*/
	{0x9d7014, SECTOR_SIZE_64K, NUM_OF_SECTORS16, BYTES256_PER_PAGE,
		0x1000, 0x100000, 0xFFFF0000, 1},
	/*is25lp080d*/
	{0x9d6014, SECTOR_SIZE_64K, NUM_OF_SECTORS16, BYTES256_PER_PAGE,
		0x1000, 0x100000, 0xFFFF0000, 1},
	/*is25wp016d*/
	{0x9d7015, SECTOR_SIZE_64K, NUM_OF_SECTORS32, BYTES256_PER_PAGE,
		0x2000, 0x200000, 0xFFFF0000, 1},
	/*is25lp016d*/
	{0x9d6015, SECTOR_SIZE_64K, NUM_OF_SECTORS32, BYTES256_PER_PAGE,
		0x2000, 0x200000, 0xFFFF0000, 1},
	/*is25wp032*/
	{0x9d7016, SECTOR_SIZE_64K, NUM_OF_SECTORS64, BYTES256_PER_PAGE,
		0x4000, 0x400000, 0xFFFF0000, 1},
	/*is25lp032*/
	{0x9d6016, SECTOR_SIZE_64K, NUM_OF_SECTORS64, BYTES256_PER_PAGE,
		0x4000, 0x400000, 0xFFFF0000, 1},
	/*is25wp064*/
	{0x9d7017, SECTOR_SIZE_64K, NUM_OF_SECTORS128, BYTES256_PER_PAGE,
		0x8000, 0x800000, 0xFFFF0000, 1},
	/*is25lp064*/
	{0x9d6017, SECTOR_SIZE_64K, NUM_OF_SECTORS128, BYTES256_PER_PAGE,
		0x8000, 0x800000, 0xFFFF0000, 1},
	/*is25wp128*/
	{0x9d7018, SECTOR_SIZE_64K, NUM_OF_SECTORS256, BYTES256_PER_PAGE,
		0x10000, 0x1000000, 0xFFFF0000, 1},
	/*is25lp128*/
	{0x9d6018, SECTOR_SIZE_64K, NUM_OF_SECTORS256, BYTES256_PER_PAGE,
		0x10000, 0x1000000, 0xFFFF0000, 1},
	/*is25lp256d*/
	{0x9d6019, SECTOR_SIZE_64K, NUM_OF_SECTORS512, BYTES256_PER_PAGE,
		0x20000, 0x2000000, 0xFFFF0000, 1},
	/*is25wp256d*/
	{0x9d7019, SECTOR_SIZE_64K, NUM_OF_SECTORS512, BYTES256_PER_PAGE,
		0x20000, 0x2000000, 0xFFFF0000, 1},
	/*is25lp512m*/
	{0x9d601a, SECTOR_SIZE_64K, NUM_OF_SECTORS1024, BYTES256_PER_PAGE,
		0x40000, 0x4000000, 0xFFFF0000, 2},
	/*is25wp512m*/
	{0x9d701a, SECTOR_SIZE_64K, NUM_OF_SECTORS1024, BYTES256_PER_PAGE,
		0x40000, 0x4000000, 0xFFFF0000, 2},
	/*is25lp01g*/
	{0x9d601b, SECTOR_SIZE_64K, NUM_OF_SECTORS2048, BYTES256_PER_PAGE,
		0x80000, 0x8000000, 0xFFFF0000, 1},
	/*is25wp01g*/
	{0x9d701b, SECTOR_SIZE_64K, NUM_OF_SECTORS2048, BYTES256_PER_PAGE,
		0x80000, 0x8000000, 0xFFFF0000, 1}
};

/*******************************************************************************
 * Local Variables
 ******************************************************************************/
static INLINE u32 CalculateFCTIndex(u32 ReadId, u32 *FCTIndex)
{
	u32 Index;

	for (Index = 0; Index < sizeof(Flash_Config_Table)/sizeof(Flash_Config_Table[0]);
				Index++) {
		if (ReadId == Flash_Config_Table[Index].jedec_id) {
			*FCTIndex = Index;
			return XST_SUCCESS;
		}
	}

	return XST_FAILURE;
}

static int32_t EnterExit4BAddMode(XQspiPsu *QspiPsuPtr, uint32_t flag)
{
	int Status;

	if (Flash_Config_Table[FCTIndex].FlashDeviceSize > SIXTEENMB)
	{
		Status = FlashEnterExit4BAddMode(QspiPsuPtr, flag);
		if (Status != XST_SUCCESS)
		{
			return XST_FAILURE;
		}
	}

	return XST_SUCCESS;
}

/*****************************************************************************/
int32_t QspiInstanceRegister(XQspiPsu *QspiPsuPtr, uint32_t *pageSize)
{
	int Status;

	XQspiPsu_Config *QspiPsuConfig;

	/*
	 * Initialize the QSPI driver so that it's ready to use
	 */
	QspiPsuConfig = XQspiPsu_LookupConfig(QSPIPSU_DEVICE_ID);
	if (NULL == QspiPsuConfig) {
		return XST_FAILURE;
	}

	Status = XQspiPsu_CfgInitialize(QspiPsuPtr, QspiPsuConfig,
			QspiPsuConfig->BaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Set Manual Start
	 */
	XQspiPsu_SetOptions(QspiPsuPtr, XQSPIPSU_MANUAL_START_OPTION);

	/*
	 * Set the pre-scaler for QSPI clock
	 */
	Status = XQspiPsu_SetClkPrescaler(QspiPsuPtr, XQSPIPSU_CLK_PRESCALE_8);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	XQspiPsu_SelectFlash(QspiPsuPtr, XQSPIPSU_SELECT_FLASH_CS_LOWER,
						 XQSPIPSU_SELECT_FLASH_BUS_LOWER);

	/*
	 * Read flash ID and obtain all flash related information
	 * It is important to call the read id function before
	 * performing proceeding to any operation, including
	 * preparing the WriteBuffer
	 */
	Status = FlashReadID(QspiPsuPtr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	LOG_DEBUG("Flash connection mode : %d\n\r",
			QspiPsuConfig->ConnectionMode);
	LOG_DEBUG("where 0 - Single; 1 - Stacked; 2 - Parallel\n\r");
	LOG_DEBUG("FCTIndex: %d\n\r", FCTIndex);

	/*
	 * Initialize MaxData according to page size.
	 */
	if (QspiPsuPtr->Config.ConnectionMode == XQSPIPSU_CONNECTION_MODE_PARALLEL) {
		*pageSize = Flash_Config_Table[FCTIndex].PageSize * 2;
	} else {
		*pageSize = Flash_Config_Table[FCTIndex].PageSize;
	}

	Status = FlashEnableQuadMode(QspiPsuPtr);
	if (Status != XST_SUCCESS)
		return XST_FAILURE;

	/* Status cmd - SR or FSR selection */
	if ((Flash_Config_Table[FCTIndex].NumDie > 1) &&
			(FlashMake == MICRON_ID_BYTE0)) {
		StatusCmd = READ_FLAG_STATUS_CMD;
		FSRFlag = 1;
	} else {
		StatusCmd = READ_STATUS_CMD;
		FSRFlag = 0;
	}

	return XST_SUCCESS;
}

/**
 *
 * Reads the flash ID and identifies the flash in FCT table.
 *
 * @param	QspiPsuPtr is a pointer to the QSPIPSU driver component to use.
 *
 * @return	XST_SUCCESS if successful, else XST_FAILURE.
 *
 * @note	None.
 *
 *****************************************************************************/
int FlashReadID(XQspiPsu *QspiPsuPtr)
{
	int Status;
	u32 ReadId = 0;

	/*
	 * Read ID
	 */
	TxBfrPtr = READ_ID;
	FlashMsg[0].TxBfrPtr = &TxBfrPtr;
	FlashMsg[0].RxBfrPtr = NULL;
	FlashMsg[0].ByteCount = 1;
	FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
	FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

	FlashMsg[1].TxBfrPtr = NULL;
	FlashMsg[1].RxBfrPtr = ReadBfrPtr;
	FlashMsg[1].ByteCount = 3;
	FlashMsg[1].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
	FlashMsg[1].Flags = XQSPIPSU_MSG_FLAG_RX;

	Status = XQspiPsu_PolledTransfer(QspiPsuPtr, FlashMsg, 2);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	LOG_DEBUG("FlashID=0x%x 0x%x 0x%x\n\r", ReadBfrPtr[0], ReadBfrPtr[1],
		   ReadBfrPtr[2]);

	/* In case of dual, read both and ensure they are same make/size */

	/*
	 * Deduce flash make
	 */
	FlashMake = ReadBfrPtr[0];

	ReadId = ((ReadBfrPtr[0] << 16) | (ReadBfrPtr[1] << 8) | ReadBfrPtr[2]);
	/*
	 * Assign corresponding index in the Flash configuration table
	 */
	Status = CalculateFCTIndex(ReadId, &FCTIndex);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

/*****************************************************************************/
/**
 *
 * This function writes to the  serial Flash connected to the QSPIPSU interface.
 * All the data put into the buffer must be in the same page of the device with
 * page boundaries being on 256 byte boundaries.
 *
 * @param	QspiPsuPtr is a pointer to the QSPIPSU driver component to use.
 * @param	Address contains the address to write data to in the Flash.
 * @param	ByteCount contains the number of bytes to write.
 * @param	Command is the command used to write data to the flash. QSPIPSU
 *		device supports only Page Program command to write data to the
 *		flash.
 * @param	WriteBfrPtr is pointer to the write buffer (which is to be transmitted)
 *
 * @return	XST_SUCCESS if successful, else XST_FAILURE.
 *
 * @note	None.
 *
 ******************************************************************************/
int FlashWrite(XQspiPsu *QspiPsuPtr, u32 Address, u32 ByteCount, u8 Command, u8 *WriteBfrPtr)
{
	u8 WriteEnableCmd;
	u8 ReadStatusCmd;
	u8 FlashStatus[2];
	u8 WriteCmd[5];
	u32 RealAddr;
	u32 CmdByteCount;
	int Status;

	WriteEnableCmd = WRITE_ENABLE_CMD;
	/*
	 * Translate address based on type of connection
	 * If stacked assert the slave select based on address
	 */
	RealAddr = GetRealAddr(QspiPsuPtr, Address);

	Status = EnterExit4BAddMode(QspiPsuPtr, ENTER_4B);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Send the write enable command to the Flash so that it can be
	 * written to, this needs to be sent as a separate transfer before
	 * the write
	 */
	FlashMsg[0].TxBfrPtr = &WriteEnableCmd;
	FlashMsg[0].RxBfrPtr = NULL;
	FlashMsg[0].ByteCount = 1;
	FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
	FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

	Status = XQspiPsu_PolledTransfer(QspiPsuPtr, FlashMsg, 1);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	WriteCmd[COMMAND_OFFSET] = Command;

	/* To be used only if 4B address program cmd is supported by flash */
	if (Flash_Config_Table[FCTIndex].FlashDeviceSize > SIXTEENMB) {
		WriteCmd[ADDRESS_1_OFFSET] =
				(u8)((RealAddr & 0xFF000000) >> 24);
		WriteCmd[ADDRESS_2_OFFSET] =
				(u8)((RealAddr & 0xFF0000) >> 16);
		WriteCmd[ADDRESS_3_OFFSET] =
				(u8)((RealAddr & 0xFF00) >> 8);
		WriteCmd[ADDRESS_4_OFFSET] =
				(u8)(RealAddr & 0xFF);
		CmdByteCount = 5;
	} else {
		WriteCmd[ADDRESS_1_OFFSET] =
				(u8)((RealAddr & 0xFF0000) >> 16);
		WriteCmd[ADDRESS_2_OFFSET] =
				(u8)((RealAddr & 0xFF00) >> 8);
		WriteCmd[ADDRESS_3_OFFSET] =
				(u8)(RealAddr & 0xFF);
		CmdByteCount = 4;
	}

	FlashMsg[0].TxBfrPtr = WriteCmd;
	FlashMsg[0].RxBfrPtr = NULL;
	FlashMsg[0].ByteCount = CmdByteCount;
	FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
	FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

	FlashMsg[1].TxBfrPtr = WriteBfrPtr;
	FlashMsg[1].RxBfrPtr = NULL;
	FlashMsg[1].ByteCount = ByteCount;
	FlashMsg[1].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
	FlashMsg[1].Flags = XQSPIPSU_MSG_FLAG_TX;
	if (QspiPsuPtr->Config.ConnectionMode ==
			XQSPIPSU_CONNECTION_MODE_PARALLEL) {
		FlashMsg[1].Flags |= XQSPIPSU_MSG_FLAG_STRIPE;
	}

	Status = XQspiPsu_PolledTransfer(QspiPsuPtr, FlashMsg, 2);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Wait for the write command to the Flash to be completed, it takes
	 * some time for the data to be written
	 */
	while (1) {
		ReadStatusCmd = StatusCmd;
		FlashMsg[0].TxBfrPtr = &ReadStatusCmd;
		FlashMsg[0].RxBfrPtr = NULL;
		FlashMsg[0].ByteCount = 1;
		FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
		FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

		FlashMsg[1].TxBfrPtr = NULL;
		FlashMsg[1].RxBfrPtr = FlashStatus;
		FlashMsg[1].ByteCount = 2;
		FlashMsg[1].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
		FlashMsg[1].Flags = XQSPIPSU_MSG_FLAG_RX;
		if (QspiPsuPtr->Config.ConnectionMode ==
				XQSPIPSU_CONNECTION_MODE_PARALLEL) {
			FlashMsg[1].Flags |= XQSPIPSU_MSG_FLAG_STRIPE;
		}

		Status = XQspiPsu_PolledTransfer(QspiPsuPtr, FlashMsg, 2);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}

		if (QspiPsuPtr->Config.ConnectionMode ==
				XQSPIPSU_CONNECTION_MODE_PARALLEL) {
			if (FSRFlag) {
				FlashStatus[1] &= FlashStatus[0];
			} else {
				FlashStatus[1] |= FlashStatus[0];
			}
		}

		if (FSRFlag) {
			if ((FlashStatus[1] & 0x80) != 0) {
				break;
			}
		} else {
			if ((FlashStatus[1] & 0x01) == 0) {
				break;
			}
		}
	}

	if (Flash_Config_Table[FCTIndex].FlashDeviceSize > SIXTEENMB) {
		Status = FlashEnterExit4BAddMode(QspiPsuPtr, EXIT_4B);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}
	}

	Status = EnterExit4BAddMode(QspiPsuPtr, EXIT_4B);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

/*****************************************************************************/
/**
 *
 * This function erases the sectors in the  serial Flash connected to the
 * QSPIPSU interface.
 *
 * @param	QspiPsuPtr is a pointer to the QSPIPSU driver component to use.
 * @param	Address contains the address of the first sector which needs to
 *		be erased.
 * @param	ByteCount contains the total size to be erased.
 * @param	WriteBfrPtr is pointer to the write buffer (which is to be transmitted)
 *
 * @return	XST_SUCCESS if successful, else XST_FAILURE.
 *
 * @note	None.
 *
 ******************************************************************************/
int FlashErase(XQspiPsu *QspiPsuPtr, u32 Address, u32 ByteCount, u8 *WriteBfrPtr)
{
	u8 WriteEnableCmd;
	u8 ReadStatusCmd;
	u8 FlashStatus[2];
	int Sector;
	u32 RealAddr;
	u32 NumSect;
	int Status;
	u32 SectSize;
	u32 SectMask;

	WriteEnableCmd = WRITE_ENABLE_CMD;

	if(QspiPsuPtr->Config.ConnectionMode == XQSPIPSU_CONNECTION_MODE_PARALLEL) {
		SectMask = (Flash_Config_Table[FCTIndex]).SectMask - (Flash_Config_Table[FCTIndex]).SectSize;
		SectSize = (Flash_Config_Table[FCTIndex]).SectSize * 2;
	} else if (QspiPsuPtr->Config.ConnectionMode == XQSPIPSU_CONNECTION_MODE_STACKED) {
		NumSect = (Flash_Config_Table[FCTIndex]).NumSect * 2;
		SectMask = (Flash_Config_Table[FCTIndex]).SectMask;
	} else {
		SectSize = (Flash_Config_Table[FCTIndex]).SectSize;
		NumSect = (Flash_Config_Table[FCTIndex]).NumSect;
		SectMask = (Flash_Config_Table[FCTIndex]).SectMask;
	}

	Status = EnterExit4BAddMode(QspiPsuPtr, ENTER_4B);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * If erase size is same as the total size of the flash, use bulk erase
	 * command or die erase command multiple times as required
	 */
	if (ByteCount == NumSect * SectSize) {

		if (QspiPsuPtr->Config.ConnectionMode ==
				XQSPIPSU_CONNECTION_MODE_STACKED) {
			XQspiPsu_SelectFlash(QspiPsuPtr,
				XQSPIPSU_SELECT_FLASH_CS_LOWER,
				XQSPIPSU_SELECT_FLASH_BUS_LOWER);
		}

		if (Flash_Config_Table[FCTIndex].NumDie == 1) {
			/*
			 * Call Bulk erase
			 */
			BulkErase(QspiPsuPtr, WriteBfrPtr);
		}

		if (Flash_Config_Table[FCTIndex].NumDie > 1) {
			/*
			 * Call Die erase
			 */
			DieErase(QspiPsuPtr, WriteBfrPtr);
		}
		/*
		 * If stacked mode, bulk erase second flash
		 */
		if (QspiPsuPtr->Config.ConnectionMode ==
				XQSPIPSU_CONNECTION_MODE_STACKED) {

			XQspiPsu_SelectFlash(QspiPsuPtr,
				XQSPIPSU_SELECT_FLASH_CS_UPPER,
				XQSPIPSU_SELECT_FLASH_BUS_LOWER);

			if (Flash_Config_Table[FCTIndex].NumDie == 1) {
				/*
				 * Call Bulk erase
				 */
				BulkErase(QspiPsuPtr, WriteBfrPtr);
			}

			if (Flash_Config_Table[FCTIndex].NumDie > 1) {
				/*
				 * Call Die erase
				 */
				DieErase(QspiPsuPtr, WriteBfrPtr);
			}
		}

		return 0;
	}

	/*
	 * If the erase size is less than the total size of the flash, use
	 * sector erase command
	 */

	/*
	 * Calculate no. of sectors to erase based on byte count
	 */
//	NumSect = (ByteCount / SectSize) + 1;
	NumSect = (ByteCount - 1) / SECTOR_SIZE_4K + 1;

	/*
	 * If ByteCount to k sectors,
	 * but the address range spans from N to N+k+1 sectors, then
	 * increment no. of sectors to be erased
	 */

//	if (((Address + ByteCount) & SectMask) ==
//		((Address + (NumSect * SectSize)) & SectMask)) {
//		NumSect++;
//	}

	for (Sector = 0; Sector < NumSect; Sector++) {

		/*
		 * Translate address based on type of connection
		 * If stacked assert the slave select based on address
		 */
		RealAddr = GetRealAddr(QspiPsuPtr, Address);

		/*
		 * Send the write enable command to the Flash so that it can be
		 * written to, this needs to be sent as a separate
		 * transfer before the write
		 */
		FlashMsg[0].TxBfrPtr = &WriteEnableCmd;
		FlashMsg[0].RxBfrPtr = NULL;
		FlashMsg[0].ByteCount = 1;
		FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
		FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

		Status = XQspiPsu_PolledTransfer(QspiPsuPtr, FlashMsg, 1);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}

		WriteBfrPtr[COMMAND_OFFSET]  = SEC_ERASE_4K_CMD;
//		WriteBfrPtr[COMMAND_OFFSET]  = SEC_ERASE_64K_CMD;

		/*
		 * To be used only if 4B address sector erase cmd is
		 * supported by flash
		 */
		if (Flash_Config_Table[FCTIndex].FlashDeviceSize > SIXTEENMB) {
			WriteBfrPtr[ADDRESS_1_OFFSET] =
					(u8)((RealAddr & 0xFF000000) >> 24);
			WriteBfrPtr[ADDRESS_2_OFFSET] =
					(u8)((RealAddr & 0xFF0000) >> 16);
			WriteBfrPtr[ADDRESS_3_OFFSET] =
					(u8)((RealAddr & 0xFF00) >> 8);
			WriteBfrPtr[ADDRESS_4_OFFSET] =
					(u8)(RealAddr & 0xFF);
			FlashMsg[0].ByteCount = 5;
		} else {
			WriteBfrPtr[ADDRESS_1_OFFSET] =
					(u8)((RealAddr & 0xFF0000) >> 16);
			WriteBfrPtr[ADDRESS_2_OFFSET] =
					(u8)((RealAddr & 0xFF00) >> 8);
			WriteBfrPtr[ADDRESS_3_OFFSET] =
					(u8)(RealAddr & 0xFF);
			FlashMsg[0].ByteCount = 4;
		}

		FlashMsg[0].TxBfrPtr = WriteBfrPtr;
		FlashMsg[0].RxBfrPtr = NULL;
		FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
		FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

		Status = XQspiPsu_PolledTransfer(QspiPsuPtr, FlashMsg, 1);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}

		/*
		 * Wait for the erase command to be completed
		 */
		while (1) {
			ReadStatusCmd = StatusCmd;
			FlashMsg[0].TxBfrPtr = &ReadStatusCmd;
			FlashMsg[0].RxBfrPtr = NULL;
			FlashMsg[0].ByteCount = 1;
			FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
			FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

			FlashMsg[1].TxBfrPtr = NULL;
			FlashMsg[1].RxBfrPtr = FlashStatus;
			FlashMsg[1].ByteCount = 2;
			FlashMsg[1].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
			FlashMsg[1].Flags = XQSPIPSU_MSG_FLAG_RX;
			if (QspiPsuPtr->Config.ConnectionMode ==
					XQSPIPSU_CONNECTION_MODE_PARALLEL) {
				FlashMsg[1].Flags |= XQSPIPSU_MSG_FLAG_STRIPE;
			}

			Status = XQspiPsu_PolledTransfer(QspiPsuPtr,
					FlashMsg, 2);
			if (Status != XST_SUCCESS) {
				return XST_FAILURE;
			}

			if (QspiPsuPtr->Config.ConnectionMode ==
					XQSPIPSU_CONNECTION_MODE_PARALLEL) {
				if (FSRFlag) {
					FlashStatus[1] &= FlashStatus[0];
				} else {
					FlashStatus[1] |= FlashStatus[0];
				}
			}

			if (FSRFlag) {
				if ((FlashStatus[1] & 0x80) != 0) {
					break;
				}
			} else {
				if ((FlashStatus[1] & 0x01) == 0) {
					break;
				}
			}
		}
//		Address += SectSize;
		Address += SECTOR_SIZE_4K;
	}

	Status = EnterExit4BAddMode(QspiPsuPtr, EXIT_4B);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	return 0;
}


/*****************************************************************************/
/**
 *
 * This function performs read. DMA is the default setting.
 *
 * @param	QspiPsuPtr is a pointer to the QSPIPSU driver component to use.
 * @param	Address contains the address of the first sector which needs to
 *			be erased.
 * @param	ByteCount contains the total size to be erased.
 * @param	Command is the command used to read data from the flash.
 *		Supports normal, fast, dual and quad read commands.
 * @param	WriteBfrPtr is pointer to the write buffer which contains data to be
 *		transmitted
 * @param	ReadBfrPtr is pointer to the read buffer to which valid received data
 *		should be written
 *
 * @return	XST_SUCCESS if successful, else XST_FAILURE.
 *
 * @note	None.
 *
 ******************************************************************************/
int FlashRead(XQspiPsu *QspiPsuPtr, u32 Address, u32 ByteCount, u8 Command,
				u8 *WriteBfrPtr, u8 *ReadBfrPtr)
{
	u32 RealAddr;
	u32 DiscardByteCnt;
	u32 FlashMsgCnt;
	int Status;

	Status = EnterExit4BAddMode(QspiPsuPtr, ENTER_4B);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/* Check die boundary conditions if required for any flash */
	if (Flash_Config_Table[FCTIndex].NumDie > 1) {

		Status = MultiDieRead(QspiPsuPtr, Address, ByteCount, Command,
				      WriteBfrPtr, ReadBfrPtr);
		if (Status != XST_SUCCESS)
			return XST_FAILURE;
	} else {
		/* For Dual Stacked, split and read for boundary crossing */
		/*
		 * Translate address based on type of connection
		 * If stacked assert the slave select based on address
		 */
		RealAddr = GetRealAddr(QspiPsuPtr, Address);

		WriteBfrPtr[COMMAND_OFFSET]   = Command;
		if (Flash_Config_Table[FCTIndex].FlashDeviceSize > SIXTEENMB) {
			WriteBfrPtr[ADDRESS_1_OFFSET] =
					(u8)((RealAddr & 0xFF000000) >> 24);
			WriteBfrPtr[ADDRESS_2_OFFSET] =
					(u8)((RealAddr & 0xFF0000) >> 16);
			WriteBfrPtr[ADDRESS_3_OFFSET] =
					(u8)((RealAddr & 0xFF00) >> 8);
			WriteBfrPtr[ADDRESS_4_OFFSET] =
					(u8)(RealAddr & 0xFF);
			DiscardByteCnt = 5;
		} else {
			WriteBfrPtr[ADDRESS_1_OFFSET] =
					(u8)((RealAddr & 0xFF0000) >> 16);
			WriteBfrPtr[ADDRESS_2_OFFSET] =
					(u8)((RealAddr & 0xFF00) >> 8);
			WriteBfrPtr[ADDRESS_3_OFFSET] =
					(u8)(RealAddr & 0xFF);
			DiscardByteCnt = 4;
		}

		FlashMsg[0].TxBfrPtr = WriteBfrPtr;
		FlashMsg[0].RxBfrPtr = NULL;
		FlashMsg[0].ByteCount = DiscardByteCnt;
		FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
		FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

		FlashMsgCnt = 1;

		/* It is recommended to have a separate entry for dummy */
		if (Command == FAST_READ_CMD || Command == DUAL_READ_CMD ||
		    Command == QUAD_READ_CMD || Command == FAST_READ_CMD_4B ||
		    Command == DUAL_READ_CMD_4B ||
		    Command == QUAD_READ_CMD_4B) {
			/* Update Dummy cycles as per flash specs for QUAD IO */

			/*
			 * It is recommended that Bus width value during dummy
			 * phase should be same as data phase
			 */
			if (Command == FAST_READ_CMD ||
			    Command == FAST_READ_CMD_4B)
				FlashMsg[1].BusWidth =
						XQSPIPSU_SELECT_MODE_SPI;

			if (Command == DUAL_READ_CMD ||
			    Command == DUAL_READ_CMD_4B)
				FlashMsg[1].BusWidth =
						XQSPIPSU_SELECT_MODE_DUALSPI;

			if (Command == QUAD_READ_CMD ||
			    Command == QUAD_READ_CMD_4B)
				FlashMsg[1].BusWidth =
						XQSPIPSU_SELECT_MODE_QUADSPI;

			FlashMsg[1].TxBfrPtr = NULL;
			FlashMsg[1].RxBfrPtr = NULL;
			FlashMsg[1].ByteCount = DUMMY_CLOCKS;
			FlashMsg[1].Flags = 0;

			FlashMsgCnt++;
		}

		if (Command == FAST_READ_CMD ||
		    Command == FAST_READ_CMD_4B)
			FlashMsg[FlashMsgCnt].BusWidth =
					XQSPIPSU_SELECT_MODE_SPI;

		if (Command == DUAL_READ_CMD ||
		    Command == DUAL_READ_CMD_4B)
			FlashMsg[FlashMsgCnt].BusWidth =
					XQSPIPSU_SELECT_MODE_DUALSPI;

		if (Command == QUAD_READ_CMD ||
		    Command == QUAD_READ_CMD_4B)
			FlashMsg[FlashMsgCnt].BusWidth =
					XQSPIPSU_SELECT_MODE_QUADSPI;

		FlashMsg[FlashMsgCnt].TxBfrPtr = NULL;
		FlashMsg[FlashMsgCnt].RxBfrPtr = ReadBfrPtr;
		FlashMsg[FlashMsgCnt].ByteCount = ByteCount;
		FlashMsg[FlashMsgCnt].Flags = XQSPIPSU_MSG_FLAG_RX;

		if (QspiPsuPtr->Config.ConnectionMode ==
				XQSPIPSU_CONNECTION_MODE_PARALLEL)
			FlashMsg[FlashMsgCnt].Flags |=
					XQSPIPSU_MSG_FLAG_STRIPE;

		Status = XQspiPsu_PolledTransfer(QspiPsuPtr, FlashMsg,
						 FlashMsgCnt + 1);
		if (Status != XST_SUCCESS)
			return XST_FAILURE;
	}

	Status = EnterExit4BAddMode(QspiPsuPtr, EXIT_4B);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

/*****************************************************************************/
/**
 *
 * This functions performs a read operation for multi die flash devices.
 *
 * @param	QspiPsuPtr is a pointer to the QSPIPSU driver component to use.
 * @param	Address contains the address of the first sector which needs to
 *			be erased.
 * @param	ByteCount contains the total size to be erased.
 * @param	Command is the command used to read data from the flash.
 *			Supports normal, fast, dual and quad read commands.
 * @param	WriteBfrPtr is pointer to the write buffer which contains data to be
 *			transmitted
 * @param	ReadBfrPtr is pointer to the read buffer to which valid received data
 *			should be written.
 *
 * @return	XST_SUCCESS if successful, else XST_FAILURE.
 *
 * @note	None.
 *
 ******************************************************************************/
int MultiDieRead(XQspiPsu *QspiPsuPtr, u32 Address, u32 ByteCount, u8 Command,
		 u8 *WriteBfrPtr, u8 *ReadBfrPtr)
{
	u32 RealAddr;
	u32 DiscardByteCnt;
	u32 FlashMsgCnt;
	int Status;
	u32 cur_bank = 0;
	u32 nxt_bank = 0;
	u32 bank_size;
	u32 remain_len = ByteCount;
	u32 data_len;
	u32 transfer_len;
	u8 *ReadBuffer = ReadBfrPtr;

	Status = EnterExit4BAddMode(QspiPsuPtr, ENTER_4B);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Some flash devices like N25Q512 have multiple dies
	 * in it. Read operation in these devices is bounded
	 * by its die segment. In a continuous read, across
	 * multiple dies, when the last byte of the selected
	 * die segment is read, the next byte read is the
	 * first byte of the same die segment. This is Die
	 * cross over issue. So to handle this issue, split
	 * a read transaction, that spans across multiple
	 * banks, into one read per bank. Bank size is 16MB
	 * for single and dual stacked mode and 32MB for dual
	 * parallel mode.
	 */
	if (QspiPsuPtr->Config.ConnectionMode ==
			XQSPIPSU_CONNECTION_MODE_PARALLEL)

		bank_size = SIXTEENMB << 1;

	else if (QspiPsuPtr->Config.ConnectionMode ==
			XQSPIPSU_CONNECTION_MODE_SINGLE)

		bank_size = SIXTEENMB;

	while (remain_len) {
		cur_bank = Address / bank_size;
		nxt_bank = (Address + remain_len) / bank_size;

		if (cur_bank != nxt_bank) {
			transfer_len = (bank_size * (cur_bank  + 1)) - Address;
			if (remain_len < transfer_len)
				data_len = remain_len;
			else
				data_len = transfer_len;
		} else {
			data_len = remain_len;
		}
		/*
		 * Translate address based on type of connection
		 * If stacked assert the slave select based on address
		 */
		RealAddr = GetRealAddr(QspiPsuPtr, Address);

		WriteBfrPtr[COMMAND_OFFSET]   = Command;
		if (Flash_Config_Table[FCTIndex].FlashDeviceSize > SIXTEENMB) {
			WriteBfrPtr[ADDRESS_1_OFFSET] =
					(u8)((RealAddr & 0xFF000000) >> 24);
			WriteBfrPtr[ADDRESS_2_OFFSET] =
					(u8)((RealAddr & 0xFF0000) >> 16);
			WriteBfrPtr[ADDRESS_3_OFFSET] =
					(u8)((RealAddr & 0xFF00) >> 8);
			WriteBfrPtr[ADDRESS_4_OFFSET] =
					(u8)(RealAddr & 0xFF);
			DiscardByteCnt = 5;
		} else {
			WriteBfrPtr[ADDRESS_1_OFFSET] =
					(u8)((RealAddr & 0xFF0000) >> 16);
			WriteBfrPtr[ADDRESS_2_OFFSET] =
					(u8)((RealAddr & 0xFF00) >> 8);
			WriteBfrPtr[ADDRESS_3_OFFSET] =
					(u8)(RealAddr & 0xFF);
			DiscardByteCnt = 4;
		}

		FlashMsg[0].TxBfrPtr = WriteBfrPtr;
		FlashMsg[0].RxBfrPtr = NULL;
		FlashMsg[0].ByteCount = DiscardByteCnt;
		FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
		FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

		FlashMsgCnt = 1;

		/* It is recommended to have a separate entry for dummy */
		if (Command == FAST_READ_CMD || Command == DUAL_READ_CMD ||
		    Command == QUAD_READ_CMD || Command == FAST_READ_CMD_4B ||
		    Command == DUAL_READ_CMD_4B ||
		    Command == QUAD_READ_CMD_4B) {
			/* Update Dummy cycles as per flash specs for QUAD IO */

			/*
			 * It is recommended that Bus width value during dummy
			 * phase should be same as data phase
			 */
			if (Command == FAST_READ_CMD ||
			    Command == FAST_READ_CMD_4B)
				FlashMsg[1].BusWidth =
						XQSPIPSU_SELECT_MODE_SPI;

			if (Command == DUAL_READ_CMD ||
			    Command == DUAL_READ_CMD_4B)
				FlashMsg[1].BusWidth =
						XQSPIPSU_SELECT_MODE_DUALSPI;

			if (Command == QUAD_READ_CMD ||
			    Command == QUAD_READ_CMD_4B)
				FlashMsg[1].BusWidth =
						XQSPIPSU_SELECT_MODE_QUADSPI;

			FlashMsg[1].TxBfrPtr = NULL;
			FlashMsg[1].RxBfrPtr = NULL;
			FlashMsg[1].ByteCount = DUMMY_CLOCKS;
			FlashMsg[1].Flags = 0;

			FlashMsgCnt++;
		}

		if (Command == FAST_READ_CMD ||
		    Command == FAST_READ_CMD_4B)
			FlashMsg[FlashMsgCnt].BusWidth =
					XQSPIPSU_SELECT_MODE_SPI;

		if (Command == DUAL_READ_CMD ||
		    Command == DUAL_READ_CMD_4B)
			FlashMsg[FlashMsgCnt].BusWidth =
					XQSPIPSU_SELECT_MODE_DUALSPI;

		if (Command == QUAD_READ_CMD ||
		    Command == QUAD_READ_CMD_4B)
			FlashMsg[FlashMsgCnt].BusWidth =
					XQSPIPSU_SELECT_MODE_QUADSPI;

		FlashMsg[FlashMsgCnt].TxBfrPtr = NULL;
		FlashMsg[FlashMsgCnt].RxBfrPtr = ReadBuffer;
		FlashMsg[FlashMsgCnt].ByteCount = data_len;
		FlashMsg[FlashMsgCnt].Flags = XQSPIPSU_MSG_FLAG_RX;

		if (QspiPsuPtr->Config.ConnectionMode ==
				XQSPIPSU_CONNECTION_MODE_PARALLEL)
			FlashMsg[FlashMsgCnt].Flags |=
					XQSPIPSU_MSG_FLAG_STRIPE;

		Status = XQspiPsu_PolledTransfer(QspiPsuPtr, FlashMsg,
						 FlashMsgCnt + 1);
		if (Status != XST_SUCCESS)
			return XST_FAILURE;

		ReadBuffer += data_len;
		Address += data_len;
		remain_len -= data_len;
	}

	Status = EnterExit4BAddMode(QspiPsuPtr, EXIT_4B);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}


/*****************************************************************************/
/**
 *
 * This API can be used to write to a flash register.
 *
 * @param	QspiPsuPtr is a pointer to the QSPIPSU driver component to use.
 * @param	ByteCount is the number of bytes to write.
 * @param	Command is specific register write command.
 * @param	WriteBfrPtr is the pointer to value to be written.
 * @param	WrEn is a flag to mention if WREN has to be sent before write.
 *
 * @return	XST_SUCCESS if successful, else XST_FAILURE.
 *
 * @note	This API can only be used for one flash at a time.
 *
 ******************************************************************************/
int FlashRegisterWrite(XQspiPsu *QspiPsuPtr, u32 ByteCount, u8 Command,
					u8 *WriteBfrPtr, u8 WrEn)
{
	u8 WriteEnableCmd;
	u8 ReadStatusCmd;
	u8 FlashStatus[2];
	int Status;

	if (WrEn) {
		WriteEnableCmd = WRITE_ENABLE_CMD;

		/*
		 * Send the write enable command to the Flash so that it can be
		 * written to, this needs to be sent as a separate
		 * transfer before the write
		 */
		FlashMsg[0].TxBfrPtr = &WriteEnableCmd;
		FlashMsg[0].RxBfrPtr = NULL;
		FlashMsg[0].ByteCount = 1;
		FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
		FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

		Status = XQspiPsu_PolledTransfer(QspiPsuPtr, FlashMsg, 1);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}
	}

	WriteBfrPtr[COMMAND_OFFSET]   = Command;
	/*
	 * Value(s) is(are) expected to be written to the
	 * write buffer by calling API ByteCount is the count
	 * of the value(s) excluding the command.
	 */

	FlashMsg[0].TxBfrPtr = WriteBfrPtr;
	FlashMsg[0].RxBfrPtr = NULL;
	FlashMsg[0].ByteCount = ByteCount + 1;
	FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
	FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

	Status = XQspiPsu_PolledTransfer(QspiPsuPtr, FlashMsg, 1);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Wait for the register write command to the Flash to be completed.
	 */
	while (1) {
		ReadStatusCmd = StatusCmd;
		FlashMsg[0].TxBfrPtr = &ReadStatusCmd;
		FlashMsg[0].RxBfrPtr = NULL;
		FlashMsg[0].ByteCount = 1;
		FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
		FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

		FlashMsg[1].TxBfrPtr = NULL;
		FlashMsg[1].RxBfrPtr = FlashStatus;
		FlashMsg[1].ByteCount = 2;
		FlashMsg[1].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
		FlashMsg[1].Flags = XQSPIPSU_MSG_FLAG_RX;

		Status = XQspiPsu_PolledTransfer(QspiPsuPtr, FlashMsg, 2);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}

		if (FSRFlag) {
			if ((FlashStatus[1] & 0x80) != 0) {
				break;
			}
		} else {
			if ((FlashStatus[1] & 0x01) == 0) {
				break;
			}
		}
	}

	Status = EnterExit4BAddMode(QspiPsuPtr, EXIT_4B);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	return 0;
}

/*****************************************************************************/
/**
 *
 * This API can be used to write to a flash register.
 *
 * @param	QspiPsuPtr is a pointer to the QSPIPSU driver component to use.
 * @param	ByteCount is the number of bytes to write.
 * @param	Command is specific register write command.
 * @param	ReadBfrPtr is the pointer to value to be written.
 *
 * @return	XST_SUCCESS if successful, else XST_FAILURE.
 *
 * @note	This API can only be used for one flash at a time.
 *
 ******************************************************************************/
int FlashRegisterRead(XQspiPsu *QspiPsuPtr, u32 ByteCount,
		u8 Command, u8 *ReadBfrPtr)
{
	u8 WriteCmd;
	int Status;

	WriteCmd = Command;
	FlashMsg[0].TxBfrPtr = &WriteCmd;
	FlashMsg[0].RxBfrPtr = NULL;
	FlashMsg[0].ByteCount = 1;
	FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
	FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

	FlashMsg[1].TxBfrPtr = NULL;
	FlashMsg[1].RxBfrPtr = ReadBfrPtr;
	//FlashMsg[1].ByteCount = ByteCount;
	/* This is for DMA reasons; to be changed shortly */
	FlashMsg[1].ByteCount = 4;
	FlashMsg[1].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
	FlashMsg[1].Flags = XQSPIPSU_MSG_FLAG_RX;

	Status = XQspiPsu_PolledTransfer(QspiPsuPtr, FlashMsg, 2);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	return 0;
}

/*****************************************************************************/
/**
 *
 * This functions performs a bulk erase operation when the
 * flash device has a single die. Works for both Spansion and Micron
 *
 * @param	QspiPsuPtr is a pointer to the QSPIPSU driver component to use.
 * @param	WriteBfrPtr is the pointer to command+address to be sent
 *
 * @return	XST_SUCCESS if successful, else XST_FAILURE.
 *
 * @note	None.
 *
 ******************************************************************************/
int BulkErase(XQspiPsu *QspiPsuPtr, u8 *WriteBfrPtr)
{
	u8 WriteEnableCmd;
	u8 ReadStatusCmd;
	u8 FlashStatus[2];
	int Status;

	WriteEnableCmd = WRITE_ENABLE_CMD;

	Status = EnterExit4BAddMode(QspiPsuPtr, ENTER_4B);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Send the write enable command to the Flash so that it can be
	 * written to, this needs to be sent as a separate transfer before
	 * the write
	 */
	FlashMsg[0].TxBfrPtr = &WriteEnableCmd;
	FlashMsg[0].RxBfrPtr = NULL;
	FlashMsg[0].ByteCount = 1;
	FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
	FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

	Status = XQspiPsu_PolledTransfer(QspiPsuPtr, FlashMsg, 1);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	WriteBfrPtr[COMMAND_OFFSET]   = BULK_ERASE_CMD;
	FlashMsg[0].TxBfrPtr = WriteBfrPtr;
	FlashMsg[0].RxBfrPtr = NULL;
	FlashMsg[0].ByteCount = 1;
	FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
	FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

	Status = XQspiPsu_PolledTransfer(QspiPsuPtr, FlashMsg, 1);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Wait for the write command to the Flash to be completed, it takes
	 * some time for the data to be written
	 */
	while (1) {
		ReadStatusCmd = StatusCmd;
		FlashMsg[0].TxBfrPtr = &ReadStatusCmd;
		FlashMsg[0].RxBfrPtr = NULL;
		FlashMsg[0].ByteCount = 1;
		FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
		FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

		FlashMsg[1].TxBfrPtr = NULL;
		FlashMsg[1].RxBfrPtr = FlashStatus;
		FlashMsg[1].ByteCount = 2;
		FlashMsg[1].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
		FlashMsg[1].Flags = XQSPIPSU_MSG_FLAG_RX;
		if (QspiPsuPtr->Config.ConnectionMode ==
				XQSPIPSU_CONNECTION_MODE_PARALLEL) {
			FlashMsg[1].Flags |= XQSPIPSU_MSG_FLAG_STRIPE;
		}


		Status = XQspiPsu_PolledTransfer(QspiPsuPtr, FlashMsg, 2);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}
		if (QspiPsuPtr->Config.ConnectionMode ==
				XQSPIPSU_CONNECTION_MODE_PARALLEL) {
			if (FSRFlag) {
				FlashStatus[1] &= FlashStatus[0];
			} else {
				FlashStatus[1] |= FlashStatus[0];
			}
		}

		if (FSRFlag) {
			if ((FlashStatus[1] & 0x80) != 0) {
				break;
			}
		} else {
			if ((FlashStatus[1] & 0x01) == 0) {
				break;
			}
		}
	}

	Status = EnterExit4BAddMode(QspiPsuPtr, EXIT_4B);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	return 0;
}

/*****************************************************************************/
/**
 *
 * This functions performs a die erase operation on all the die in
 * the flash device. This function uses the die erase command for
 * Micron 512Mbit and 1Gbit
 *
 * @param	QspiPsuPtr is a pointer to the QSPIPSU driver component to use.
 * @param	WriteBfrPtr is the pointer to command+address to be sent
 *
 * @return	XST_SUCCESS if successful, else XST_FAILURE.
 *
 * @note	None.
 *
 ******************************************************************************/
int DieErase(XQspiPsu *QspiPsuPtr, u8 *WriteBfrPtr)
{
	u8 WriteEnableCmd;
	u8 DieCnt;
	u8 ReadStatusCmd;
	u8 FlashStatus[2];
	int Status;
	u32 DieSize = 0;
	u32 Address;
	u32 RealAddr;
	u32 SectSize = 0;
	u32 NumSect = 0;

	WriteEnableCmd = WRITE_ENABLE_CMD;

	if (QspiPsuPtr->Config.ConnectionMode == XQSPIPSU_CONNECTION_MODE_PARALLEL) {
		SectSize = (Flash_Config_Table[FCTIndex]).SectSize * 2;
	} else if (QspiPsuPtr->Config.ConnectionMode == XQSPIPSU_CONNECTION_MODE_STACKED) {
		NumSect = (Flash_Config_Table[FCTIndex]).NumSect * 2;
	} else {
		SectSize = (Flash_Config_Table[FCTIndex]).SectSize;
		NumSect = (Flash_Config_Table[FCTIndex]).NumSect;
	}
	DieSize = (NumSect * SectSize) / Flash_Config_Table[FCTIndex].NumDie;

	Status = EnterExit4BAddMode(QspiPsuPtr, ENTER_4B);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	for (DieCnt = 0;
		DieCnt < Flash_Config_Table[FCTIndex].NumDie;
		DieCnt++) {
		/*
		 * Send the write enable command to the Flash so that it can be
		 * written to, this needs to be sent as a separate transfer
		 * before the write
		 */
		FlashMsg[0].TxBfrPtr = &WriteEnableCmd;
		FlashMsg[0].RxBfrPtr = NULL;
		FlashMsg[0].ByteCount = 1;
		FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
		FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

		Status = XQspiPsu_PolledTransfer(QspiPsuPtr, FlashMsg, 1);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}

		WriteBfrPtr[COMMAND_OFFSET]   = DIE_ERASE_CMD;
		Address = DieSize * DieCnt;
		RealAddr = GetRealAddr(QspiPsuPtr, Address);
		/*
		 * To be used only if 4B address sector erase cmd is
		 * supported by flash
		 */
		if (Flash_Config_Table[FCTIndex].FlashDeviceSize > SIXTEENMB) {
			WriteBfrPtr[ADDRESS_1_OFFSET] =
					(u8)((RealAddr & 0xFF000000) >> 24);
			WriteBfrPtr[ADDRESS_2_OFFSET] =
					(u8)((RealAddr & 0xFF0000) >> 16);
			WriteBfrPtr[ADDRESS_3_OFFSET] =
					(u8)((RealAddr & 0xFF00) >> 8);
			WriteBfrPtr[ADDRESS_4_OFFSET] =
					(u8)(RealAddr & 0xFF);
			FlashMsg[0].ByteCount = 5;
		} else {
			WriteBfrPtr[ADDRESS_1_OFFSET] =
					(u8)((RealAddr & 0xFF0000) >> 16);
			WriteBfrPtr[ADDRESS_2_OFFSET] =
					(u8)((RealAddr & 0xFF00) >> 8);
			WriteBfrPtr[ADDRESS_3_OFFSET] =
					(u8)(RealAddr & 0xFF);
			FlashMsg[0].ByteCount = 4;
		}

		FlashMsg[0].TxBfrPtr = WriteBfrPtr;
		FlashMsg[0].RxBfrPtr = NULL;
		FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
		FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

		Status = XQspiPsu_PolledTransfer(QspiPsuPtr, FlashMsg, 1);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}

		/*
		 * Wait for the write command to the Flash to be completed,
		 * it takes some time for the data to be written
		 */
		while (1) {
			ReadStatusCmd = StatusCmd;
			FlashMsg[0].TxBfrPtr = &ReadStatusCmd;
			FlashMsg[0].RxBfrPtr = NULL;
			FlashMsg[0].ByteCount = 1;
			FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
			FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

			FlashMsg[1].TxBfrPtr = NULL;
			FlashMsg[1].RxBfrPtr = FlashStatus;
			FlashMsg[1].ByteCount = 2;
			FlashMsg[1].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
			FlashMsg[1].Flags = XQSPIPSU_MSG_FLAG_RX;
			if (QspiPsuPtr->Config.ConnectionMode ==
					XQSPIPSU_CONNECTION_MODE_PARALLEL) {
				FlashMsg[1].Flags |= XQSPIPSU_MSG_FLAG_STRIPE;
			}

			Status = XQspiPsu_PolledTransfer(QspiPsuPtr,
					FlashMsg, 2);
			if (Status != XST_SUCCESS) {
				return XST_FAILURE;
			}
			if (QspiPsuPtr->Config.ConnectionMode ==
					XQSPIPSU_CONNECTION_MODE_PARALLEL) {
				if (FSRFlag) {
					FlashStatus[1] &= FlashStatus[0];
				} else {
					FlashStatus[1] |= FlashStatus[0];
				}
			}

			if (FSRFlag) {
				if ((FlashStatus[1] & 0x80) != 0) {
					break;
				}
			} else {
				if ((FlashStatus[1] & 0x01) == 0) {
					break;
				}
			}
		}
	}

	Status = EnterExit4BAddMode(QspiPsuPtr, EXIT_4B);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	return 0;
}

/*****************************************************************************/
/**
 *
 * This functions translates the address based on the type of interconnection.
 * In case of stacked, this function asserts the corresponding slave select.
 *
 * @param	QspiPsuPtr is a pointer to the QSPIPSU driver component to use.
 * @param	Address which is to be accessed (for erase, write or read)
 *
 * @return	RealAddr is the translated address - for single it is unchanged;
 *		for stacked, the lower flash size is subtracted;
 *		for parallel the address is divided by 2.
 *
 * @note	In addition to get the actual address to work on flash this
 *		function also selects the CS and BUS based on the configuration
 *		detected.
 *
 ******************************************************************************/
u32 GetRealAddr(XQspiPsu *QspiPsuPtr, u32 Address)
{
	u32 RealAddr;

	switch (QspiPsuPtr->Config.ConnectionMode) {
	case XQSPIPSU_CONNECTION_MODE_SINGLE:
		XQspiPsu_SelectFlash(QspiPsuPtr,
				XQSPIPSU_SELECT_FLASH_CS_LOWER,
				XQSPIPSU_SELECT_FLASH_BUS_LOWER);
		RealAddr = Address;
		break;
	case XQSPIPSU_CONNECTION_MODE_STACKED:
		/* Select lower or upper Flash based on sector address */
		if (Address & Flash_Config_Table[FCTIndex].FlashDeviceSize) {

			XQspiPsu_SelectFlash(QspiPsuPtr,
					XQSPIPSU_SELECT_FLASH_CS_UPPER,
					XQSPIPSU_SELECT_FLASH_BUS_LOWER);
			/*
			 * Subtract first flash size when accessing second flash
			 */
			RealAddr = Address &
				(~Flash_Config_Table[FCTIndex].FlashDeviceSize);
		} else {
			/*
			 * Set selection to L_PAGE
			 */
			XQspiPsu_SelectFlash(QspiPsuPtr,
					XQSPIPSU_SELECT_FLASH_CS_LOWER,
					XQSPIPSU_SELECT_FLASH_BUS_LOWER);

			RealAddr = Address;

		}
		break;
	case XQSPIPSU_CONNECTION_MODE_PARALLEL:
		/*
		 * The effective address in each flash is the actual
		 * address / 2
		 */
		XQspiPsu_SelectFlash(QspiPsuPtr,
				XQSPIPSU_SELECT_FLASH_CS_BOTH,
				XQSPIPSU_SELECT_FLASH_BUS_BOTH);
		RealAddr = Address / 2;
		break;
	default:
		/* RealAddr wont be assigned in this case; */
	break;

	}

	return(RealAddr);
}

/*****************************************************************************/
/**
 * @brief
 * This API enters the flash device into 4 bytes addressing mode.
 * As per the Micron and ISSI spec, before issuing the command to enter
 * into 4 byte addr mode, a write enable command is issued.
 * For Macronix and Winbond flash parts write
 * enable is not required.
 *
 * @param	QspiPsuPtr is a pointer to the QSPIPSU driver component to use.
 * @param	Enable is a either 1 or 0 if 1 then enters 4 byte if 0 exits.
 *
 * @return
 *		- XST_SUCCESS if successful.
 *		- XST_FAILURE if it fails.
 *
 *
 ******************************************************************************/
int FlashEnterExit4BAddMode(XQspiPsu *QspiPsuPtr, unsigned int Enable)
{
	int Status;
	u8 WriteEnableCmd;
	u8 Cmd;
	u8 WriteDisableCmd;
	u8 ReadStatusCmd;
	u8 WriteBuffer[2] = {0};
	u8 FlashStatus[2] = {0};

	if (Enable) {
		Cmd = ENTER_4B_ADDR_MODE;
	} else {
		if (FlashMake == ISSI_ID_BYTE0)
			Cmd = EXIT_4B_ADDR_MODE_ISSI;
		else
			Cmd = EXIT_4B_ADDR_MODE;
	}

	switch (FlashMake) {
	case ISSI_ID_BYTE0:
	case MICRON_ID_BYTE0:
		WriteEnableCmd = WRITE_ENABLE_CMD;
		GetRealAddr(QspiPsuPtr, TEST_ADDRESS);
		/*
		 * Send the write enable command to the
		 * Flash so that it can be written to, this
		 * needs to be sent as a separate transfer before
		 * the write
		 */
		FlashMsg[0].TxBfrPtr = &WriteEnableCmd;
		FlashMsg[0].RxBfrPtr = NULL;
		FlashMsg[0].ByteCount = 1;
		FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
		FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

		Status = XQspiPsu_PolledTransfer(QspiPsuPtr, FlashMsg, 1);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}
		break;

	case SPANSION_ID_BYTE0:

		if (Enable) {
			WriteBuffer[0] = BANK_REG_WR;
			WriteBuffer[1] = 1 << 7;
		} else {
			WriteBuffer[0] = BANK_REG_WR;
			WriteBuffer[1] = 0 << 7;
		}

		FlashMsg[0].TxBfrPtr = WriteBuffer;
		FlashMsg[0].RxBfrPtr = NULL;
		FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
		FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;
		FlashMsg[0].ByteCount = 2;

		Status = XQspiPsu_PolledTransfer(QspiPsuPtr, FlashMsg, 1);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}

		return Status;

	default:
		/*
		 * For Macronix and Winbond flash parts
		 * Write enable command is not required.
		 */
		break;
	}

	GetRealAddr(QspiPsuPtr, TEST_ADDRESS);

	FlashMsg[0].TxBfrPtr = &Cmd;
	FlashMsg[0].RxBfrPtr = NULL;
	FlashMsg[0].ByteCount = 1;
	FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
	FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

	Status = XQspiPsu_PolledTransfer(QspiPsuPtr, FlashMsg, 1);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	while (1) {
		ReadStatusCmd = StatusCmd;

		FlashMsg[0].TxBfrPtr = &ReadStatusCmd;
		FlashMsg[0].RxBfrPtr = NULL;
		FlashMsg[0].ByteCount = 1;
		FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
		FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

		FlashMsg[1].TxBfrPtr = NULL;
		FlashMsg[1].RxBfrPtr = FlashStatus;
		FlashMsg[1].ByteCount = 2;
		FlashMsg[1].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
		FlashMsg[1].Flags = XQSPIPSU_MSG_FLAG_RX;

		if (QspiPsuPtr->Config.ConnectionMode ==
				XQSPIPSU_CONNECTION_MODE_PARALLEL) {
			FlashMsg[1].Flags |= XQSPIPSU_MSG_FLAG_STRIPE;
		}

		Status = XQspiPsu_PolledTransfer(QspiPsuPtr, FlashMsg, 2);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}

		if (QspiPsuPtr->Config.ConnectionMode ==
				XQSPIPSU_CONNECTION_MODE_PARALLEL) {
			if (FSRFlag) {
				FlashStatus[1] &= FlashStatus[0];
			} else {
				FlashStatus[1] |= FlashStatus[0];
			}
		}

		if (FSRFlag) {
			if ((FlashStatus[1] & 0x80) != 0) {
				break;
			}
		} else {
			if ((FlashStatus[1] & 0x01) == 0) {
				break;
			}
		}
	}

	switch (FlashMake) {
	case ISSI_ID_BYTE0:
	case MICRON_ID_BYTE0:
		WriteDisableCmd = WRITE_DISABLE_CMD;
		GetRealAddr(QspiPsuPtr, TEST_ADDRESS);
		/*
		 * Send the write enable command to the
		 * Flash so that it can be written to,
		 * this needs to be sent as a separate
		 * transfer before
		 * the write
		 */
		FlashMsg[0].TxBfrPtr = &WriteDisableCmd;
		FlashMsg[0].RxBfrPtr = NULL;
		FlashMsg[0].ByteCount = 1;
		FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
		FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

		Status = XQspiPsu_PolledTransfer(QspiPsuPtr, FlashMsg, 1);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}
		break;

	default:
		/*
		 * For Macronix and Winbond flash parts
		 * Write disable command is not required.
		 */
		break;
	}

	return Status;
}

/*****************************************************************************/
/**
 * @brief
 * This API enables Quad mode for the flash parts which require to enable quad
 * mode before using Quad commands.
 * For S25FL-L series flash parts this is required as the default configuration
 * is x1/x2 mode.
 *
 * @param	QspiPsuPtr is a pointer to the QSPIPSU driver component to use.
 *
 * @return
 *		- XST_SUCCESS if successful.
 *		- XST_FAILURE if it fails.
 *
 *
 ******************************************************************************/
int FlashEnableQuadMode(XQspiPsu *QspiPsuPtr)
{
	int Status;
	u8 WriteEnableCmd;
	u8 ReadStatusCmd;
	u8 FlashStatus[2];
	u8 StatusRegVal;
	u8 WriteBuffer[3] = {0};

	switch (FlashMake) {
	case SPANSION_ID_BYTE0:
		if (FCTIndex <= 2) {
			TxBfrPtr = READ_CONFIG_CMD;
			FlashMsg[0].TxBfrPtr = &TxBfrPtr;
			FlashMsg[0].RxBfrPtr = NULL;
			FlashMsg[0].ByteCount = 1;
			FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
			FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

			FlashMsg[1].TxBfrPtr = NULL;
			FlashMsg[1].RxBfrPtr = &WriteBuffer[2];
			FlashMsg[1].ByteCount = 1;
			FlashMsg[1].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
			FlashMsg[1].Flags = XQSPIPSU_MSG_FLAG_RX;

			Status = XQspiPsu_PolledTransfer(QspiPsuPtr,
					FlashMsg, 2);
			if (Status != XST_SUCCESS) {
				return XST_FAILURE;
			}

			WriteEnableCmd = VOLATILE_WRITE_ENABLE_CMD;
			/*
			 * Send the write enable command to the
			 * Flash so that it can be written to, this
			 * needs to be sent as a separate transfer before
			 * the write
			 */
			FlashMsg[0].TxBfrPtr = &WriteEnableCmd;
			FlashMsg[0].RxBfrPtr = NULL;
			FlashMsg[0].ByteCount = 1;
			FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
			FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

			Status = XQspiPsu_PolledTransfer(QspiPsuPtr,
					FlashMsg, 1);
			if (Status != XST_SUCCESS) {
				return XST_FAILURE;
			}

			GetRealAddr(QspiPsuPtr, TEST_ADDRESS);

			WriteBuffer[0] = WRITE_CONFIG_CMD;
			WriteBuffer[1] |= 0;
			WriteBuffer[2] |= 1 << 1;

			FlashMsg[0].TxBfrPtr = WriteBuffer;
			FlashMsg[0].RxBfrPtr = NULL;
			FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
			FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;
			FlashMsg[0].ByteCount = 3;

			Status = XQspiPsu_PolledTransfer(QspiPsuPtr,
					FlashMsg, 1);
			if (Status != XST_SUCCESS) {
				return XST_FAILURE;
			}

			TxBfrPtr = READ_CONFIG_CMD;
			FlashMsg[0].TxBfrPtr = &TxBfrPtr;
			FlashMsg[0].RxBfrPtr = NULL;
			FlashMsg[0].ByteCount = 1;
			FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
			FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

			FlashMsg[1].TxBfrPtr = NULL;
			FlashMsg[1].RxBfrPtr = ReadBfrPtr;
			FlashMsg[1].ByteCount = 1;
			FlashMsg[1].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
			FlashMsg[1].Flags = XQSPIPSU_MSG_FLAG_RX;

			Status = XQspiPsu_PolledTransfer(QspiPsuPtr,
					FlashMsg, 2);
			if (Status != XST_SUCCESS) {
				return XST_FAILURE;
			}

			if (ReadBfrPtr[0] & 0x02) {
				Status = XST_SUCCESS;
			} else {
				Status = XST_FAILURE;
			}
		}
		break;
	case ISSI_ID_BYTE0:
		/*
		 * Read Status register
		 */
		ReadStatusCmd = READ_STATUS_CMD;
		FlashMsg[0].TxBfrPtr = &ReadStatusCmd;
		FlashMsg[0].RxBfrPtr = NULL;
		FlashMsg[0].ByteCount = 1;
		FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
		FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;
		FlashMsg[1].TxBfrPtr = NULL;
		FlashMsg[1].RxBfrPtr = FlashStatus;
		FlashMsg[1].ByteCount = 2;
		FlashMsg[1].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
		FlashMsg[1].Flags = XQSPIPSU_MSG_FLAG_RX;
		if (QspiPsuPtr->Config.ConnectionMode ==
						XQSPIPSU_CONNECTION_MODE_PARALLEL) {
				FlashMsg[1].Flags |= XQSPIPSU_MSG_FLAG_STRIPE;
		}
		Status = XQspiPsu_PolledTransfer(QspiPsuPtr, FlashMsg, 2);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}
		if (QspiPsuPtr->Config.ConnectionMode ==
					XQSPIPSU_CONNECTION_MODE_PARALLEL) {
			if (FSRFlag) {
				FlashStatus[1] &= FlashStatus[0];
			}else {
				FlashStatus[1] |= FlashStatus[0];
			}
		}
		/*
		 * Set Quad Enable Bit in the buffer
		 */
		StatusRegVal = FlashStatus[1];
		StatusRegVal |= 0x1 << QUAD_MODE_ENABLE_BIT;

		/*
		 * Write Enable
		 */
		WriteEnableCmd = WRITE_ENABLE_CMD;
		FlashMsg[0].TxBfrPtr = &WriteEnableCmd;
		FlashMsg[0].RxBfrPtr = NULL;
		FlashMsg[0].ByteCount = 1;
		FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
		FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;
		Status = XQspiPsu_PolledTransfer(QspiPsuPtr, FlashMsg, 1);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}
		/*
		 * Write Status register
		 */
		WriteBuffer[COMMAND_OFFSET] = WRITE_STATUS_CMD;
		FlashMsg[0].TxBfrPtr = WriteBuffer;
		FlashMsg[0].RxBfrPtr = NULL;
		FlashMsg[0].ByteCount = 1;
		FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
		FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

		FlashMsg[1].TxBfrPtr = &StatusRegVal;
		FlashMsg[1].RxBfrPtr = NULL;
		FlashMsg[1].ByteCount = 1;
		FlashMsg[1].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
		FlashMsg[1].Flags = XQSPIPSU_MSG_FLAG_TX;
		if (QspiPsuPtr->Config.ConnectionMode ==
				XQSPIPSU_CONNECTION_MODE_PARALLEL) {
			FlashMsg[1].Flags |= XQSPIPSU_MSG_FLAG_STRIPE;
		}
		Status = XQspiPsu_PolledTransfer(QspiPsuPtr, FlashMsg, 2);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}
		/*
		 * Write Disable
		 */
		WriteEnableCmd = WRITE_DISABLE_CMD;
		FlashMsg[0].TxBfrPtr = &WriteEnableCmd;
		FlashMsg[0].RxBfrPtr = NULL;
		FlashMsg[0].ByteCount = 1;
		FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
		FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;
		Status = XQspiPsu_PolledTransfer(QspiPsuPtr, FlashMsg, 2);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}

	default:
		/*
		 * Currently only S25FL-L series requires the
		 * Quad enable bit to be set to 1.
		 */
		Status = XST_SUCCESS;
		break;
	}

	return Status;
}
