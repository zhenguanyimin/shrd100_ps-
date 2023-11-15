
#include "ut_sys_ctrl.h"

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* FreeRTOS+CLI includes. */
#include "../../srv/cli/cli_if.h"
#include "../../srv/log/log.h"
#include "../../cfg/version.h"
#include "../../drv/flash/flash.h"
#include "../../drv/fc41d/bt_wifi.h"
#include "../../hal/flash_hal/flash_hal.h"
#include "../../srv/flash_nv/flash_nv.h"
#include "../../srv/protocol/protocol_common.h"
#include "../../hal/hal.h"
#include "../../app/devInfo_process/devInfo_process.h"

BaseType_t Version_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	uint32_t PL_Version_date;
	uint32_t PL_Version_time;

	PL_Version_date = axi_read_data(PL_VOERSION_VP0);
	PL_Version_time = axi_read_data(PL_VOERSION_VP1);
	LOG_PRINTF("Firmware compile time:%s %s\r\n", __DATE__, __TIME__);
	LOG_PRINTF("version:%s\r\n", EMBED_SOFTWARE_PS_VERSION_STR);
	LOG_PRINTF("PL_Version: %X %X\r\n", PL_Version_date & 0xFFFF, PL_Version_time);
	snprintf(pcWriteBuffer, xWriteBufferLen, "OK");

	return pdFALSE;
}
BaseType_t SetBoardSN_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len = 0;
	uint8_t sn_data[26] = {0};
	const char *param1 = 0;

	param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len);

	if( len <= 25 )
	{
		sn_data[0] = (uint8_t)len ;
		memcpy( &sn_data[1] , param1 , len );
		FlashNV_Set(FLASH_NV_ID_SN_BOARD, sn_data, 25+1 );
		LOG_PRINTF("sn code : %s\r\n", sn_data );
	}
	else
	{
		LOG_PRINTF("para len invalide!\r\n");
	}

	return pdFALSE;
}

BaseType_t GetBoardSN_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	size_t retLent = 0 ;
	int8_t ret = 0;
	uint8_t getBoardSN[26] = {0};

	ret = FlashNV_Get(FLASH_NV_ID_SN_BOARD, getBoardSN, 26, &retLent);
	if (ret == 0)
	{
		LOG_PRINTF("no sn code in flash!\r\n");
	}
	else
	{
		LOG_PRINTF("sn code : %s\r\n", &getBoardSN[1] );
	}

	return pdFALSE;
}

static BaseType_t SetTcpIpInfo(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len = 0 , valid_len = 0x00;
	const char *param1 = 0;

	uint8_t l_tcp_ip[256] = {0};

	param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len);

	for( valid_len = 0x00 ; valid_len < len ; valid_len++ )
	{
		if( ( param1[valid_len] == '\n' ) || ( param1[valid_len] == '\r' ) )
		{
			break;
		}
	}

	memcpy( l_tcp_ip , param1 , valid_len );
	l_tcp_ip[valid_len++] = '\r';
	l_tcp_ip[valid_len] = '\n';

	FlashNV_Set( FLASH_NV_ID_TCP_IP_INFO , (void *)l_tcp_ip, sizeof(l_tcp_ip) );
//	memset( l_tcp_ip , 0x00 , sizeof(AT_QIOPEN_) );
//	FlashNV_Get( FLASH_NV_ID_TCP_IP_INFO , (void *)l_tcp_ip, sizeof(l_tcp_ip) , &len );
	memcpy( AT_QIOPEN_ , l_tcp_ip , sizeof(l_tcp_ip) );
	snprintf(pcWriteBuffer, xWriteBufferLen, "\r\n AT_QIOPEN_[] = %s ;\r\n" , AT_QIOPEN_ );

	return pdFALSE;
}

static BaseType_t SetOtherTcpIpInfo(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len = 0 , valid_len = 0x00;
	const char *param1 = 0;

	uint8_t l_tcp_ip[256] = {0};

	param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len);

	for( valid_len = 0x00 ; valid_len < len ; valid_len++ )
	{
		if( ( param1[valid_len] == '\n' ) || ( param1[valid_len] == '\r' ) )
		{
			break;
		}
	}

	memcpy( l_tcp_ip , param1 , valid_len );
	l_tcp_ip[valid_len++] = '\r';
	l_tcp_ip[valid_len] = '\n';

	FlashNV_Set( FLASH_NV_ID_OTHER_TCP_IP_INFO , (void *)l_tcp_ip, sizeof(l_tcp_ip) );
//	memset( l_tcp_ip , 0x00 , sizeof(AT_QIOPEN_) );
//	FlashNV_Get( FLASH_NV_ID_TCP_IP_INFO , (void *)l_tcp_ip, sizeof(l_tcp_ip) , &len );
	memcpy( AT_QIOPEN_OTHER_ , l_tcp_ip , sizeof(l_tcp_ip) );
	snprintf(pcWriteBuffer, xWriteBufferLen, "\r\n AT_QIOPEN_[] = %s ;\r\n" , AT_QIOPEN_OTHER_ );

	return pdFALSE;
}
static BaseType_t SetWifiInfo(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len = 0 , valid_len = 0x00;
	const char *param1 = 0;

	uint8_t l_wifi_info[256] = {0};

	param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len);

	for( valid_len = 0x00 ; valid_len < len ; valid_len++ )
	{
		if( ( param1[valid_len] == '\n' ) || ( param1[valid_len] == '\r' ) )
		{
			break;
		}
	}

	memcpy( l_wifi_info , param1 , valid_len );
	l_wifi_info[valid_len++] = '\r';
	l_wifi_info[valid_len] = '\n';

	FlashNV_Set( FLASH_NV_ID_WIFI_INFO , (void *)l_wifi_info, sizeof(l_wifi_info) );
//	memset( l_wifi_info , 0x00 , sizeof(AT_QSTAAPINFO_) );
//	FlashNV_Get( FLASH_NV_ID_WIFI_INFO , (void *)l_wifi_info, sizeof(l_wifi_info) , &len );
	memcpy( AT_QSTAAPINFO_ , l_wifi_info , sizeof(l_wifi_info) );
	snprintf(pcWriteBuffer, xWriteBufferLen, "\r\n AT_QSTAAPINFO_[] = %s ;\r\n" , AT_QSTAAPINFO_ );

	return pdFALSE;
}

static BaseType_t GetGyroInfo(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{

	float roll;
	float pitch;
	float yaw;

	GetAngleInfo(&roll,  &pitch,  &yaw);

	LOG_PRINTF("GyroInfo: roll = %f , pitch = %f ,yaw = %f \r\n", roll,  pitch,  yaw);
	snprintf(pcWriteBuffer, xWriteBufferLen, "OK");

	return pdFALSE;
}


static const CLI_Command_Definition_t Version_cmd =
	{
		"Version",
		"\r\nVersion\r\n  \r\n",
		Version_handler,
		0};

static const CLI_Command_Definition_t SetBoardSN_cmd =
	{
		"SetBoardSN",
		"\r\nSetBoardSN\r\n  \r\n",
		SetBoardSN_handler,
		1};

static const CLI_Command_Definition_t GetBoardSN_cmd =
	{
		"GetBoardSN",
		"\r\nGetBoardSN\r\n  \r\n",
		GetBoardSN_handler,
		0};
static const CLI_Command_Definition_t SetTcpIpInfo_cmd =
	{
		"SetTcpIpInfo",
		"\r\nSetTcpIpInfo  <AT+QIOPEN=1,\"TCP\",\"192.168.82.155\",8060,2,2> \r\n",
		SetTcpIpInfo,
		1};

static const CLI_Command_Definition_t SetOtherTcpIpInfo_cmd =
	{
		"SetOtherTcpIpInfo",
		"\r\nSetOtherTcpIpInfo  <AT+QIOPEN=1,\"TCP\",\"192.168.82.155\",8060,2,2> \r\n",
		SetOtherTcpIpInfo,
		1};

static const CLI_Command_Definition_t SetWifiInfo_cmd =
	{
		"SetWifiInfo",
		"\r\nSetWifiInfo  <AT+QSTAAPINFO=Autel_drone,12345678> \r\n",
		SetWifiInfo,
		1};

static const CLI_Command_Definition_t GetGyroInfo_cmd =
	{
		"GetGyroInfo",
		"\r\nGetGyroInfo\r\n   \r\n",
		GetGyroInfo,
		0};

void ut_dev_info_init(void)
{

	FreeRTOS_CLIRegisterCommand(&Version_cmd);
	FreeRTOS_CLIRegisterCommand(&SetOtherTcpIpInfo_cmd);
	FreeRTOS_CLIRegisterCommand(&SetTcpIpInfo_cmd);
	FreeRTOS_CLIRegisterCommand(&SetWifiInfo_cmd);
	FreeRTOS_CLIRegisterCommand(&GetGyroInfo_cmd);

    return;
}
