/*
 * service_getSelfTestInfo.c
 *
 *  Created on: 2023年4月10日
 *      Author: A21001
 */
#include <string.h>
#include "service_manager.h"
#include "service_getSelfTestInfo.h"
#include "../../system_monitor/system_monitor.h"
#include "../../../app/devInfo_process/devInfo_process.h"
#include "../../../hal/hal.h"
#include "../../../cfg/version.h"
#include "../../../app/sys_status_data/detection_param.h"
#define TEMP_OFFSET 10000

static void Service_GetSelftestInfo_test(service_SelfTestInfo_t *pSelfTestInfo)
{
	float fTemp =0.0;

	pSelfTestInfo->Version = 0x0000;

	fTemp = 11.11;
	fTemp = fTemp *100.0;
	pSelfTestInfo->TempFpga = TEMP_OFFSET + (uint16_t)(fTemp);

	fTemp = 22.22;
	fTemp = fTemp *100.0;
	pSelfTestInfo->TempBatBoard1 = TEMP_OFFSET + (uint16_t)(fTemp );

	fTemp = 33.33;
	fTemp = fTemp *100.0;
	pSelfTestInfo->TempBatBoard2 = TEMP_OFFSET + (uint16_t)(fTemp);

	pSelfTestInfo->BatteryCap = 100;

	pSelfTestInfo->WorkMode = 2 ;
	pSelfTestInfo->DigitComStatus = 1 ;
	pSelfTestInfo->FanRateStatus = 1;
	pSelfTestInfo->FanRate = axi_read_data(PL_I_FPGA_TEMP1);
	pSelfTestInfo->TcDevStatus = 1 ;
	pSelfTestInfo->TcConnectStatus = 0x00 ;
	pSelfTestInfo->TcFrequence = 0x00 ;
	pSelfTestInfo->RN440DevStatus = 1 ;
	pSelfTestInfo->RN440ConnectStatus = 0x00 ;
	pSelfTestInfo->RG450DevStatus = 0x00 ;
	pSelfTestInfo->RG450ConnectStatus = 1 ;

}

static void Service_GetSelftestInfo(service_SelfTestInfo_t *pSelfTestInfo)
{
	float fTemp =0.0;

	pSelfTestInfo->Version = 0x0000;

	fTemp = GetTempFpga();
	fTemp = fTemp *100.0;
	pSelfTestInfo->TempFpga = TEMP_OFFSET + (uint16_t)(fTemp);

	fTemp = GetTempBatBoard1();
	fTemp = fTemp *100.0;
	pSelfTestInfo->TempBatBoard1 = TEMP_OFFSET + (uint16_t)(fTemp);

	fTemp = GetTempBatBoard1();
	fTemp = fTemp *100.0;
	pSelfTestInfo->TempBatBoard2 = TEMP_OFFSET + (uint16_t)(fTemp);

	pSelfTestInfo->BatteryCap = GetBatCap();

	pSelfTestInfo->WorkMode = SYS_GetWorkMode() ;
	pSelfTestInfo->DigitComStatus = GetGyroStatus() ;
	pSelfTestInfo->FanRateStatus = GetFanStatus();
	pSelfTestInfo->FanRate = axi_read_data(PL_I_FPGA_TEMP1);
	pSelfTestInfo->TcDevStatus = 0x00 ;
	pSelfTestInfo->TcConnectStatus = 0x00 ;
	pSelfTestInfo->TcFrequence = 0x00 ;
	pSelfTestInfo->RN440DevStatus = 0x00 ;
	pSelfTestInfo->RN440ConnectStatus = 0x00 ;
	pSelfTestInfo->RG450DevStatus = 0x00 ;
	pSelfTestInfo->RG450ConnectStatus = 0x00 ;

}

uint16_t Service_Get_Selftest_Info(alink_msg_t *request_msg, alink_msg_t *response_msg)
{
	uint16_t retLen = 0;

	service_SelfTestInfo_t lSelfTestInfo = {0};

	retLen = sizeof(service_SelfTestInfo_t);

	Service_GetSelftestInfo(&lSelfTestInfo);

	memcpy(&response_msg->buffer[0], (uint8_t*)(&lSelfTestInfo), retLen);

	return retLen;
}

uint16_t Service_PCGetSoftwareVersion_fun(alink_msg_t *request_msg, alink_msg_t *response_msg)
{
	uint8_t len;

	len = strlen(EMBED_SOFTWARE_PS_VERSION_STR);
	memcpy(&response_msg->buffer[0], EMBED_SOFTWARE_PS_VERSION_STR, strlen(EMBED_SOFTWARE_PS_VERSION_STR));

	return len;
}


__attribute__((section (".service_entry")))
const SERVICE_ENTRY Service_PCGetSoftwareVersion =
{
		.id = CMD_GET_SOFTWARE_VERSION,
		.index = 0x5555AAAA,
		.entry = Service_PCGetSoftwareVersion_fun,
};

__attribute__((section (".service_entry")))
const SERVICE_ENTRY Service_GetSelfTestInfo =
	{
		.id = CMD_PC_GET_SELFTEST_INFO,
		.index = 0x5555AAAA,
		.entry = Service_Get_Selftest_Info,
};
