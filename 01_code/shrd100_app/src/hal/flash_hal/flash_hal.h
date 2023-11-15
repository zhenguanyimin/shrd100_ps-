#ifndef HAL_FLASH_HAL_H
#define HAL_FLASH_HAL_H

#include <stdio.h>
#include "../../inc/common.h"

#define MICRON_ID		0x20
#define SPANSION_ID		0x01
#define WINBOND_ID		0xEF
#define MACRONIX_ID		0xC2
#define ISSI_ID			0x9D


/* global function define */
int32_t FlashHal_Init(void);
int32_t FlashHal_Erase(uint32_t flashAddr, uint32_t size);
int32_t FlashHal_ReadByBytes(uint32_t flashAddr, uint8_t *readBuf, uint32_t size);
int32_t FlashHal_WriteByBytes(uint32_t flashAddr, uint8_t *writeBuf, uint32_t size);

#endif /* HAL_FLASH_HAL_H */
