#ifndef _FLASH_MMAP_H_
#define _FLASH_MMAP_H_

/**
 * Flash partition table
 * | PartitionName  | PartitionSize        | AddressBegin  | AddressEnd  	| Note |
 *   golden_image     0x00000000 (1MB)       0x00000000      0x000FFFFF
 *   upgrade_image    0x01F00000 (31MB)      0x00100000      0x01FFFFFF
 *   rsv              0x01A00000 (30MB)      0x02000000      0x03DFFFFF      unused
 *   sys_info         0x00100000 (1MB)       0x03E00000      0x03EFFFFF
 *   log_info         0x00100000 (1MB )      0x03F00000      0x03FFFFFF
 */

/* If you want change the start address of "FLASH_HEADER_BASE" or "FLASH_BOOT_BASE" or "FLASH_FIRMWARE_BASE",
   Don't forget to change the value in "settings" file accordingly */
#define FLASH_GOLDEN_IMAGE_BASE       	(0x00000000)  // 0x0000_0000 -> 0x000F_FFFF 1MB
#define FLASH_GOLDEN_IMAGE_SIZE       	(0x00100000-0x1000)
#define FLASH_HEADER_IMAGE_BASE         (0x00100000-0x1000) 	// 0x0010_0000 - 0x1000 -> 0x0010_0000
#define FLASH_HEADER_IMAGE_SIZE         (0x1000)
#define FLASH_UPGADE_IMAGE_BASE         (0x00100000)  // 0x0010_0000 -> 0x01FF_FFFF 31MB
#define FLASH_UPGADE_IMAGE_SIZE         (0x01F00000)
#define FLASH_RESERVED_BASE           	(0x02000000)  // 0x0200_0000 -> 0x03DF_FFFF 30MB
#define FLASH_RESERVED_SIZE           	(0x01E00000)
#define FLASH_EF_ENV_BASE     			(0x03E00000)  // 0x03E0_0000 -> 0x03FF_FFFF 2MB
#define FLASH_EF_ENV_SIZE     			(0x00200000)

#define FLASH_EF_LOG_SIZE           	(0x100000)

#define FLASH_ERASE_MIN_SIZE        	(0x001000)  // 4k

#define READ_FLASH_SIZE					(0x1000)
/*
 * Size in bits
 */
#define FLASH_SIZE_64MB					0x4000000
#define FLASH_VALIED_SIZE_16MB			0x1000000
#define FLASH_VALIED_SIZE_32MB			0x2000000

#define FLASH_PAGE_SIZE					0x100		// 256

#endif /* _FLASH_MMAP_H_ */
