
#include "flash_raw.h"
#include "../easyflash/inc/easyflash.h" // need EF_ASSERT ef_calc_crc32
#include "string.h"

static FlashRaw_Item s_flash_raw_item_tbl[] = FLASH_RAW_ITEM_TABLE;

/* FlashRaw items header */
typedef struct FlashRaw_Header_t
{
    uint32_t magic; /* magic used to identify the raw flash blob */
    uint32_t len;   /* length of the raw flash blob data, ont include this header */
    uint32_t crc;   /* crc32 of the raw flash blob data, ont include this header */
    uint32_t rsv;   /* reserved for future use */
} FlashRaw_Header;

#define FLASH_RAW_HEADER_MAGIC  0X5A5A5A5A
#define FLASH_RAW_HEADER_RSV    0XFFFFFFFF

#define FLASH_RAW_HEADER_INIT(header) do {\
    header.magic = FLASH_RAW_HEADER_MAGIC;\
    header.len = 0;\
    header.crc = 0;\
    header.rsv = FLASH_RAW_HEADER_RSV;\
} while (0)

#define FLASH_CRC_INIT_VAL      0XFFFFFFFF

/**
 *  @b Description
 *  @n
 *      Init FlashRaw module. None of the FlashRaw API's can be used without invoking this API.
 *
 *  @retval
 *      Success    - 0
 *      Fail       < -1
 */
int32_t FlashRaw_Init(void)
{
    int32_t ret = 0;
    FlashRaw_Item *item = NULL;
    uint32_t item_tbl_size = 0;
    uint32_t i = 0;
    static bool init_ok = false;

    if (init_ok)
    {
        goto out;
    }

    item_tbl_size = sizeof(s_flash_raw_item_tbl) / sizeof(s_flash_raw_item_tbl[0]);
    for (i = 0; i < item_tbl_size; i++)
    {
        item = &s_flash_raw_item_tbl[i];
        EF_ASSERT(item->key != NULL);
        /* the raw item's address should be aligned to the flash's sector size */
        EF_ASSERT((item->addr % EF_ERASE_MIN_SIZE) == 0);
        EF_ASSERT(item->len > 0);
    }

    /* QSPI flash init */
    ret = FlashHal_Init();
    if (0 == ret)
    {
        init_ok = true;
    }
    else
    {
        goto out;
    }

out:
    return ret;
}

/**
 *  @b Description
 *  @n
 *      Set a NV item to flash.
 *
 *  @param[in]  key
 *      Key of the NV.
 *
 *  @param[in]  value_buf
 *      Buffer address of the contents.
 *
 *  @param[in]  buf_len
 *      Length of the contents in the buffer.
 *
 *  @retval
 *      Success    - 0
 *      Fail       < -1
 */
int32_t FlashRaw_Set(const char *key, const void *value_buf, size_t buf_len)
{
    int32_t ret = 0;
    FlashRaw_Header header;
    FlashRaw_Item *item = NULL;
    uint32_t item_tbl_size = 0;
    uint32_t i = 0;
    uint32_t total_len = 0;
    uint32_t sector_cnt = 0;

    if ((NULL == key) || (NULL == value_buf) || (0 == buf_len))
    {
        ret = RET_INVALID_PARAM;
        goto out;
    }

    item_tbl_size = sizeof(s_flash_raw_item_tbl) / sizeof(s_flash_raw_item_tbl[0]);
    for (i = 0; i < item_tbl_size; i++)
    {
        item = &s_flash_raw_item_tbl[i];
        if (0 == strncmp(key, item->key, strlen(item->key)))
        {
            /* the key has been found */
            break;
        }
    }
    if (i >= item_tbl_size)
    {
        /* the key cann't been found */
        ret = RET_INVALID_PARAM;
        goto out;
    }

    total_len = buf_len + sizeof(FlashRaw_Header);
    if (total_len > item->len)
    {
        /* the flash space isn't enough */
        ret = RET_INVALID_PARAM;
        goto out;
    }

    /* Erase the flash */
    sector_cnt = (total_len % EF_ERASE_MIN_SIZE) > 0 ? (total_len / EF_ERASE_MIN_SIZE) + 1 : (total_len / EF_ERASE_MIN_SIZE);
    for (i = 0; i < sector_cnt; i++)
    {
        ret = FlashHal_Erase(item->addr + i * EF_ERASE_MIN_SIZE, EF_ERASE_MIN_SIZE);
        if (ret != 0)
        {
            ret = RET_SYSTEM_ERR;
            goto out;
        }
    }

    /* Write the header */
    FLASH_RAW_HEADER_INIT(header);
    header.len = buf_len;
    header.crc = ef_calc_crc32(FLASH_CRC_INIT_VAL, value_buf, buf_len);
    ret = FlashHal_WriteByBytes(item->addr, (uint8_t *)&header, sizeof(header));
    if (ret != 0)
    {
        ret = RET_SYSTEM_ERR;
        goto out;
    }

    /* Write the data */
    ret = FlashHal_WriteByBytes(item->addr + sizeof(header), (uint8_t *)value_buf, buf_len);
    if (ret != 0)
    {
        ret = RET_SYSTEM_ERR;
        goto out;
    }

out:
    return ret;
}

/**
 *  @b Description
 *  @n
 *      Get a NV item from flash.
 *
 *  @param[in]  key
 *      Key of the NV.
 *
 *  @param[in]  value_buf
 *      Buffer address used to store the contents.
 *
 *  @param[in]  buf_len
 *      Length of the buffer.
 *
 *  @param[out]  saved_value_len
 *      Length of the NV contents in the flash.
 *
 *  @retval
 *      Length of data store in the buffer.
 */
size_t FlashRaw_Get(const char *key, void *value_buf, size_t buf_len, size_t *saved_value_len)
{
    size_t ret = 0;
    FlashRaw_Header header;
    FlashRaw_Item *item = NULL;
    uint32_t item_tbl_size = 0;
    uint32_t i = 0;
    uint32_t rd_len = 0;
    uint32_t crc = 0;

    if ((NULL == key) || (NULL == value_buf) || (0 == buf_len))
    {
        ret = 0;
        goto out;
    }

    item_tbl_size = sizeof(s_flash_raw_item_tbl) / sizeof(s_flash_raw_item_tbl[0]);
    for (i = 0; i < item_tbl_size; i++)
    {
        item = &s_flash_raw_item_tbl[i];
        if (0 == strncmp(key, item->key, strlen(item->key)))
        {
            /* the key has been found */
            break;
        }
    }
    if (i >= item_tbl_size)
    {
        /* the key cann't been found */
        ret = 0;
        goto out;
    }

    /* Read the header */
    ret = FlashHal_ReadByBytes(item->addr, (uint8_t *)&header, sizeof(header));
    if ((ret != 0) || (header.magic != FLASH_RAW_HEADER_MAGIC))
    {
        ret = 0;
        goto out;
    }

    /* Read the data */
    rd_len = header.len < buf_len ? header.len : buf_len;
    ret = FlashHal_ReadByBytes(item->addr + sizeof(header), (uint8_t *)value_buf, rd_len);
    if (ret != 0)
    {
        ret = 0;
        goto out;
    }

    /* Check the integrity */
    if (header.len == rd_len)
    {
        crc = ef_calc_crc32(FLASH_CRC_INIT_VAL, value_buf, rd_len);
        if (crc != header.crc)
        {
            ret = 0;
            goto out;
        }
    }

    /* read success, set the return value as the data length read from flash */
    ret = rd_len;
    if (saved_value_len != NULL)
    {
        *saved_value_len = header.len;
    }

out:
    return ret;
}

/**
 *  @b Description
 *  @n
 *      Delete a NV item.
 *
 *  @param[in]  key
 *      Key of the NV.
 *
 *  @retval
 *      Success    - 0
 *      Fail       < -1
 */
int32_t FlashRaw_Del(const char *key)
{
    int32_t ret = 0;
    FlashRaw_Item *item = NULL;
    uint32_t item_tbl_size = 0;
    uint32_t i = 0;
    uint32_t total_len = 0;
    uint32_t sector_cnt = 0;

    if (NULL == key)
    {
        ret = RET_INVALID_PARAM;
        goto out;
    }

    item_tbl_size = sizeof(s_flash_raw_item_tbl) / sizeof(s_flash_raw_item_tbl[0]);
    for (i = 0; i < item_tbl_size; i++)
    {
        item = &s_flash_raw_item_tbl[i];
        if (0 == strncmp(key, item->key, strlen(item->key)))
        {
            /* the key has been found */
            break;
        }
    }
    if (i >= item_tbl_size)
    {
        /* the key cann't been found */
        ret = RET_INVALID_PARAM;
        goto out;
    }

    /* Erase the flash */
    total_len = item->len;
    sector_cnt = (total_len % EF_ERASE_MIN_SIZE) > 0 ? (total_len / EF_ERASE_MIN_SIZE) + 1 : (total_len / EF_ERASE_MIN_SIZE);
    for (i = 0; i < sector_cnt; i++)
    {
        ret = FlashHal_Erase(item->addr + i * EF_ERASE_MIN_SIZE, EF_ERASE_MIN_SIZE);
        if (ret != 0)
        {
            ret = RET_SYSTEM_ERR;
            goto out;
        }
    }

out:
    return ret;
}

/**
 *  @b Description
 *  @n
 *      Find the NV by key in the flash raw table.
 *
 *  @param[in]  key
 *      Key of the NV.
 *
 *  @retval
 *      The NV is in the table     - true
 *      The NV is not in the table - false
 */
bool FlashRaw_Find(const char *key)
{
    bool ret = true;
    FlashRaw_Item *item = NULL;
    uint32_t item_tbl_size = 0;
    uint32_t i = 0;

    if (NULL == key)
    {
        ret = false;
        goto out;
    }

    item_tbl_size = sizeof(s_flash_raw_item_tbl) / sizeof(s_flash_raw_item_tbl[0]);
    for (i = 0; i < item_tbl_size; i++)
    {
        item = &s_flash_raw_item_tbl[i];
        if (0 == strncmp(key, item->key, strlen(item->key)))
        {
            /* the key has been found */
            ret = true;
            goto out;
        }
    }
    if (i >= item_tbl_size)
    {
        /* the key cann't been found */
        ret = false;
        goto out;
    }

out:
    return ret;
}
