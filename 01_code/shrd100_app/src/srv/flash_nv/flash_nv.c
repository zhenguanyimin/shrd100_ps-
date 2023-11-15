
#include "flash_nv.h"
#include "flash_raw.h"
#include "../easyflash/inc/easyflash.h"

static FlashNV_KeyMap s_flash_nv_key_map_tbl[] = FLASH_NV_KEY_MAP_TABLE;

static const char * FlashNV_GetNameById(FlashNV_ID id)
{
    const char *name = NULL;
    uint32_t i = 0;

    if (!FLASH_NV_ID_VALID(id))
    {
        return NULL;
    }

    for (i = 0; i < sizeof(s_flash_nv_key_map_tbl) / sizeof(s_flash_nv_key_map_tbl[0]); i++)
    {
        if (id == s_flash_nv_key_map_tbl[i].id)
        {
            name = s_flash_nv_key_map_tbl[i].name;
            break;
        }
    }

    return name;
}

/**
 *  @b Description
 *  @n
 *      Init FlashNV module. None of the FlashNV API's can be used without invoking this API.
 *
 *  @retval
 *      Success    - 0
 *      Fail       < -1
 */
int32_t FlashNV_Init(void)
{
    int32_t ret = 0;

    ret = FlashRaw_Init();
    if (ret != 0)
    {
        goto out;
    }

    ret = (int32_t)easyflash_init();
    if (ret != EF_NO_ERR)
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
 *  @param[in]  id
 *      ID of the NV.
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
int32_t FlashNV_Set(FlashNV_ID id, const void *value_buf, size_t buf_len)
{
    int32_t ret = 0;
    const char *key = NULL;

    key = FlashNV_GetNameById(id);
    if (NULL == key)
    {
        return RET_INVALID_PARAM;
    }

    if (FlashRaw_Find(key))
    {
        ret = FlashRaw_Set(key, value_buf, buf_len);
    }
    else
    {
        ret = (int32_t)ef_set_env_blob(key, value_buf, buf_len);
    }

    return ret;
}

/**
 *  @b Description
 *  @n
 *      Get a NV item from flash.
 *
 *  @param[in]  id
 *      ID of the NV.
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
size_t FlashNV_Get(FlashNV_ID id, void *value_buf, size_t buf_len, size_t *saved_value_len)
{
    size_t ret = 0;
    const char *key = NULL;

    key = FlashNV_GetNameById(id);
    if (NULL == key)
    {
        return 0;
    }

    if (FlashRaw_Find(key))
    {
        ret = FlashRaw_Get(key, value_buf, buf_len, saved_value_len);
    }
    else
    {
        ret = ef_get_env_blob(key, value_buf, buf_len, saved_value_len);
    }

    return ret;
}

/**
 *  @b Description
 *  @n
 *      Delete a NV item.
 *
 *  @param[in]  id
 *      ID of the NV.
 *
 *  @retval
 *      Success    - 0
 *      Fail       < -1
 */
int32_t FlashNV_Del(FlashNV_ID id)
{
    int32_t ret = 0;
    const char *key = NULL;

    key = FlashNV_GetNameById(id);
    if (NULL == key)
    {
        return RET_INVALID_PARAM;
    }

    if (FlashRaw_Find(key))
    {
        ret = FlashRaw_Del(key);
    }
    else
    {
        ret = (int32_t)ef_del_env(key);
    }

    return ret;
}
