/*
 * service_getSelfTestInfo.h
 *
 *  Created on: 2023年4月10日
 *      Author: A21001
 */

#ifndef SRC_SRV_LINK_SRV_SERVERS_SERVICE_GETSELFTESTINFO_H_
#define SRC_SRV_LINK_SRV_SERVERS_SERVICE_GETSELFTESTINFO_H_
#pragma pack(1)
typedef struct service_SelfTestInfo
{

	uint16_t Version;
	uint16_t TempFpga;
	uint16_t TempBatBoard1;
	uint16_t TempBatBoard2;
	uint16_t BatteryCap;
	uint8_t WorkMode;
	uint8_t DigitComStatus;
	uint8_t FanRateStatus;
	uint8_t FanRate;
	uint8_t TcDevStatus;
	uint8_t TcConnectStatus;
	uint8_t TcFrequence;
	uint8_t RN440DevStatus;
	uint8_t RN440ConnectStatus;
	uint8_t RG450DevStatus;
	uint8_t RG450ConnectStatus;

} service_SelfTestInfo_t;
#pragma pack()
#endif /* SRC_SRV_LINK_SRV_SERVERS_SERVICE_GETSELFTESTINFO_H_ */
