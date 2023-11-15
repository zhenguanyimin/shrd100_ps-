
#ifndef FLASH_COMMUNICATION_H
#define FLASH_COMMUNICATION_H

/***************************** Include Files *********************************/
#include <stdio.h>
#include "xparameters.h"	/* SDK generated parameters */
#include "xqspipsu.h"		/* QSPIPSU device driver */


/*
 * The following constants define the commands which may be sent to the FLASH device.
 */
#define WRITE_STATUS_CMD				0x01
#define WRITE_CMD						0x02
#define READ_CMD						0x03
#define WRITE_DISABLE_CMD				0x04
#define READ_STATUS_CMD					0x05
#define WRITE_ENABLE_CMD				0x06
#define VOLATILE_WRITE_ENABLE_CMD		0x50
#define QUAD_MODE_ENABLE_BIT			0x06
#define FAST_READ_CMD					0x0B
#define DUAL_READ_CMD					0x3B
#define QUAD_READ_CMD					0x6B
#define BULK_ERASE_CMD					0xC7
#define	SEC_ERASE_4K_CMD				0x20
#define	BLK_ERASE_32K_CMD				0x52
#define	SEC_ERASE_64K_CMD				0xD8

#define FAST_READ_CMD					0x0B
#define DUAL_READ_CMD					0x3B
#define QUAD_READ_CMD					0x6B
#define BULK_ERASE_CMD					0xC7
#define READ_ID							0x9F
#define READ_CONFIG_CMD					0x35
#define WRITE_CONFIG_CMD				0x01
#define ENTER_4B_ADDR_MODE				0xB7
#define EXIT_4B_ADDR_MODE				0xE9
#define EXIT_4B_ADDR_MODE_ISSI			0x29

#define READ_CMD_4B						0x13
#define FAST_READ_CMD_4B				0x0C
#define DUAL_READ_CMD_4B				0x3C
#define QUAD_READ_CMD_4B				0x6C

#define BANK_REG_RD						0x16
#define BANK_REG_WR						0x17
/* Bank register is called Extended Address Register in Micron */
#define EXTADD_REG_RD					0xC8
#define EXTADD_REG_WR					0xC5
#define	DIE_ERASE_CMD					0xC4
#define READ_FLAG_STATUS_CMD			0x70

/*
 * Identification of Flash
 * Micron:
 * Byte 0 is Manufacturer ID;
 * Byte 1 is first byte of Device ID - 0xBB or 0xBA
 * Byte 2 is second byte of Device ID describes flash size:
 * 128Mbit : 0x18; 256Mbit : 0x19; 512Mbit : 0x20
 * Spansion:
 * Byte 0 is Manufacturer ID;
 * Byte 1 is Device ID - Memory Interface type - 0x20 or 0x02
 * Byte 2 is second byte of Device ID describes flash size:
 * 128Mbit : 0x18; 256Mbit : 0x19; 512Mbit : 0x20
 */
#define MICRON_ID_BYTE0		0x20
#define SPANSION_ID_BYTE0	0x01
#define WINBOND_ID_BYTE0	0xEF
#define MACRONIX_ID_BYTE0	0xC2
#define ISSI_ID_BYTE0		0x9D

#define FLASH_SIZE_ID_8M		0x14
#define FLASH_SIZE_ID_16M		0x15
#define FLASH_SIZE_ID_32M		0x16
#define FLASH_SIZE_ID_64M		0x17
#define FLASH_SIZE_ID_128M		0x18
#define FLASH_SIZE_ID_256M		0x19
#define FLASH_SIZE_ID_512M		0x20
#define FLASH_SIZE_ID_1G		0x21
#define FLASH_SIZE_ID_2G		0x22
/* Macronix size constants are different for 512M and 1G */
#define MACRONIX_FLASH_SIZE_ID_512M		0x1A
#define MACRONIX_FLASH_SIZE_ID_1G		0x1B
#define MACRONIX_FLASH_SIZE_ID_2G		0x1C
#define MACRONIX_FLASH_1_8_V_MX66_ID_512        (0x3A)

/*
 * Size in bits
 */
#define FLASH_SIZE_8M			0x0100000
#define FLASH_SIZE_16M			0x0200000
#define FLASH_SIZE_32M			0x0400000
#define FLASH_SIZE_64M			0x0800000
#define FLASH_SIZE_128M			0x1000000
#define FLASH_SIZE_256M			0x2000000
#define FLASH_SIZE_512M			0x4000000
#define FLASH_SIZE_1G			0x8000000
#define FLASH_SIZE_2G			0x10000000

/*
 * Flash address to which data is to be written.
 */
#define TEST_ADDRESS		0x000000
/*
 * Base address of Flash1
 */
#define FLASH1BASE 0x0000000

/*
 * Sixteen MB
 */
#define SIXTEENMB 0x1000000


/**************************** Type Definitions *******************************/

typedef struct{
	u32 jedec_id;	/* JEDEC ID */

	u32 SectSize;		/* Individual sector size or combined sector
				 * size in case of parallel config
				 */
	u32 NumSect;		/* Total no. of sectors in one/two
				 * flash devices
				 */
	u32 PageSize;		/* Individual page size or
				 * combined page size in case of parallel
				 * config
				 */
	u32 NumPage;		/* Total no. of pages in one/two flash
				 * devices
				 */
	u32 FlashDeviceSize;	/* This is the size of one flash device
				 * NOT the combination of both devices,
				 * if present
				 */
	u32 SectMask;		/* Mask to get sector start address */
	u8 NumDie;		/* No. of die forming a single flash */
} FlashInfo;


/************************** Function Prototypes ******************************/
int32_t QspiInstanceRegister(XQspiPsu *QspiPsuPtr, uint32_t *pageSize);
int FlashReadID(XQspiPsu *QspiPsuPtr);
int BulkErase(XQspiPsu *QspiPsuPtr, u8 *WriteBfrPtr);

int FlashErase(XQspiPsu *QspiPsuPtr, u32 Address, u32 ByteCount, u8 *WriteBfrPtr);
int FlashWrite(XQspiPsu *QspiPsuPtr, u32 Address, u32 ByteCount, u8 Command,
				u8 *WriteBfrPtr);
int FlashRead(XQspiPsu *QspiPsuPtr, u32 Address, u32 ByteCount, u8 Command,
				u8 *WriteBfrPtr, u8 *ReadBfrPtr);
int MultiDieRead(XQspiPsu *QspiPsuPtr, u32 Address, u32 ByteCount, u8 Command,
                 u8 *WriteBfrPtr, u8 *ReadBfrPtr);
u32 GetRealAddr(XQspiPsu *QspiPsuPtr, u32 Address);
int BulkErase(XQspiPsu *QspiPsuPtr, u8 *WriteBfrPtr);
int DieErase(XQspiPsu *QspiPsuPtr, u8 *WriteBfrPtr);
int FlashRegisterRead(XQspiPsu *QspiPsuPtr, u32 ByteCount, u8 Command, u8 *ReadBfrPtr);
int FlashRegisterWrite(XQspiPsu *QspiPsuPtr, u32 ByteCount, u8 Command,
					u8 *WriteBfrPtr, u8 WrEn);
int FlashEnterExit4BAddMode(XQspiPsu *QspiPsuPtr,unsigned int Enable);
int FlashEnableQuadMode(XQspiPsu *QspiPsuPtr);

#endif /* FLASH_COMMUNICATION_H */
