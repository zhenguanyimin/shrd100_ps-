/*
 * app_bt_wifi.c
 *
 *  Created on: 2022年11月24日
 *      Author: A22745
 */
#include "FreeRTOS.h"
#include "task.h"
#include <stdint.h>
#include <timers.h>
#include "app_bt_wifi.h"
#include "../../cfg/shrd_config.h"
#include "../../drv/fc41d/bt_wifi.h"
#include "../../srv/heartbeat_packet/heartbeat_packet.h"
#include "../rf_config/rf_config.h"
#include <stdlib.h>
#include "../../srv/alink/alink_msg_proc.h"
//#include "../../srv/c2_srv/servers/c2_service_uploadWifiStateInfo.h"
#include "semphr.h"
#include "../../srv/log/log.h"
#include "../../drv/fc41d/bt_uart.h"
#include "../../srv/flash_nv/flash_nv.h"
#include "../../hal/fc41d/wifi.h"

uint16_t NewSourceIDFlag = FALSE ;
uint16_t g_local_ID = DEV_DRONEID ;
uint16_t g_remote_ID = DEV_C2 ;
alink_msg_t BroadcastAckMsg;

extern XScuGic xInterruptController;

#define CMD_BUFF_LEN	1024
static uint8_t	s_cmd_buffer[CMD_BUFF_LEN];

static btConfigStepState_e stepState = BT_CONFIG_STEP_STATE_PH_INIT ;

static uint32_t broadcastflag = 0x00 ;
static uint32_t protocol_version = 0x00 ;
static uint32_t C2_Heartbeat_Tick = 0x00 ;
static uint32_t C2_Tcp_Tick = 0x00 ;
static uint32_t C2_Rec_Heart_flag = 0x00;
extern socket_info_t s_tcp_socket;

int get_bt_wifi_connect_status()
{
	if( stepState == BT_STATE_CHECK)
	{
		return 1;
	}
	else
	{
		return 0;
	}

}

void update_broadcastInfo( uint32_t version )
{
	broadcastflag = 0x01;
	protocol_version = version ;
}

void update_c2_heartbeat_time()
{
	C2_Heartbeat_Tick = xTaskGetTickCount();
	C2_Rec_Heart_flag = 1;
}

uint32_t Tcp_Timeout_check()
{
	uint32_t ret = 0x00 ;

	if ( C2_Rec_Heart_flag != 0x00 )
	{
		C2_Tcp_Tick = xTaskGetTickCount();
		if( (abs(C2_Tcp_Tick - C2_Heartbeat_Tick)) > 2000 )
		{
			ret = 0x01 ;
		}
	}
	return ret ;
}

static uint8_t GetHeaderChecksum(alink_msg_head_t *header)
{
	uint8_t *pkt = (uint8_t *)header;
	uint8_t checksum = 0;
	uint8_t i = 0;

	for (i = 0; i < ALINK_CORE_HEADER_LEN; i++)
	{
		checksum += pkt[i];
	}

	return checksum;
}

void wifi_hex_to_asciistring(uint8_t *hexData, uint32_t size, uint8_t *getStr)
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

void fc41d_create_response_ble(char* at_buff,uint8_t res_result)
{
    uint16_t length = 0;
	uint8_t cnt = 0;
	uint16_t status = 0x02;
	uint16_t crc = 0;
	alink_msg_t response_ble;
	uint8_t ble_res_str[30] = {0};

	memset(response_ble.buffer, 0, 1024);

	length = 1;
    response_ble.msg_head.magic = 0xFD;
    response_ble.msg_head.len_lo = (length & 0x00FF);
	response_ble.msg_head.len_hi = (length & 0xFF00) >> 8;
	response_ble.msg_head.seq = 0;
    response_ble.msg_head.destid = DEV_C2_BLE;
    response_ble.msg_head.sourceid = DEV_DRONEID;
    response_ble.msg_head.msgid = 0xBD;
    response_ble.msg_head.ans = 0;

	response_ble.buffer[cnt] = res_result;
    cnt += 1 ;

	response_ble.msg_head.checksum = GetHeaderChecksum(&response_ble.msg_head);
	crc = crc_calculate((const uint8_t *)((&response_ble.msg_head.magic) + 1), length + 8); // 计算校验值不包含帧头，校验从字节1至字节N+8
	response_ble.buffer[cnt++] = crc & 0xFF;
	response_ble.buffer[cnt++] = (crc & 0xFF00) >> 8;
    

    memset(&ble_res_str[0],0,strlen((ble_res_str[0])));
	wifi_hex_to_asciistring((uint8_t*)(&response_ble.msg_head.magic), 12, &ble_res_str[0]);
	sprintf(at_buff,"AT+QBLEGATTSNTFY=fff2,12,%s\r\n", &ble_res_str[0]);
}

uint32_t PacketBroadcastAck(alink_msg_t *response_msg , uint8_t destid )
{
	uint16_t length = 0;
	uint8_t cnt = 0;
	uint16_t status = 0x02;
	uint16_t crc = 0;

	memset(response_msg->buffer, 0, 1024);

	length = 104;
    response_msg->msg_head.magic = 0xFD;
    response_msg->msg_head.len_lo = (length & 0x00FF);
	response_msg->msg_head.len_hi = (length & 0xFF00) >> 8;
	response_msg->msg_head.seq = 0;
    response_msg->msg_head.destid = DEV_C2;
    response_msg->msg_head.sourceid = DEV_DRONEID;
    response_msg->msg_head.msgid = 0xBB;
    response_msg->msg_head.ans = 0;

    memcpy(&response_msg->buffer[cnt], &protocol_version, sizeof(protocol_version));
    cnt += 4 ;

    memcpy(&response_msg->buffer[cnt], l_sn_code, strlen(l_sn_code));
    cnt += 32 ;

    memcpy(&response_msg->buffer[cnt], &l_device_type, sizeof(l_device_type));
    cnt += 2 ;

    memcpy(&response_msg->buffer[cnt], &status, sizeof(status));
    cnt += 2 ;

    memcpy(&response_msg->buffer[cnt], l_device_version, strlen(l_device_version));
    cnt += 64 ;
    
	response_msg->msg_head.checksum = GetHeaderChecksum(&response_msg->msg_head);
	crc = crc_calculate((const uint8_t *)((&response_msg->msg_head.magic) + 1), length + 8); // 计算校验值不包含帧头，校验从字节1至字节N+8
	response_msg->buffer[cnt++] = crc & 0xFF;
	response_msg->buffer[cnt++] = (crc & 0xFF00) >> 8;

	return ( sizeof(response_msg->msg_head) + cnt );
    //response_msg->buffer[cnt] = '\0';

}

uint32_t BroadcastAck()
{
	uint32_t ret = 0x00 ;
	uint32_t len= 0x00 ;

	ret = broadcastflag ;

	if( broadcastflag != 0x00 )
	{
		len = PacketBroadcastAck( &BroadcastAckMsg , DEV_C2 );
		for( uint32_t i = 0x00 ; i < 3 ; i++)
		{
			BtSendData_( 10 , (char*)&BroadcastAckMsg , len , "255.255.255.255" , 1810 );
		}
		broadcastflag = 0x00;
	}
	return ret ;
}

//need parse UDP data,fill into AT open command
void fc41d_parse_udp()
{
	uint32_t ret = 0x00 ;

	while( 1 )
	{
		vTaskDelay(1);
		ret =  BroadcastAck();
		if( ret != 0x00 )
		{
			break;
		}
	}
}
void PacketDeviceInfo(alink_msg_t *response_msg , uint8_t destid )
{
	uint16_t length = 0;
	uint8_t cnt = 0;

	memset(response_msg->buffer, 0, 1024);

	length = 114;
    response_msg->msg_head.magic = 0xFD;
    response_msg->msg_head.len_lo = (length & 0x00FF);
	response_msg->msg_head.len_hi = (length & 0xFF00) >> 8;
	response_msg->msg_head.seq = 0;
    response_msg->msg_head.destid = destid;
    response_msg->msg_head.sourceid = g_local_ID;
    response_msg->msg_head.msgid = 0xB3;
    response_msg->msg_head.ans = 0;

    memcpy(&response_msg->buffer[cnt], EMBED_SN_CODE, strlen(EMBED_SN_CODE));
    cnt += 32 ;

    memcpy(&response_msg->buffer[cnt], EMBED_COMPANY_NAME, strlen(EMBED_COMPANY_NAME));
    cnt += 32 ;

    memcpy(&response_msg->buffer[cnt], EMBED_DEVICE_NAME, strlen(EMBED_DEVICE_NAME));
    cnt += 32 ;

    response_msg->buffer[cnt++] = ( ( EMBED_DEVICE_TYPE>>0 ) & 0xff );
    response_msg->buffer[cnt++] = ( ( EMBED_DEVICE_TYPE>>8 ) & 0xff );

    memcpy(&response_msg->buffer[cnt], EMBED_DEVICE_VERSION, strlen(EMBED_DEVICE_VERSION));
    cnt += 16 ;

    response_msg->buffer[cnt] = '\0';

}

int32_t fc41d_parse_reply_chain()
{
	int32_t ret = BT_WIFI_OK;

	alink_msg_t mTeartbeatPacketMsg;


	PacketDeviceInfo( &mTeartbeatPacketMsg , g_remote_ID );

	while( NewSourceIDFlag != TRUE )
	{
		wifi_send_data( (char*)&mTeartbeatPacketMsg , sizeof(mTeartbeatPacketMsg) );
		vTaskDelay(3000);
		ret = BT_WIFI_OK;
	}

	return ret ;

}

static uint8_t BTWifiMsgProc_GetHeaderChecksum(bt_wifi_msg_head_t *header)
{
	uint8_t *pkt = (uint8_t *)header;
	uint8_t checksum = 0;
	uint8_t i = 0;

	for (i = 0; i < ALINK_CORE_HEADER_LEN; i++)
	{
		checksum += pkt[i];
	}

	return checksum;
}

static uint8_t BtWifiMsgProc_IsHeaderValid(bt_wifi_msg_head_t *header)
{
	uint8_t ret = true;
	uint32_t len_msg_payload = 0;
	uint8_t checksum = 0;

	// check the magic
	if (header->magic != BT_WIFI_MSG_MAGIC_VAL)
	{
		ret = false;
		goto out;
	}

	// check the length
	len_msg_payload = (header->len_hi << 8) + header->len_lo;
	if (len_msg_payload > BT_WIFI_MAX_PAYLOAD_LEN)
	{
		ret = false;
		goto out;
	}

	// check the destid
	if ((header->destid & 0xf) != DEV_DRONEID)
	{
		ret = false;
		goto out;
	}

	// check the checksum
	checksum = BTWifiMsgProc_GetHeaderChecksum(header);
	if (header->checksum != checksum)
	{
		ret = false;
		goto out;
	}

	out:
	return ret;
}

void malink_cmd_check( alink_msg_t *sAlinkRecv_msg , uint32_t index )
{


}

void fc41d_cmd_process( )
{
	uint16_t crc;
	uint32_t uart_data_len = 0x00 ;
	uint32_t msg_data_len = 0x00 ;
	uint32_t index = 0x00 ;
	alink_msg_t *sAlinkRecv_msg = (alink_msg_t *)s_cmd_buffer;

	uint32_t uRecvHeadCnt = 0;

	memset( s_cmd_buffer , 0x00 , sizeof(s_cmd_buffer) );
	while(1)
	{
		uart_data_len = output_wifi_uart_data( &s_cmd_buffer[index] , CMD_BUFF_LEN-index );

		if( uart_data_len <= 0x00 )
		{
			vTaskDelay(1);
		}

		index += uart_data_len;

		if( index >= sizeof(alink_msg_head_t) )
		{
			if( uRecvHeadCnt < sizeof(alink_msg_head_t) )
			{
				msg_data_len = ((sAlinkRecv_msg->msg_head.len_hi << 8) + sAlinkRecv_msg->msg_head.len_lo);

				uRecvHeadCnt = sizeof(alink_msg_head_t);

				if (!AlinkMsgProc_IsHeaderValid(&sAlinkRecv_msg->msg_head))
				{
					break;
				}
			}

			if (index >= ( uRecvHeadCnt + msg_data_len + 2 ) )
			{
				crc = (sAlinkRecv_msg->buffer[msg_data_len + 1] << 8) + sAlinkRecv_msg->buffer[msg_data_len];
				if (AlinkMsg_IsCrcValid((uint8_t *)sAlinkRecv_msg, msg_data_len + uRecvHeadCnt + ALINK_NUM_CHECKSUM_BYTES, crc))
				{
					AlinkMsgProcTask_Post((uint32_t)sAlinkRecv_msg);
					SetAlinkInterface(WIFI);
				}
				break;
			}

		}

		if( ( index >= CMD_BUFF_LEN ) || ( s_cmd_buffer[index-1] == '\n' ) )
		{
			break;
		}

	}
}

void BtDevManage_Task(void *p_arg)
{

	int32_t ret = BT_WIFI_OK;

	wifi_Setup_Intr_System( &xInterruptController );

	while(1)
	{
		switch(stepState)
		{
			case BT_CONFIG_STEP_STATE_PH_INIT:
			{
				ret = fc41d_ble_peripherals_init();
				if( ret != BT_WIFI_OK )
				{
					break;
				}
				stepState = BT_CONFIG_STEP_REC_WIFIINFO ;
				break;
			}
			case BT_CONFIG_STEP_REC_WIFIINFO:
			{
				ret = wifi_info_status();
				if( ret == 0x00 )
				{
					vTaskDelay(1);
					break;
				}
				else
				{
					stepState = BT_CONFIG_STEP_STATE_CONNECT_WIFI ;
					break;
				}
			}
			case BT_CONFIG_STEP_STATE_CONNECT_WIFI:
			{
				ret = fc41d_wifi_connect();
				if( ret != BT_WIFI_OK )
				{
					break;
				}
				stepState = BT_CONFIG_STEP_STATE_CONNECT_UDP ;
				break;
			}
			case BT_CONFIG_STEP_STATE_CONNECT_UDP:
			{
				fc41d_udp_connect();
				fc41d_parse_udp();
				//fc41d_udp_socket_close();

				if(s_tcp_socket.update == 1)
				{
					s_tcp_socket.update = 0;
					stepState = BT_CONFIG_STEP_STATE_CONNECT_TCP ;
				}
				else
				{
					stepState = BT_CONFIG_STEP_STATE_CONNECT_WIFI ;
				}
				break;
			}
			case BT_CONFIG_STEP_STATE_CONNECT_TCP:
			{
				ret = fc41d_tcp_connect();
				if( ret != BT_WIFI_OK )
				{
					//fc41d_tcp_socket_close();
					stepState = BT_CONFIG_STEP_STATE_CONNECT_WIFI ;
					break;
				}
				stepState = BT_STATE_CHECK ;
				break;
			}
			case BT_STATE_CHECK:
			{
				ret = Tcp_Timeout_check();
				if( ret == 0x01 )
				{
					fc41d_tcp_socket_close();
					C2_Rec_Heart_flag = 0;
					stepState = BT_CONFIG_STEP_STATE_CONNECT_TCP ;
				}
				vTaskDelay(100);
                break;

			}
			default:
			{

				break;
			}
		}
	}



}
void BtDataProcess_Task(void *p_arg)
{
	uint8_t  buff[10] = {0} ;

	init_fc41d_uart();
	init_fc41d_ack_buff();

	while(1)
	{

		if( wifi_uart_buff_empty() )
		{
			vTaskDelay(1);
			memset( buff , 0x00 , sizeof( buff ) );
		}
		else
		{
			get_wifi_uart_data( 0 , &buff[0] , sizeof(char) );
			if( buff[0] == 0xfd  )
			{
				fc41d_cmd_process( );
			}
			else
			{
				fc41d_ack_process( );
			}
		}
	}
}
int32_t InitBtWifiProcess_Task(void)
{
	int32_t retVal = -1;


	if (xTaskCreate(BtDataProcess_Task, "BtDataProcess_Task", TASK_STACK_SIZE_BT_DATA_PROCESS, NULL, TASK_PRI_BT_DATA_PROCESS, NULL) == 1)
	{
		retVal = 0;
	}

	if (xTaskCreate(BtDevManage_Task, "BtDevManage_Task", TASK_STACK_SIZE_BT_DEV_MANAGE, NULL, TASK_PRI_BT_DEV_MANAGE, NULL) == 1)
	{
		retVal = 0;
	}

	return retVal;
}

