#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../inc/easyflash.h"

/* Demo: key for test */
#define EF_TEST_DEMO_KEY "ef_test_demo_key"

/* Demo: type for test */
typedef struct ef_test_demo_info {
    uint8_t m_u8;
    int16_t m_i16;
    float   m_f;
    int32_t m_arr[3];
    char m_str[10];
} ef_test_demo_info_s;

/* Demo: set default value for test */
static void ef_test_demo_set_to_default(ef_test_demo_info_s *info)
{
    memset(info, 0, sizeof(*info));
    info->m_u8 = 8;
    info->m_i16 = -16;
    info->m_f = 12.345;

    info->m_arr[0] = -1;
    info->m_arr[1] = 0;
    info->m_arr[2] = 2;

    strncpy(info->m_str, EF_TEST_DEMO_KEY, sizeof(info->m_str) - 1);
}

/* Demo: dump value for test */
static void ef_test_demo_dump(ef_test_demo_info_s *info)
{
    uint32_t i = 0;

    EF_INFO("m_u8       = %u\r\n", info->m_u8);
    EF_INFO("m_i16      = %d\r\n", info->m_i16);
    EF_INFO("m_f        = %f\r\n", info->m_f);
    for (i = 0; i < 3; i++)
    {
        EF_INFO("m_arr[%d]   = %d\r\n", i, info->m_arr[i]);
    }
    EF_INFO("m_str      = %.*s\r\n", sizeof(info->m_str) - 1, info->m_str);
}

/**
 * @brief Env demo.
 */
void ef_test_demo(void)
{
    EfErrCode ef_ret = EF_NO_ERR;
    size_t rd_len = 0;
    size_t rd_ret = 0;
    ef_test_demo_info_s demo_val;

    do {
        // step 1: init the library.
        ef_ret = easyflash_init();
        if (ef_ret != EF_NO_ERR)
        {
            EF_DEBUG("Init esayflash failed, result=%d.\r\n", ef_ret);
            break;
        }

        // step 2: check whether your key is exist, if not, set to default value.
        rd_ret = ef_get_env_blob(EF_TEST_DEMO_KEY, &demo_val, sizeof(demo_val), &rd_len);
        if ((rd_len > 0) && (rd_ret == rd_len) && (rd_len == sizeof(demo_val)))
        {
            EF_DEBUG("Get env %s success.\r\n", EF_TEST_DEMO_KEY);
        }
        else
        {
            EF_DEBUG("Get env %s failed, rd_len=%u, rd_ret=%u.\r\n", EF_TEST_DEMO_KEY, rd_len, rd_ret);
            /* When get failed, set to the default value. */
            ef_test_demo_set_to_default(&demo_val);
            ef_ret = ef_set_env_blob(EF_TEST_DEMO_KEY, &demo_val, sizeof(demo_val));
            if (ef_ret != EF_NO_ERR)
            {
                EF_DEBUG("Set env %s failed, result=%d.\r\n", EF_TEST_DEMO_KEY, ef_ret);
                break;
            }
        }
        ef_test_demo_dump(&demo_val);

        // step 3: modify it when needed.
        demo_val.m_u8++;
        demo_val.m_i16 -= demo_val.m_u8;
        demo_val.m_f *= -0.1;
        demo_val.m_arr[0] = 3;
        demo_val.m_arr[1] = 6;
        demo_val.m_arr[2] = -9;
        memset(demo_val.m_str, 0, sizeof(demo_val.m_str));
        strncpy(demo_val.m_str, "hello", sizeof(demo_val.m_str) - 1);

        ef_ret = ef_set_env_blob(EF_TEST_DEMO_KEY, &demo_val, sizeof(demo_val));
        if (ef_ret != EF_NO_ERR)
        {
            EF_DEBUG("Set env %s failed, result=%d.\r\n", EF_TEST_DEMO_KEY, ef_ret);
            break;
        }

        rd_ret = ef_get_env_blob(EF_TEST_DEMO_KEY, &demo_val, sizeof(demo_val), &rd_len);
        if ((rd_len > 0) && (rd_ret == rd_len) && (rd_len == sizeof(demo_val)))
        {
            EF_DEBUG("Get env %s success.\r\n", EF_TEST_DEMO_KEY);
        }
        else
        {
            EF_DEBUG("Get env %s failed.\r\n", EF_TEST_DEMO_KEY);
            break;
        }
        ef_test_demo_dump(&demo_val);

        // step x: delete it when needed, which maybe unuseful.
    } while (0);
}

/**
 * @brief Evn read and write function test.
 */
static void ef_test_env_rw(void)
{
    EfErrCode ef_ret = EF_NO_ERR;
    size_t rd_len = 0;
    size_t rd_ret = 0;
    uint32_t test_count = 0;

    do {
        ef_ret = easyflash_init();
        if (ef_ret != EF_NO_ERR)
        {
            EF_DEBUG("Init esayflash failed, result=%d.\r\n", ef_ret);
            break;
        }

        rd_ret = ef_get_env_blob("test_count", &test_count, sizeof(test_count), &rd_len);
        if ((rd_len > 0) && (rd_ret == rd_len) && (rd_len == sizeof(test_count)))
        {
            EF_DEBUG("Get env success, test_count is %u.\r\n", test_count);
        }
        else
        {
            EF_DEBUG("Get env failed, rd_len=%u, rd_ret=%u.\r\n", rd_len, rd_ret);
            break;
        }

        test_count++;
        ef_ret = ef_set_env_blob("test_count", &test_count, sizeof(test_count));
        if (EF_NO_ERR == ef_ret)
        {
            EF_DEBUG("Set env test_count to %u success.\r\n", test_count);
        }
        else
        {
            EF_DEBUG("Set env failed, result=%d.\r\n", ef_ret);
            break;
        }
    } while (0);
}

/**
 * @brief Evn delete function test.
 */
static void ef_test_env_del(void)
{
    EfErrCode ef_ret = EF_NO_ERR;
    size_t rd_len = 0;
    size_t rd_ret = 0;
    char test_buf[20] = {0};

    do
    {
        ef_ret = ef_set_env_blob("test_key", "test_value", strlen("test_value"));
        if (ef_ret != EF_NO_ERR)
        {
            EF_DEBUG("Set env failed, result=%d.\r\n", ef_ret);
            break;
        }

        rd_ret = ef_get_env_blob("test_key", test_buf, sizeof(test_buf), &rd_len);
        if (rd_len > 0)
        {
            EF_DEBUG("rd_ret=%u, rd_len=%u, test_buf=%s.\r\n", rd_ret, rd_len, test_buf);
        }
        else
        {
            EF_DEBUG("Get env failed, rd_len=%u, rd_ret=%u.\r\n", rd_len, rd_ret);
            break;
        }

        ef_ret = ef_del_env("test_key");
        if (EF_NO_ERR == ef_ret)
        {
            rd_ret = ef_get_env_blob("test_key", test_buf, sizeof(test_buf), &rd_len);
            if ((rd_ret > 0) || (rd_len > 0))
            {
                EF_DEBUG("After delete, rd_ret=u%, rd_len=%u, test_buf=%s, !!!no effect!!!\r\n", rd_ret, rd_len, test_buf);
                break;
            }
            else
            {
                EF_DEBUG("After delete, get env failed, rd_len=%u, rd_ret=%u.\r\n", rd_len, rd_ret);
            }
        }
        else
        {
            EF_DEBUG("Delete env failed, result=%d.\r\n", ef_ret);
            break;
        }
    } while (0);
}

// #define EF_TEST_SMOKE_RUN
#ifdef EF_TEST_SMOKE_RUN
#define EF_TEST_SMOKE_KEY "ef_smoke_key"
#define EF_TEST_SMOKE_LOOP_CNT (109)
#define EF_TEST_SMOKE_BUF_SIZE (3 * 1024 + 3)
static uint8_t ef_test_smoke_buff_set[EF_TEST_SMOKE_BUF_SIZE];
static uint8_t ef_test_smoke_buff_get[EF_TEST_SMOKE_BUF_SIZE];

/**
 * @brief Evn smoke test.
 */
static void ef_test_env_smoke(void)
{
    uint32_t i = 0;
    uint32_t k = 0;
    size_t rd_len = 0;
    size_t rd_ret = 0;

    for (i = 0; i < EF_TEST_SMOKE_LOOP_CNT; i++)
    {
        for (k = 0; k < EF_TEST_SMOKE_BUF_SIZE; k++)
        {
            ef_test_smoke_buff_set[k] = (i + k) & 0xFF;
        }

        EF_DEBUG("%s round %u.\r\n", __FUNCTION__, i);

        rd_ret = ef_set_env_blob(EF_TEST_SMOKE_KEY, ef_test_smoke_buff_set, sizeof(ef_test_smoke_buff_set));
        if (rd_ret != EF_NO_ERR)
        {
            EF_DEBUG("Set env %s failed, result=%d.\r\n", EF_TEST_SMOKE_KEY, rd_ret);
            goto out;
        }

        memset(ef_test_smoke_buff_get, 0, sizeof(ef_test_smoke_buff_get));
        rd_ret = ef_get_env_blob(EF_TEST_SMOKE_KEY, ef_test_smoke_buff_get, sizeof(ef_test_smoke_buff_get), &rd_len);
        if ((rd_len > 0) && (rd_ret == rd_len) && (rd_len == sizeof(ef_test_smoke_buff_get)))
        {
            EF_DEBUG("Get env %s success, rd_len=%u.\r\n", EF_TEST_SMOKE_KEY, rd_len);
            if (memcmp(ef_test_smoke_buff_set, ef_test_smoke_buff_get, EF_TEST_SMOKE_BUF_SIZE) != 0)
            {
                EF_DEBUG("The buffer contents have some differents, !!!test failed!!!\r\n");
                goto out;
            }
        }
        else
        {
            EF_DEBUG("Get env %s failed.\r\n", EF_TEST_SMOKE_KEY);
            goto out;
        }
    }

out:
    EF_DEBUG("%s finish.\r\n", __FUNCTION__);
    return;
}
#endif /* EF_TEST_SMOKE_RUN */

/**
 * @brief Evn function test routine.
 */
void ef_test_ft(void)
{
    /* Basic read and write test */
    ef_test_env_rw();

    /* Delete test */
    ef_test_env_del();

#ifdef EF_TEST_SMOKE_RUN
    /* Smoke test, large size env loop test */
    ef_test_env_smoke();
#endif /* EF_TEST_SMOKE_RUN */
}
