
#include <stdint.h>
#include "xil_printf.h"
#include "flash_hal.h"
#include "../../drv/flash/flash.h"
#include "../../cfg/flash_mmap.h"


#define EREASE_SIZE_BYTE    4096U
#define QSPI_DEVICE_ID		XPAR_XQSPIPS_0_DEVICE_ID

#define FLASH_ADDR_VALID(val) (((val) >= 0) && ((val) < FLASH_SIZE_64MB))

/**
 * Brief: Check whether the flash write operation is successful.
 */
static int32_t FlashHal_CheckData(uint32_t flashAddr, uint8_t *writeBuf, uint32_t size)
{
    int32_t ret = RET_OK;
    uint8_t tmp_buf[FLASH_PAGE_SIZE] = {0};
    uint32_t tmp_len = 0;
    uint32_t tmp_idx = 0;

    while (size > 0)
    {
        memset(tmp_buf, 0, sizeof(tmp_buf));
        tmp_len = size < sizeof(tmp_buf) ? size : sizeof(tmp_buf);
        ret = flash_memory_readb(flashAddr + tmp_idx, tmp_buf, tmp_len);
        if (ret != RET_OK)
        {
            break;
        }
        if (memcmp(writeBuf + tmp_idx, tmp_buf, tmp_len) != 0)
        {
            ret = RET_SYSTEM_ERR;
            break;
        }
        size -= tmp_len;
        tmp_idx += tmp_len;
    }

    return ret;
}

/**
 *  @b Description
 *  @n
 *      Init flash driver module.
 *
 *  @retval
 *      Success    - 0
 *      Fail       - others
 */
int32_t FlashHal_Init(void)
{
    // flash_init is called in main.c, so here we don't need to do anything.

    return RET_OK;
}

/**
 *  @b Description
 *  @n
 *      Erase flash area.
 *
 *  @param[in]  flashAddr
 *      The start address to erase.
 *
 *  @param[in]  size
 *      The size to erase, in bytes.
 *
 *  @retval
 *      Success    - 0
 *      Fail       - others
 */
int32_t FlashHal_Erase(uint32_t flashAddr, uint32_t size)
{
    int32_t ret = RET_OK;

    if (!FLASH_ADDR_VALID(flashAddr))
    {
        ret = RET_INVALID_PARAM;
        goto out;
    }

    if (flashAddr % FLASH_ERASE_MIN_SIZE != 0)
    {
        ret = RET_INVALID_PARAM;
        goto out;
    }

    flash_memory_erase(flashAddr, EREASE_SIZE_BYTE);

out:
    return ret;
}


/**
 *  @b Description
 *  @n
 *      Read data from flash to ram.
 *
 *  @param[in]  flashAddr
 *      The start address to read.
 *
 *  @param[in]  readBuf
 *      The start address in ram used to store the data read from flash.
 *
 *  @param[in]  size
 *      The size to read, in bytes.
 *
 *  @retval
 *      Success    - 0
 *      Fail       - others
 */
int32_t FlashHal_ReadByBytes(uint32_t flashAddr, uint8_t *readBuf, uint32_t size)
{
    int32_t ret = RET_OK;

	if (!FLASH_ADDR_VALID(flashAddr))
    {
        ret = RET_INVALID_PARAM;
        goto out;
    }

    ret = flash_memory_readb(flashAddr, readBuf, size);

out:
    return ret;
}

/**
 *  @b Description
 *  @n
 *      Write data from ram to flash.
 *
 *  @param[in]  flashAddr
 *      The start address to write.
 *
 *  @param[in]  writeBuf
 *      The start address in ram used to store the data write to flash.
 *
 *  @param[in]  size
 *      The size to write, in bytes.
 *
 *  @retval
 *      Success    - 0
 *      Fail       - others
 */
int32_t FlashHal_WriteByBytes(uint32_t flashAddr, uint8_t *writeBuf, uint32_t size)
{
    int32_t ret = RET_OK;
    uint32_t tail_len = 0;

    if (!FLASH_ADDR_VALID(flashAddr))
    {
        ret = RET_INVALID_PARAM;
        goto out;
    }

    if (0 == (flashAddr % FLASH_PAGE_SIZE)) // align with page size
    {
        ret = flash_memory_writeb(flashAddr, writeBuf, size);
        if (ret != RET_OK)
        {
            goto out;
        }
    }
    else
    {
        tail_len = FLASH_PAGE_SIZE - (flashAddr % FLASH_PAGE_SIZE); // length need to be written to a non-empty page
        tail_len = tail_len < size ? tail_len : size;
        ret = flash_memory_writeb(flashAddr, writeBuf, tail_len);
        if (ret != RET_OK)
        {
            goto out;
        }

        if (tail_len < size)
        {
            ret = flash_memory_writeb(flashAddr + tail_len, writeBuf + tail_len, size - tail_len);
            if (ret != RET_OK)
            {
                goto out;
            }
        }
    }

    ret = FlashHal_CheckData(flashAddr, writeBuf, size);
    if (ret != RET_OK)
    {
        goto out;
    }

out:
    return ret;
}
