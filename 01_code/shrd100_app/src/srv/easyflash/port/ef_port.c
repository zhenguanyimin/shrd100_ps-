/*
 * This file is part of the EasyFlash Library.
 *
 * Copyright (c) 2015-2019, Armink, <armink.ztl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Function: Portable interface for each platform.
 * Created on: 2015-01-16
 */

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define OS_FREERTOS

#ifdef OS_FREERTOS
#include <FreeRTOS.h>
#include <semphr.h>
#endif /* OS_FREERTOS */

#include "../../../hal/flash_hal/flash_hal.h"
#include "../inc/easyflash.h"
#include "../../log/log.h"

#define EF_LOG_BUF_SIZE 256

#ifdef OS_FREERTOS
#define USE_RTOS
static SemaphoreHandle_t s_ef_sem_hdl;
#endif /* OS_FREERTOS */

static uint32_t test_count = 0;
static uint8_t boot_time[10] = {0, 1, 2, 3};

/* default environment variables set for user */
static const ef_env default_env_set[] =
{
    {"maintainer", "chenandong", 0},
    {"test_count", &test_count, sizeof(test_count)},
    {"boot_time", &boot_time, sizeof(boot_time)},
    /** note: the above envs are just used for demo and test purpose,
    * please don't add any envs here, instead, do that in the specific app module is better.
    * you can see the function ef_test_demo() in the ../test/ef_test.c file for the example.
    */
};

/**
 * Flash port for hardware initialize.
 *
 * @param default_env default ENV set for user
 * @param default_env_size default ENV size
 *
 * @return result
 */
EfErrCode ef_port_init(ef_env const **default_env, size_t *default_env_size)
{
    EfErrCode result = EF_NO_ERR;

    EF_ASSERT(default_env != NULL);
    EF_ASSERT(default_env_size != NULL);

    *default_env = default_env_set;
    *default_env_size = sizeof(default_env_set) / sizeof(default_env_set[0]);

#ifdef USE_RTOS
    if (NULL == s_ef_sem_hdl)
    {
        s_ef_sem_hdl = xSemaphoreCreateMutex();
        EF_ASSERT(s_ef_sem_hdl != NULL);
    }
#endif /* USE_RTOS */

    return result;
}

/**
 * Read data from flash.
 * @note This operation's units is word.
 *
 * @param addr flash address
 * @param buf buffer to store read data
 * @param size read bytes size
 *
 * @return result
 */
EfErrCode ef_port_read(uint32_t addr, uint32_t *buf, size_t size)
{
    EfErrCode result = EF_NO_ERR;
    int32_t flash_ret = 0;

    EF_ASSERT(buf != NULL);
    EF_ASSERT(size > 0);

    flash_ret = FlashHal_ReadByBytes(addr, (uint8_t *)buf, size);
    result = (0 == flash_ret) ? EF_NO_ERR : EF_READ_ERR;

    // ef_log_debug(__FILE__, __LINE__, "result=%d, flash_ret=%d\r\n", result, flash_ret);
    return result;
}

/**
 * Erase data on flash.
 * @note This operation is irreversible.
 * @note This operation's units is different which on many chips.
 *
 * @param addr flash address
 * @param size erase bytes size
 *
 * @return result
 */
EfErrCode ef_port_erase(uint32_t addr, size_t size)
{
    EfErrCode result = EF_NO_ERR;
    int32_t flash_ret = 0;

    /* make sure the start address is a multiple of EF_ERASE_MIN_SIZE */
    EF_ASSERT(addr % EF_ERASE_MIN_SIZE == 0);
    // dingff TODO: allow erase more than one sector or not ?
    EF_ASSERT((size > 0) && (size <= EF_ERASE_MIN_SIZE));

    flash_ret = FlashHal_Erase(addr, EF_ERASE_MIN_SIZE);
    result = (0 == flash_ret) ? EF_NO_ERR : EF_ERASE_ERR;

    // ef_log_debug(__FILE__, __LINE__, "result=%d, flash_ret=%d\r\n", result, flash_ret);
    return result;
}

/**
 * Write data to flash.
 * @note This operation's units is word.
 * @note This operation must after erase. @see flash_erase.
 *
 * @param addr flash address
 * @param buf the write data buffer
 * @param size write bytes size
 *
 * @return result
 */
EfErrCode ef_port_write(uint32_t addr, const uint32_t *buf, size_t size)
{
    EfErrCode result = EF_NO_ERR;
    int32_t flash_ret = 0;
    uint32_t tmp_cnt = 0;

    EF_ASSERT(buf != NULL);
    EF_ASSERT(size > 0);

    for (tmp_cnt = 0; tmp_cnt < 3; tmp_cnt++)
    {
        flash_ret = FlashHal_WriteByBytes(addr, (uint8_t *)buf, size);
        if (0 == flash_ret)
        {
            break;
        }
        else
        {
            result = EF_WRITE_ERR; // set a breakpoint here and check the tmp_cnt
        }
    }
    result = (0 == flash_ret) ? EF_NO_ERR : EF_WRITE_ERR;

    // ef_log_debug(__FILE__, __LINE__, "result=%d, flash_ret=%d\r\n", result, flash_ret);
    return result;
}

/**
 * lock the ENV ram cache
 */
void ef_port_env_lock(void)
{
#ifdef USE_RTOS
    if (s_ef_sem_hdl != NULL)
    {
        (void)xSemaphoreTake(s_ef_sem_hdl, portMAX_DELAY);
    }
#endif /* USE_RTOS */
}

/**
 * unlock the ENV ram cache
 */
void ef_port_env_unlock(void)
{
#ifdef USE_RTOS
    if (s_ef_sem_hdl != NULL)
    {
        (void)xSemaphoreGive(s_ef_sem_hdl);
    }
#endif /* USE_RTOS */
}

/**
 * This function is print flash debug info.
 *
 * @param file the file which has call this function
 * @param line the line number which has call this function
 * @param format output format
 * @param ... args
 *
 */
void ef_log_debug(const char *file, const long line, const char *format, ...)
{
#ifdef PRINT_DEBUG
    char log_buf[EF_LOG_BUF_SIZE] = {0};
    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);

    memset(log_buf, 0, sizeof(log_buf));
    vsnprintf(log_buf, sizeof(log_buf), format, args);

    log_record(LL_DEBUG, "[EF] [%s:%d] %s", file, line, log_buf);

    va_end(args);
#endif
}

/**
 * This function is print flash routine info.
 *
 * @param format output format
 * @param ... args
 */
void ef_log_info(const char *format, ...)
{
    char log_buf[EF_LOG_BUF_SIZE] = {0};
    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);

    memset(log_buf, 0, sizeof(log_buf));
    vsnprintf(log_buf, sizeof(log_buf), format, args);

    log_record(LL_INFO, "[EF] %s", log_buf);

    va_end(args);
}

/**
 * This function is print flash non-package info.
 *
 * @param format output format
 * @param ... args
 */
void ef_print(const char *format, ...)
{
    char log_buf[EF_LOG_BUF_SIZE] = {0};
    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);

    memset(log_buf, 0, sizeof(log_buf));
    vsnprintf(log_buf, sizeof(log_buf), format, args);

    log_record(LL_PRINTF, "%s", log_buf);

    va_end(args);
}
