#include "../remote_id/remoteid_parse.h"
#include <stdint.h>
#include <string.h>
#include "lwip/def.h"
#include "../../srv/log/log.h"
#include "FreeRTOS.h"
#include "task.h"


#define  LONGI_LATI_SCALE    10000000
#define  MAX_RECORD_REMOTEID_NUM  10
#define  MAX_KEEP_REMOTEID_TIME   8000
static RemoteID_ParseInfo_t RemoteIdParseInfo[MAX_RECORD_REMOTEID_NUM] = {0};
static RemoteID_OutputInfoList_t RemoteIdOutput = {0};
extern void RemoteIdSeamphoreTake(void);
extern void RemoteIdSeamphoreGive(void);

RemoteID_OutputInfoList_t* RemoteID_GetOutputListInfo(void)
{
	return &RemoteIdOutput;
}


void RemoteId_UpdateOutputList(void)
{
	int i;
	uint32_t time = 0;

	RemoteIdSeamphoreTake();
	memset(&RemoteIdOutput, 0, sizeof(RemoteID_OutputInfoList_t));
	for (i = 0; i < MAX_RECORD_REMOTEID_NUM; i++)
	{
		if (RemoteIdParseInfo[i].DeviceType != 0x0)
		{
			time = xTaskGetTickCount() - RemoteIdParseInfo[i].UavRecordTime;
			if (time < MAX_KEEP_REMOTEID_TIME)
			{
				memcpy(&RemoteIdOutput.RemoteIdOutputInfoList[RemoteIdOutput.RemoteIdNum],  &RemoteIdParseInfo[i], sizeof(RemoteID_ParseInfo_t));
				RemoteIdOutput.RemoteIdNum++;
			}
			else
			{
				memset(&RemoteIdParseInfo[i], 0, sizeof(RemoteID_ParseInfo_t));
			}
		}
	}
	RemoteIdSeamphoreGive();
}


static int RemoteID_ParseUavMesssgeInfo(uint8_t* RevBuff, uint16_t len, RemoteID_ParseInfo_t* RemoteIdInfo)
{
	RemoteIdInfo->UavSpeedMultip = (*(RevBuff + 1)) & 0x01;
	RemoteIdInfo->UavDirection = *(RevBuff + 2);
	RemoteIdInfo->UavAbsolSpeed = *(RevBuff + 3);
	RemoteIdInfo->UavVertSpeed = *(RevBuff + 4);
	RemoteIdInfo->UavLatitude = *((uint32_t*)(RevBuff + 5));
	RemoteIdInfo->UavLongitude = *((uint32_t*)(RevBuff + 9));
	RemoteIdInfo->UavAtmosAltitude = *(uint16_t*)(RevBuff + 13);
	RemoteIdInfo->UavEarthAltitude = *(uint16_t*)(RevBuff + 15);
	RemoteIdInfo->UavHeightAgl =*(uint16_t*)(RevBuff + 17);
	RemoteIdInfo->UavRecordTime = xTaskGetTickCount();

	LOG_DEBUG("Multip:%d   Dire:%d  bsolSpe:%d VertSp:%d Lati:%d Long:%d Atmos:%d Earth:%d Heig:%d \r\n",\
			RemoteIdInfo->UavSpeedMultip, RemoteIdInfo->UavDirection, RemoteIdInfo->UavAbsolSpeed,RemoteIdInfo->UavVertSpeed,RemoteIdInfo->UavLatitude,
			RemoteIdInfo->UavLongitude,RemoteIdInfo->UavAtmosAltitude,RemoteIdInfo->UavEarthAltitude,RemoteIdInfo->UavHeightAgl);

    return 0;
}


static int RemoteID_ParseSysMesssgeInfo(uint8_t* RevBuff, uint16_t len, RemoteID_ParseInfo_t* RemoteIdInfo)
{
	RemoteIdInfo->PilotLatitude = *((uint32_t*)(RevBuff + 2));
	RemoteIdInfo->PilotLongitude = *((uint32_t*)(RevBuff + 6));
	RemoteIdInfo->UavRecordTime = xTaskGetTickCount();

	LOG_DEBUG("PiLat:%d   PiLon:%d\r\n", RemoteIdInfo->PilotLatitude, RemoteIdInfo->PilotLongitude);

    return 0;
}

static int RemoteID_ParseSnCodeMesssgeInfo(uint8_t* RevBuff, uint16_t len, RemoteID_ParseInfo_t* RemoteIdInfo)
{
	int i;
	memcpy(RemoteIdInfo->UavSnCode, RevBuff + 2, 20);
	RemoteIdInfo->UavRecordTime = xTaskGetTickCount();
	LOG_DEBUG("SnCode:");
	for (i = 0; i < 20; i++)
	{
		LOG_DEBUG("%c ", RemoteIdInfo->UavSnCode[i]);
	}
	LOG_DEBUG("\r\n");
    return 0;
}

int RemoteID_BeaconFreamProcess(uint8_t* RevBuff, uint32_t len)
{
    int i, j;
    int ret = 0;
    int ParseDataFlag = 0;
    uint8_t data[512] = {0};
    uint16_t RemoteLen = 0;
    uint16_t MessageLen = 0;
    uint16_t MessageNum = 0;
    uint8_t* MessageStart = NULL;
    uint8_t DeviceType = 0;
    uint8_t DataTypeFlag = 0;
    RemoteID_ParseInfo_t* remoteIDparseaddr = NULL;

    if ((*(RevBuff + 32) == 0x71) && (*(RevBuff + 33) == 0x02))
    {
    	DeviceType = LITE; // lite
    }
    else if ((*(RevBuff + 32) == 0x64) && (*(RevBuff + 33) == 0x0))
    {
    	DeviceType = PARROT; // parrot
    }
    else
    {
    	ret = -1;
    	return ret;
    }
    for (i = 0; i < MAX_RECORD_REMOTEID_NUM; i++)
    {
    	if (RemoteIdParseInfo[i].DeviceType == DeviceType)
    	{
    		remoteIDparseaddr = &RemoteIdParseInfo[i];
    		DataTypeFlag = 1;
    		break;
    	}
    }
    if (i == MAX_RECORD_REMOTEID_NUM)
    {
		for (i = 0; i < MAX_RECORD_REMOTEID_NUM; i++)
		{
			if (RemoteIdParseInfo[i].DeviceType == 0x0)
			{
				remoteIDparseaddr = &RemoteIdParseInfo[i];
				DataTypeFlag = 2;
				break;
			}
		}
    }
    if (remoteIDparseaddr == NULL)
    {
    	ret = -1;
    	return ret;
    }

    for(i = 0; i < len; i++)
    {
        if (*(RevBuff + i) == 0xDD)
        {
            memcpy(data, RevBuff + i, 9);
            if ((*(data + 5) == 0x0D) && (*(data + 7) == 0xF0) && (*(data + 8) == 0x19))
            {
                memcpy(data, RevBuff + i, len - i);
                break;
            }
        }
    }
    if (i != len)
    {
        RemoteLen = *(data + 1);
        MessageLen = *(data + 8);
        MessageNum = *(data + 9);
        if ((RemoteLen - 8) == (MessageLen * MessageNum))
        {
            MessageStart = data + 10;
            for (j = 0; j < MessageNum; j++)
            {
            	if (DataTypeFlag == 2)
            	{
            		remoteIDparseaddr->DeviceType = DeviceType;
            	}
                if (((*(MessageStart + j * MessageLen) & 0xf0) == 0x0) && ((*(MessageStart + j * MessageLen + 1) & 0xf0) == 0x10))
                {
                	RemoteID_ParseSnCodeMesssgeInfo(MessageStart + j * MessageLen, MessageLen, remoteIDparseaddr);  // get sn
                	ParseDataFlag = 1;
                }
                else if ((*(MessageStart + j * MessageLen) & 0xf0) == 0x10)
                {
                	RemoteID_ParseUavMesssgeInfo(MessageStart + j * MessageLen, MessageLen, remoteIDparseaddr);  // get uav information
                	ParseDataFlag = 1;
                }
                else if ((*(MessageStart + j * MessageLen) & 0xf0) == 0x40)
                {
                    RemoteID_ParseSysMesssgeInfo(MessageStart + j * MessageLen, MessageLen, remoteIDparseaddr);  // get pilot informaiton
                    ParseDataFlag = 1;
                    return ret;
                }
            }
            if (ParseDataFlag == 0)
			{
            	if (DataTypeFlag == 2)
            	{
            		memset(remoteIDparseaddr, 0, sizeof(RemoteID_ParseInfo_t));
            	}
				ret = -1;
			}
        }
        else
        {
        	ret = -1;
        }
    }
    else
	{
		ret = -1;
	}

    return ret;
}


bool RemoteIDMsgProc_IsHeaderValid(remotid_msg_head_t *header)
{
	bool ret = true;

	// check the magic
	if (header->data0 != 0x80)
	{
		LOG_VERBOSE("RemoteID message header data0 err!\r\n");
		ret = false;
		goto out;
	}
	if ((header->data1 != 0x0) || (header->data2 != 0x0) || (header->data3 != 0x0))
	{
		LOG_VERBOSE("RemoteID message header data1 err!\r\n");
		ret = false;
		goto out;
	}
	if ((header->data4 != 0xff) || (header->data5 != 0xff) || (header->data6 != 0xff) || (header->data7 != 0xff))
	{
		LOG_VERBOSE("RemoteID message header data4 err!\r\n");
		ret = false;
		goto out;
	}

out:
	return ret;
}
