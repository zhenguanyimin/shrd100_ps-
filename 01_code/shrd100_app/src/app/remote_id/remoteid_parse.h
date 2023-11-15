#ifndef REMOTEID_PARSE_H
#define REMOTEID_PARSE_H

#include <stdint.h>
#include <stdbool.h>


typedef struct remotid_msg_head
{
    uint8_t data0;    ///< protocol magic marker
    uint8_t data1;   ///< Low byte of length of payload
    uint8_t data2;   ///< Hight byte of length of payload
    uint8_t data3;      ///< Sequence of packet
    uint8_t data4;   ///< ID of message destination
    uint8_t data5; ///< ID of the message sounrce
    uint8_t data6;    ///< ID of message in payload
    uint8_t data7;      ///< Whether the command requires an answer 0:no  1:yes
    uint8_t data8; ///< Checksum of message header
    uint8_t data9; ///< Checksum of message header
} remotid_msg_head_t;



typedef struct RemoteID_ParseInfo
{
	uint32_t PilotLongitude;
    uint32_t PilotLatitude;
    uint32_t UavLongitude;
    uint32_t UavLatitude;
    uint16_t UavAtmosAltitude;
    uint16_t UavEarthAltitude;
    uint16_t UavHeightAgl;
    uint16_t UavSpeedMultip;
    uint8_t UavDirection;
    uint8_t UavAbsolSpeed;
    uint8_t UavVertSpeed;
    uint8_t DeviceType;
    uint32_t UavRecordTime;
    uint8_t UavSnCode[20];
}RemoteID_ParseInfo_t;


typedef struct RemoteID_OutputInfoList
{
	uint32_t RemoteIdNum;
	RemoteID_ParseInfo_t RemoteIdOutputInfoList[10];
}RemoteID_OutputInfoList_t;


typedef enum RemoteID_DeviceType
{
	LITE     = 0x01,
	PARROT   = 0x02,
}RemoteID_DeviceType_t;


RemoteID_OutputInfoList_t* RemoteID_GetOutputListInfo(void);
int RemoteID_BeaconFreamProcess(uint8_t* RevBuff, uint32_t len);
bool RemoteIDMsgProc_IsHeaderValid(remotid_msg_head_t *header);
void RemoteId_UpdateOutputList(void);

#endif

