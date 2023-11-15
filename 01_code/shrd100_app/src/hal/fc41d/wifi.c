
#include "wifi.h"
#include "../../drv/fc41d/bt_wifi.h"
#include "../../srv/flash_nv/flash_nv.h"
#include "../../srv/alink/alink_msg_proc.h"
#include "../../srv/ring/ring.h"


//#define UART_BUFFER_SIZE 					256
#define UART_BUFFER_SIZE 					1024
//#define BT_DATA_TEMP_LEN 					256

#define BT_SEND_DATA_MAX_LEN_PER 			20
#define BT_SEND_DATA_START_POSITION 		22
#define WIFI_HEADER_LENGHT					14
#define TCPIP_HEADER_LENGHT					19
#define GET_TCPIP_CHANNEL_IP_PORT_LENGHT	7
#define LOCAL_TCPIP_PORT					2022


#define IN_PASSTHROUGH						1
#define EXIT_PASSTHROUGH					0

#define ALINK_MSG_SEAMPHORE_DELY	10000


void hex_to_asciistring(uint8_t *hexData, uint32_t size, uint8_t *getStr)
{
	uint8_t deposit [2];
	uint16_t i=0;
	uint8_t j = 0;

	if (hexData == NULL || getStr == NULL)
	{
		return ;
	}

	for(i=0; i<size; i++)
	{

		deposit[1] = hexData[i] & 0x0F;
		deposit[0] = (hexData[i] &0xF0) >> 4;
		for(j = 0; j < 2; j++)
		{
			switch(deposit[j])
			{
			case 0x00:
				getStr[i*2+j]='0';
				break;
			case 0x01:
				getStr[i*2+j]='1';
				break;
			case 0x02:
				getStr[i*2+j]='2';
				break;
			case 0x03:
				getStr[i*2+j]='3';
				break;
			case 0x04:
				getStr[i*2+j]='4';
				break;
			case 0x05:
				getStr[i*2+j]='5';
				break;
			case 0x06:
				getStr[i*2+j]='6';
				break;
			case 0x07:
				getStr[i*2+j]='7';
				break;
			case 0x08:
				getStr[i*2+j]='8';
				break;
			case 0x09:
				getStr[i*2+j]='9';
				break;
			case 0x0A:
				getStr[i*2+j]='A';
				break;
			case 0x0B:
				getStr[i*2+j]='B';
				break;
			case 0x0C:
				getStr[i*2+j]='C';
				break;
			case 0x0D:
				getStr[i*2+j]='D';
				break;
			case 0x0E:
				getStr[i*2+j]='E';
				break;
			case 0x0F:
				getStr[i*2+j]='F';
				break;
			default:
				return ;
			}

		}
	}
}
int8_t GetCurrentTcpipInfo(uint8_t *ip, uint8_t ipLen, uint8_t *srcArray, uint8_t splitNumer)
{
	char *ptr = NULL;
	char *needPtr = NULL;
	char *retPtr = NULL;
	uint8_t i;

	if (ip == NULL || srcArray == NULL)
	{
		return -1;
	}

	ptr = strtok((char*)srcArray, ",");
	for(i = 0; i < splitNumer; i++)
	{
		if (ptr == NULL)
		{
			break;
		}
		ptr = strtok(NULL, ",");
	}


	if (i != splitNumer || ptr == NULL)
	{
		return -2;
	}

	needPtr = &ptr[1];
	retPtr = strtok(needPtr, ".");
	for(i = 0; i < ipLen; i++)
	{
		if (retPtr != NULL)
		{
//			ip[i] = atoi((char*)retPtr);
			ip[i] = strtol((char*)retPtr, NULL, 10);

		}
		else
		{
			break;
		}
		retPtr = strtok(NULL, ".");
	}

	if (i == ipLen)
	{
		return 0; // success
	}
	else
	{
		return -2;
	}
}


int8_t GetCurrentTcpPortInfo(uint8_t *port, uint8_t *srcArray)
{
	char *ptr = NULL;
	uint8_t i;
	uint16_t tempPort;

	if (port == NULL || srcArray == NULL)
	{
		return -1;
	}

	ptr = strtok((char*)srcArray, ",");

	for(i = 0; i < 3; i++)
	{
		if (ptr == NULL)
		{
			break;
		}
		ptr = strtok(NULL, ",");
	}


	if (i != 3 || ptr == NULL)
	{
		return -2;
	}

//	tempPort = atoi(ptr);
	tempPort = strtol(ptr, NULL, 10);

	port[0] = tempPort & 0xff;
	port[1] = (tempPort >> 8) & 0xff;

	return 0; // success

}


int8_t GetBoardSn(uint8_t *buf)
{
	int64_t retLent;
	uint8_t ret = 0;

	if (buf == NULL)
	{
		return -1;
	}

	ret = FlashNV_Get(FLASH_NV_ID_SN_BOARD, buf, BOARD_SN_LENGTH, (size_t*)&retLent);
	if (ret == 0)
	{
		return -2;
	}

	return 0; // success
}


int8_t decodTcpipAndPort(alink_ans_t *srcBuf, tcpipInfo_t *getBuf)
{
	uint8_t i = 0;
	if (srcBuf == NULL || getBuf == NULL)
	{
		return -1;
	}

	if (srcBuf->msg_len != GET_TCPIP_CHANNEL_IP_PORT_LENGHT)
	{
		return -2;
	}
	for (i = 0; i < IP_LENGTH; i++)
	{
		getBuf->ip[i] = srcBuf->buffer[i];
	}

	for(i = 0; i < PORT_LENGTH; i++)
	{
		getBuf->port[i] = srcBuf->buffer[IP_LENGTH + i];
	}
	getBuf->proxy = srcBuf->buffer[IP_LENGTH + PORT_LENGTH];

	return 0;
}



