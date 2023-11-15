
#ifndef FLASH_NV_H
#define FLASH_NV_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "../../inc/common.h"
#include "../../hal/flash_hal/flash_hal.h"
#include "../../hal/flash_hal/flash_map.h"

/* FlashNV constants */
#define FLASH_NV_KEY_LEN_MAX            (32) /* the NV key length must less than it, ref EF_ENV_NAME_MAX */

/* FlashNV keys list, enum (ID) */
typedef enum FlashNV_ID_t
{
    FLASH_NV_ID_INVALID = 0,
    FLASH_NV_ID_SN_PRODUCT, // serial number of the product
    FLASH_NV_ID_SN_BOARD, // serial number of the board
    FLASH_NV_ID_WORK_MODE, // work mode of the radar
    FLASH_NV_ID_UPGRADE_IMAGE, // firmware upgrade information
    FLASH_NV_ID_TROUBLECODE, // diagnostic trouble code (DTC)
    FLASH_NV_ID_WATCHDOG_STAT, // the state of watchdog
    FLASH_NV_ID_WAVA_CONFIG, // parameters of the RF subsystem
    FLASH_NV_ID_CALIB_DATA, // radar position calibration data
    FLASH_NV_ID_ANGLE_CALIB_DATA, // angle calibration data
    FLASH_NV_ID_VEHICLE_PARAMS, // parameters about the vehicle
    FLASH_NV_ID_VEHICLE_SIGNAL_INFO, // CAN signal definition of the vehicle, like DBC
    FLASH_NV_ID_WARNINGCFG,// Warning parameters
	FLASH_NV_ID_LOGGING_INDEX,//Store log data length
    FLASH_NV_ID_CALIB_DATA_1, // radar position calibration data
    FLASH_NV_ID_CALIB_DATA_2, // radar position calibration data
    FLASH_NV_ID_CALIB_DATA_3, // radar position calibration data
    FLASH_NV_ID_MAG_DATA, // Magnetometer calibration data
    FLASH_NV_ID_ACC_DATA, // Accelerometer calibration data
	FLASH_NV_ID_IMAGE_VERIFICATION_PARA, // Upgrade image verification parameter
    /* Add new items above here */
    FLASH_NV_ID_WIFI_TCPIP_INFO,
	FLASH_NV_ID_SCREEN_C2NETWORK_SWITCH_FLAG, // screen c2 network switch flag
	FLASH_NV_ID_TCP_IP_INFO,
	FLASH_NV_ID_WIFI_INFO,
	FLASH_NV_ID_UPGRADE_FLAG, // Upgrade image verification parameter
	FLASH_NV_ID_OTHER_TCP_IP_INFO,

    FLASH_NV_ID_BUTT
} FlashNV_ID;
#define FLASH_NV_ID_VALID(id) (((id) > FLASH_NV_ID_INVALID) && ((id) < FLASH_NV_ID_BUTT))

/* FlashNV keys list, string (Name) */
#define FLASH_NV_NAME_SN_PRODUCT            "SN_PRODUCT"
#define FLASH_NV_NAME_SN_BOARD              "SN_BOARD"
#define FLASH_NV_NAME_WORK_MODE             "WORK_MODE"
#define FLASH_NV_NAME_UPGRADE_IMAGE         "UPGRADE_IMAGE"
#define FLASH_NV_NAME_TROUBLECODE           "TROUBLECODE"
#define FLASH_NV_NAME_WATCHDOG_STAT         "WATCHDOG_STAT"
#define FLASH_NV_NAME_WAVA_CONFIG           "WAVA_CONFIG"
#define FLASH_NV_NAME_FRE_CALIB_DATA        "FRE_CALIB_DATA"
#define FLASH_NV_NAME_ANGLE_CALIB_DATA      "ANGLE_CALIB_DATA"
#define FLASH_NV_NAME_VEHICLE_PARAMS        "VEHICLE_PARAMS"
#define FLASH_NV_NAME_VEHICLE_SIGNAL_INFO   "VEHICLE_SIGNAL_INFO"
#define FLASH_NV_NAME_WARNINGCFG            "WARNINGCFG"
#define FLASH_NV_NAME_LOGGING_INDEX         "LOGGING_INDEX"
#define FLASH_NV_NAME_FRE_CALIB_DATA_1        "FRE_CALIB_DATA_1"
#define FLASH_NV_NAME_FRE_CALIB_DATA_2        "FRE_CALIB_DATA_2"
#define FLASH_NV_NAME_FRE_CALIB_DATA_3        "FRE_CALIB_DATA_3"
#define FLASH_NV_NAME_MAG_CALIB_DATA        "MAG_CALIB_DATA"
#define FLASH_NV_NAME_ACC_CALIB_DATA        "ACC_CALIB_DATA"
#define FLASH_NV_NAME_WIFI_TCPIP_INFO       		"WIFI_TCPIP_INFO"
#define FLASH_NV_NAME_SCREEN_C2NETWORK_SWITCH_FLAG   "SCREEN_C2NETWORK_SWITCH_FLAG"
#define FLASH_NV_NAME_TCP_IP   "TCP_IP_INFO"
#define FLASH_NV_NAME_WIFI   "WIFI_INFO"
#define FLASH_NV_NAME_UPGRADE_FLAG			"UPGRADE_FLAG"
#define FLASH_NV_NAME_OTHER_TCP_IP   "OTHER_TCP_IP_INFO"

/* FlashNV key map type */
typedef struct FlashNV_KeyMap_t
{
    FlashNV_ID id;
    const char *name;
} FlashNV_KeyMap;

/* FlashNV key map table */
#define FLASH_NV_KEY_MAP_TABLE {\
    {FLASH_NV_ID_SN_PRODUCT, FLASH_NV_NAME_SN_PRODUCT},\
    {FLASH_NV_ID_SN_BOARD, FLASH_NV_NAME_SN_BOARD},\
    {FLASH_NV_ID_WORK_MODE, FLASH_NV_NAME_WORK_MODE},\
    {FLASH_NV_ID_UPGRADE_IMAGE, FLASH_NV_NAME_UPGRADE_IMAGE},\
    {FLASH_NV_ID_TROUBLECODE, FLASH_NV_NAME_TROUBLECODE},\
    {FLASH_NV_ID_WATCHDOG_STAT, FLASH_NV_NAME_WATCHDOG_STAT},\
    {FLASH_NV_ID_WAVA_CONFIG, FLASH_NV_NAME_WAVA_CONFIG},\
    {FLASH_NV_ID_CALIB_DATA, FLASH_NV_NAME_FRE_CALIB_DATA},\
    {FLASH_NV_ID_ANGLE_CALIB_DATA, FLASH_NV_NAME_ANGLE_CALIB_DATA},\
    {FLASH_NV_ID_VEHICLE_PARAMS, FLASH_NV_NAME_VEHICLE_PARAMS},\
    {FLASH_NV_ID_VEHICLE_SIGNAL_INFO, FLASH_NV_NAME_VEHICLE_SIGNAL_INFO},\
    {FLASH_NV_ID_WARNINGCFG, FLASH_NV_NAME_WARNINGCFG},\
	{FLASH_NV_ID_LOGGING_INDEX, FLASH_NV_NAME_LOGGING_INDEX},\
    {FLASH_NV_ID_CALIB_DATA_1, FLASH_NV_NAME_FRE_CALIB_DATA_1},\
    {FLASH_NV_ID_CALIB_DATA_2, FLASH_NV_NAME_FRE_CALIB_DATA_2},\
    {FLASH_NV_ID_CALIB_DATA_3, FLASH_NV_NAME_FRE_CALIB_DATA_3},\
    {FLASH_NV_ID_MAG_DATA, FLASH_NV_NAME_MAG_CALIB_DATA},\
    {FLASH_NV_ID_ACC_DATA, FLASH_NV_NAME_ACC_CALIB_DATA},\
    {FLASH_NV_ID_WIFI_TCPIP_INFO, FLASH_NV_NAME_WIFI_TCPIP_INFO},\
	{FLASH_NV_ID_SCREEN_C2NETWORK_SWITCH_FLAG, FLASH_NV_NAME_SCREEN_C2NETWORK_SWITCH_FLAG},\
	{FLASH_NV_ID_TCP_IP_INFO, FLASH_NV_NAME_TCP_IP},\
	{FLASH_NV_ID_WIFI_INFO, FLASH_NV_NAME_WIFI},\
	{FLASH_NV_ID_UPGRADE_FLAG, FLASH_NV_NAME_UPGRADE_FLAG},\
	{FLASH_NV_ID_OTHER_TCP_IP_INFO, FLASH_NV_NAME_OTHER_TCP_IP}\
}

/* FlashRaw items type */
typedef struct FlashRaw_Item_t
{
    char *key;
    uint32_t addr;
    uint32_t len;
} FlashRaw_Item;

/* FlashNV raw data address & length list (FlashRaw items data) */
#define FLASH_RAW_ITEM_TABLE {\
    {FLASH_NV_NAME_UPGRADE_IMAGE, FLASH_UPGADE_IMAGE_BASE, FLASH_UPGADE_IMAGE_SIZE}\
}

/* FlashNV types list */
/* FlashNV type of product serial number */
typedef struct FlashNV_ProductSN_t
{
    uint32_t len;
    uint8_t val[32]; // actual 22 bytes, others reserved
} FlashNV_ProductSN;

/* FlashNV type of board serial number */
typedef struct FlashNV_BoardSN_t
{
    uint32_t len;
    uint8_t val[32]; // actual 20 bytes, others reserved
} FlashNV_BoardSN;

/* FlashNV type of work mode */
typedef struct FlashNV_WorkMode_t
{
    uint32_t val;
} FlashNV_WorkMode;

/* FlashNV type of DTC */
#define FLASH_NV_ERROR_CODE_CNT_MAX (32) // ref DTC_STORED_CNT_MAX
#define FLASH_NV_ERROR_CODE_WRITE_LIMIT_MAX (100) // DTC maximum write count limit, ref DTC_STORED_TIMES_MAX

typedef struct FlashNV_ErrorCodeItem_t
{
    uint32_t dtc;
    uint8_t  times;
} FlashNV_ErrorCodeItem;

typedef struct FlashNV_ErrorCode_t
{
    FlashNV_ErrorCodeItem val[FLASH_NV_ERROR_CODE_CNT_MAX];
} FlashNV_ErrorCode;

/**
 *  @b Description
 *  @n
 *      Init FlashNV module. None of the FlashNV API's can be used without invoking this API.
 *
 *  @retval
 *      Success    - 0
 *      Fail       < -1
 */
int32_t FlashNV_Init(void);

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
int32_t FlashNV_Set(FlashNV_ID id, const void *value_buf, size_t buf_len);

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
size_t FlashNV_Get(FlashNV_ID id, void *value_buf, size_t buf_len, size_t *saved_value_len);

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
int32_t FlashNV_Del(FlashNV_ID id);

#endif /* FLASH_NV_H */
