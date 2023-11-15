
#ifndef RADAR_ERROR_H
#define RADAR_ERROR_H
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "../cfg/shrd_config.h"

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))
#endif /* ARRAY_SIZE */

#ifndef UNUSED
#define UNUSED(var) (void)(var)
#endif /*UNUSED */

#ifndef STATIC
#define STATIC static
#endif /* STATIC */

#ifndef PRIVATE
#define PRIVATE static
#endif /* PRIVATE */

#define U64_H32(val) ((val >> 32) & 0xffffffff)
#define U64_L32(val) (val & 0xffffffff)

#define RADAR_MANUFACTURER "AUTEL"

#define RADAR_TX_CHN_ROW_CNT (4)
#define RADAR_TX_CHN_COL_CNT (16)
#define RADAR_RX_CHN_ROW_CNT (4)
#define RADAR_RX_CHN_COL_CNT (16)

#define RADAR_TARGET_NUM_MAX (64)

#define RADAR_ERRORNO_BASE (-1000)
typedef enum radar_ret_code {
    RET_OK = 0,
    RET_GENERAL_ERR = RADAR_ERRORNO_BASE - 1,
    RET_SYSTEM_ERR = RADAR_ERRORNO_BASE - 2,
    RET_NOT_SUPPORT = RADAR_ERRORNO_BASE - 3,
    RET_NO_RESOURCE = RADAR_ERRORNO_BASE - 4,
    RET_INVALID_PARAM = RADAR_ERRORNO_BASE - 5,
} ret_code_t;

#endif /* RADAR_ERROR_H */
