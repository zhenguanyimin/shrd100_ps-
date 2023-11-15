/*
 * bt_wifi.c
 *
 *  Created on: 2022年11月24日
 *      Author: A22745
 */
#include "bt_wifi.h"

#include "../axiuart/sys_intr.h"
#include "FreeRTOS.h"
#include <queue.h>
#include <timers.h>
#include "../../srv/alink/alink_msg_proc.h"
#include "../../srv/log/log.h"
#include "../../srv/flash_nv/flash_nv.h"
#include "../../srv/alink/checksum.h"

#include "semphr.h"
//#include "../../srv/watch_srv/servers/service_setbluetstatus.h"
//#include "../../srv/watch_srv/servers/service_setheartbeatinfo.h"
#include "../../app/btAndWifi/app_bt_wifi.h"
#include "../uartlite/uartlite_drv.h"
#include "bt_uart.h"
#include "../../srv/ring/ring.h"


#define BT_SEND_DATA_WAIT_TIME_10S			10000
#define BT_SEND_DATA_WAIT_TIME_3S			3000
#define BT_SEND_DATA_WAIT_TIME_1S			1000
#define BT_SEND_DATA_WAIT_TIME_300MS		300
#define BT_DATA_TEMP_LEN 					1024
#define AT_QIOPEN_LEN						256
#define BT_CHECK_RETURN_DATA_IS_OK			"OK"
#define BT_CHECK_RETURN_DATA_FLAG			"recv"
#define ACK_BUFF_LEN	256


static wifi_info_t s_wifi={
		.update		= 0x00 ,
		.name		= {0} ,
		.password	= {0},
} ;

// static socket_info_t s_udp_rec_socket={
// 		.update			= 0x00 ,
// 		.socketID		= 0x00 ,
// 		.service_type	= "UDP SERVICE",
// 		.remote_addr	= "0.0.0.0",
// 		.remote_port	= 0x00,
// 		.local_port		= "1800",
// 		.state			= 0x01,

// } ;
// static socket_info_t s_udp_send_socket={
// 		.update			= 0x00 ,
// 		.socketID		= 0x01 ,
// 		.service_type	= "UDP",
// 		.remote_addr	= "0.0.0.0",
// 		.remote_port	= 1810,
// 		.local_port		= "2",
// 		.state			= 0x02,

// } ;
socket_info_t s_tcp_socket={
		.update			= 0x00 ,
		.socketID		= 0x02 ,
		.service_type	= "TCP",
		.remote_addr	= "0.0.0.0",
		.remote_port	= 8060,
		.local_port		= 0x02,
		.state			= 0x02,
} ;


static uint8_t	s_ack_buffer[ACK_BUFF_LEN]= {0x00};
static uint8_t	s_cmd_buffer[ACK_BUFF_LEN*5]= {0x00};
static ring_t	s_ack_Ring = { 0 };



char AT_QURCCFG[] = "AT+QURCCFG=1\r\n"; // 打开URC主动上报
char AT_QSTASTOP[] = "AT+QSTASTOP\r\n"; // 禁用Station模式
char AT_QBLEINIT[] = "AT+QBLEINIT=2\r\n"; // 配置模块为外围设备并进行BLE初始化
char AT_QBLENAME[] = "AT+QBLENAME=drone id\r\n"; // 设置蓝牙名称
char AT_QBLEGATTSSRV[] = "AT+QBLEGATTSSRV=fff1\r\n"; // 创建BLE服务并设置服务UUID为fff1
char AT_QBLEGATTSCHAR_1[] = "AT+QBLEGATTSCHAR=fff2\r\n"; // 设置GATT特征值UUID为fff2
char AT_QBLEGATTSCHAR_2[] = "AT+QBLEGATTSCHAR=fff3\r\n"; // 设置GATT特征值UUID为fff3
char AT_QBLE_RESPONSE_FAIL[] = "AT+QBLEGATTSNTFY=fff2,\"1\"\r\n"; // 蓝牙配网不成功后回复1
char AT_QBLE_RESPONSE_SUCCESS[1024] = "AT+QBLEGATTSNTFY=fff2,0\r\n"; // 蓝牙配网成功后回复0
char AT_QBLEADVPARAM[] = "AT+QBLEADVPARAM=150,150\r\n"; // 设置BLE广播参数
char AT_QBLEADVSTART[] = "AT+QBLEADVSTART\r\n"; // 开启BLE广播
char AT_QBLEADVSTOP[] = "AT+QBLEADVSTOP\r\n"; // 关闭BLE广播
char AT_QBLEGATTSNTFY[512] = "AT+QBLEGATTSNTFY=fff2,123456789abcdef12345\r\n"; // 发送数据
char AT_QRST[] = "AT+QRST\r\n"; // 模块重启
char AT_QISEND[1024] = "AT+QISEND=\r\n"; // 通过TCP/UDP Socket 服务发送数据
char AT_QICFG[1024] = "AT+QICFG=\"datatype\",2\r\n"; // 通过TCP/UDP Socket 服务发送数据

char AT_QEXIT_PASSTHROUGH[] = "+++"; //tcp exit passthrough
char AT_QICLOSE[] = "AT+QICLOSE=0,3\r\n"; // clsoe tcp/ip socket service
char AT_QICLOSE_UDP[] = "AT+QICLOSE=11,3\r\n"; // clsoe tcp/ip socket service
char AT_QICLOSE_UDP_SEND[] = "AT+QICLOSE=10,3\r\n"; // clsoe tcp/ip socket service
char AT_QATO_IN_PASSTHROUGH[] = "ATO\r\n"; //tcp in passthrough

char IP_REMOTE[20] = "0.0.0.0";
uint32_t PORT_REMOTE = 8060;
uint32_t PORT_LOCAL = 1;
uint32_t ACCESS_MODE = 2;

char wifiname[20] = {0};
char wifipassword[20] = {0};

//uint8_t AT_QSTAAPINFO_[256] = "AT+QSTAAPINFO=droneid,12345678\r\n"; // 连接WIFI
//char AT_QSTAAPINFO_[256] = "AT+QSTAAPINFO=customer,Autel8888\r\n"; // 连接WIFI
char AT_QSTAAPINFO_[256] = "AT+QSTAAPINFO=abc3217,12345678\r\n"; // 连接WIFI
char AT_QSTAAPINFODEF_[256] = "AT+QSTAAPINFODEF=abc3217,12345678\r\n"; // 连接热点并保存热点信息

char AT_QIOPEN_UDP[AT_QIOPEN_LEN] = "AT+QIOPEN=11,\"UDP SERVICE\",\"0.0.0.0\",1810,1800,1\r\n"; // UDP接收消息
// char AT_QIOPEN_UDP_REC[AT_QIOPEN_LEN] = "AT+QIOPEN=0,\"UDP\",\"10.10.47.20\",8060,1800,2\r\n"; // UDP接收消息
char AT_QIOPEN_UDP_SEND[AT_QIOPEN_LEN] = "AT+QIOPEN=10,\"UDP\",\"255.255.255.255\",1810,1800,2\r\n"; // UDP发送消息

//uint8_t AT_QIOPEN_[256] = "AT+QIOPEN=0,\"TCP\",\"10.10.44.144\",6789,2020,2"; // 连接TCP
//uint8_t AT_QIOPEN_[AT_QIOPEN_LEN] = "AT+QIOPEN=0,\"TCP\",\"77.88.99.11\",666,1,2"; // 连接TCP
//uint8_t AT_QIOPEN_[AT_QIOPEN_LEN] = "AT+QIOPEN=0,\"TCP\",\"192.168.1.10\",9999,1,2"; // 连接TCP
//uint8_t AT_QIOPEN_[AT_QIOPEN_LEN] = "AT+QIOPEN=0,\"TCP\",\"10.10.47.169\",9999,1,2"; // 连接TCP
//uint8_t AT_QIOPEN_[AT_QIOPEN_LEN] = "AT+QIOPEN=0,\"TCP\",\"10.10.47.20\",9999,1,2\r\n"; // 连接TCP
//char AT_QIOPEN_[AT_QIOPEN_LEN] = "AT+QIOPEN=1,\"TCP\",\"192.168.43.1\",8060,1,2\r\n"; // 连接TCP
char AT_QIOPEN_[AT_QIOPEN_LEN] = "AT+QIOPEN=2,\"TCP\",\"192.168.43.1\",8060,1,2\r\n"; // 连接TCP
char AT_QIOPEN_OTHER_[AT_QIOPEN_LEN] = "AT+QIOPEN=2,\"TCP\",\"192.168.43.58\",8060,1,2\r\n"; // 连接TCP
char AT_QVERSION_[] = "AT+QVERSION\r\n"; // 获取固件版本
char AT_QGETIP_[] = "AT+QGETIP=station\r\n"; // 获取模块IP地址
char AT_QIWTMD_[] = "AT+QIWTMD=0,1\r\n"; // 切换数据访问模式      0:缓存模式   1：直吐模式   2：透传模式
char AT_QSTAST_[] = "AT+QSTAST\r\n"; //查询STA 模式开启状态
char AT_QGETWIFISTATE_[] = "AT+QGETWIFISTATE\r\n"; //查询已连接热点信息
char AT_QISTATET_[] = "AT+QISTATE=0\r\n"; //查询TCP/UDP Socket 服务状态

void update_wifi_info( char *name , char * password )
{

	// memcpy(s_wifi.name , name , strlen(name) );
	// memcpy(s_wifi.password , password , strlen(name) );
	sprintf( s_wifi.name,	"%s" , name);
	sprintf( s_wifi.password,"%s" , password);

	s_wifi.update = 0x01 ;

}

uint32_t wifi_info_status()
{
	//s_wifi.update = 0x01 ;
	return s_wifi.update ;
}

void update_tcp_ip_port_info( uint32_t l_ip_info , uint16_t remote_port )
{

	sprintf( s_tcp_socket.remote_addr , "%d.%d.%d.%d" ,
			(l_ip_info>>0)&0xff ,
			(l_ip_info>>8)&0xff ,
			(l_ip_info>>16)&0xff ,
			(l_ip_info>>24)&0xff );

	s_tcp_socket.remote_port = remote_port ;

	s_tcp_socket.update = 0x01 ;

}

char* get_tcp_ip()
{
	return s_tcp_socket.remote_addr ;
}

 void fc41d_change_listen_port( socket_info_t *socket_info )
 {

 	socket_info->local_port++;
 	if( socket_info->local_port > 65535 )
 	{
 		socket_info->local_port = 2 ;
 	}
 }

void fc41d_inc_tcp_socekt( socket_info_t *socket_info )
{

	socket_info->socketID++;
	if( socket_info->socketID > 9 )
	{
		socket_info->socketID = 0 ;
	}
}


void fc41d_recreate_QICLOSE( char* at_buff , socket_info_t socket_info )
{
	sprintf( at_buff,	"AT+QICLOSE=%d\r\n" , socket_info.socketID );
}

void fc41d_recreate_QIOPEN( char* at_buff , socket_info_t socket_info )
{

		sprintf( at_buff,	"AT+QIOPEN=%d,\"%s\",\"%s\",%d,%d,%d\r\n" ,
				socket_info.socketID ,
				socket_info.service_type ,
				socket_info.remote_addr ,
				socket_info.remote_port ,
				socket_info.local_port ,
				socket_info.state);

}


void fc41d_recreate_QSTAAPINFO( char* at_buff , wifi_info_t wifi_info )
{

	if( wifi_info.update != 0x00 )
	{
		sprintf( at_buff,	"AT+QSTAAPINFO=%s,%s\r\n" , wifi_info.name , wifi_info.password );
		wifi_info.update = 0x00 ;
	}

}

uint32_t rec_ack_data( uint8_t *buff , uint32_t max_len )
{
	uint32_t ret = 0x00 ;

	ret = ring_oupt_data( &s_ack_Ring, &buff[0], max_len );

	return ret ;
}
static uint8_t wifi_GetHeaderChecksum(alink_msg_head_t *header)
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
void wifi_send_data(char* wifi_Data,uint32_t DataLen)
{

	uint16_t uSendLen = 0;
	uint16_t uPayloadLen = 0;
	uint16_t crc = 0;
	alink_msg_t *pMsg = (alink_msg_t *)wifi_Data;

	uPayloadLen = (pMsg->msg_head.len_hi << 8) + pMsg->msg_head.len_lo;
	uSendLen = (pMsg->msg_head.len_hi << 8) + pMsg->msg_head.len_lo + sizeof(alink_msg_head_t);
	pMsg->msg_head.checksum = wifi_GetHeaderChecksum(&pMsg->msg_head);
	crc = crc_calculate((const uint8_t *)((&pMsg->msg_head.magic) + 1), uPayloadLen + 8); // 计算校验值不包含帧头，校验从字节1至字节N+8
	pMsg->buffer[uPayloadLen] = crc & 0xFF;
	pMsg->buffer[uPayloadLen + 1] = (crc & 0xFF00) >> 8;

	if (uSendLen > 0)
	{
		WifiSendDataFunc( (char*)wifi_Data, uSendLen + 2 );
	}

}
void BtSendData_(char socket, char *buff, uint32_t data_len , char *remote_ip , uint32_t remote_port )
{

	uint32_t head_len = 0x00 , tail_len = 0x00, actu_len = data_len;
	char socket_str[6] = {0};
	char data_len_str[6] = {0};
	char remote_port_str[6] = {0};
	char data_str[1024] = {0};
	char char_pot[2] = {'"'};
    char char_pot2[2] = {','};


	memset( AT_QISEND , 0x00 , sizeof(AT_QISEND) );

	//  itoa( socket , socket_str , 10 );
	//  itoa( data_len , data_len_str , 10 );
	//  sprintf( &AT_QISEND[0],	"AT+QISEND=%d,%d,", socket , data_len );

    //  head_len =strlen(&AT_QISEND[0]);
	//  memcpy( &AT_QISEND[head_len] , char_pot , 1 );

	//  head_len++;

	//  memcpy( &AT_QISEND[head_len] , buff , data_len );
	
	//  memcpy( &AT_QISEND[head_len+actu_len] , char_pot , 1 );
	//  actu_len++;
	//  memcpy( &AT_QISEND[head_len+actu_len] , char_pot2 , 1 );
	//  actu_len++;
	//  memcpy( &AT_QISEND[head_len+actu_len] , char_pot , 1 );
	//  actu_len++;

	//  itoa( remote_port , remote_port_str , 10 );
	//  sprintf(&AT_QISEND[head_len+actu_len],"%s%s,%d\r\n" , remote_ip,char_pot,remote_port);
	
	//sprintf(&AT_QISEND[head_len+data_len]","%s",%d\r\n" , remote_ip , remote_port );

	//  tail_len =strlen(&AT_QISEND[head_len+actu_len]);

	//  char AT_QISEND2[1024] = "AT+QISEND=11,6,\"FD3000\",\"172.20.10.4\",1810\r\n";
	//  uart_send_block( UARTLITE_WIFI , (char*)&AT_QISEND2, strlen(AT_QISEND2) , 0x10000);


    memcpy( &AT_QISEND[0] , buff , data_len );
    WifiSendDataFunc( (char*)&AT_QISEND, data_len );
	vTaskDelay(1000);
	//uart_send_block( UARTLITE_WIFI , (char*)&AT_QISEND, head_len+actu_len+tail_len , 0x10000);


	//wifi_send_data( AT_QISEND , head_len+data_len+tail_len );

}

uint8_t BtSendConfigData_(char *DataBufferPtr, uint32_t len, retDatCheck_e checkRet, uint16_t timeout, char *needCheckReturnStr)
{

	uint8_t dataRec[ACK_BUFF_LEN] = {0};
	uint32_t data_num = 0x00 ,ret_len = 0x00 ;
	uint8_t ret = BT_WIFI_RETURN_ERROR;

	if (DataBufferPtr == NULL || needCheckReturnStr == NULL)
	{
		ret = BT_WIFI_INVALID_PARAM;
		LOG_DEBUG("BtSendConfigData_  param is null\r\n");
		return ret;
	}

	WifiSendDataFunc( (char*)DataBufferPtr, len );

	for( uint32_t i = 0x00 ; i < timeout/BT_SEND_DATA_WAIT_TIME_300MS ; i++ )
	{

		vTaskDelay(BT_SEND_DATA_WAIT_TIME_300MS);

		ret_len = rec_ack_data( &dataRec[data_num] , (ACK_BUFF_LEN-data_num) );
		data_num += ret_len ;

		if(checkRet == RETURN_DATA_CKECK)
		{
			if (strstr((char*)dataRec, (char*)needCheckReturnStr) != NULL)
			{
				ret = BT_WIFI_OK;
				break;
			}
		}


		if( data_num >= ACK_BUFF_LEN )
		{
			data_num = 0x00 ;
		}
	}
	memset( dataRec , 0x00 , sizeof(dataRec) );

	if(checkRet == RETURN_DATA_CKECK)
	{
		if ( (ret == BT_WIFI_RETURN_ERROR) && (data_num > 0x00) )
		{
			LOG_ERROR("\r\nAT : %s", DataBufferPtr);
			LOG_ERROR("****** debug pend rec data,len=%d,string:%s\r\n", data_num, dataRec);
		}
	}
	else
	{
		ret = BT_WIFI_OK;
	}
	return ret;
}


int32_t fc41d_ble_peripherals_init()
{

	int32_t ret = BT_WIFI_OK;

	ret = BtSendConfigData_( AT_QURCCFG, strlen((char*)AT_QURCCFG), RETURN_DATA_UNCKECK, BT_SEND_DATA_WAIT_TIME_300MS, BT_CHECK_RETURN_DATA_IS_OK);
	if( ret != BT_WIFI_OK )
	{
		goto out;
	}
	ret = BtSendConfigData_( AT_QEXIT_PASSTHROUGH, strlen((char*)AT_QEXIT_PASSTHROUGH), RETURN_DATA_UNCKECK, BT_SEND_DATA_WAIT_TIME_300MS, BT_CHECK_RETURN_DATA_IS_OK);
	if( ret != BT_WIFI_OK )
	{
		goto out;
	}
	ret = BtSendConfigData_(AT_QRST, strlen((char*)AT_QRST), RETURN_DATA_UNCKECK, BT_SEND_DATA_WAIT_TIME_10S, BT_CHECK_RETURN_DATA_IS_OK);
	if( ret != BT_WIFI_OK )
	{
		goto out;
	}
	ret = BtSendConfigData_( AT_QSTASTOP, strlen((char*)AT_QSTASTOP), RETURN_DATA_UNCKECK, BT_SEND_DATA_WAIT_TIME_300MS, BT_CHECK_RETURN_DATA_IS_OK);
	if( ret != BT_WIFI_OK )
	{
		goto out;
	}
	ret = BtSendConfigData_(AT_QBLEINIT, strlen((char*)AT_QBLEINIT), RETURN_DATA_CKECK, BT_SEND_DATA_WAIT_TIME_300MS, BT_CHECK_RETURN_DATA_IS_OK);
	if( ret != BT_WIFI_OK )
	{
		goto out;
	}

	ret = BtSendConfigData_(AT_QBLEADVPARAM, strlen((char*)AT_QBLEADVPARAM), RETURN_DATA_CKECK, BT_SEND_DATA_WAIT_TIME_300MS, BT_CHECK_RETURN_DATA_IS_OK);
	if( ret != BT_WIFI_OK )
	{
		goto out;
	}

	ret = BtSendConfigData_(AT_QBLEGATTSSRV, strlen((char*)AT_QBLEGATTSSRV), RETURN_DATA_CKECK, BT_SEND_DATA_WAIT_TIME_300MS, BT_CHECK_RETURN_DATA_IS_OK);
	if( ret != BT_WIFI_OK )
	{
		goto out;
	}

	ret = BtSendConfigData_(AT_QBLEGATTSCHAR_1, strlen((char*)AT_QBLEGATTSCHAR_1), RETURN_DATA_CKECK, BT_SEND_DATA_WAIT_TIME_300MS, BT_CHECK_RETURN_DATA_IS_OK);
	if( ret != BT_WIFI_OK )
	{
		goto out;
	}
	ret = BtSendConfigData_(AT_QBLENAME, strlen((char*)AT_QBLENAME), RETURN_DATA_CKECK, BT_SEND_DATA_WAIT_TIME_300MS, BT_CHECK_RETURN_DATA_IS_OK);
	if( ret != BT_WIFI_OK )
	{
		goto out;
	}
	ret = BtSendConfigData_(AT_QBLEADVSTART, strlen((char*)AT_QBLEADVSTART), RETURN_DATA_CKECK, BT_SEND_DATA_WAIT_TIME_300MS, BT_CHECK_RETURN_DATA_IS_OK);
	if( ret != BT_WIFI_OK )
	{
		goto out;
	}

out:

	BtSendConfigData_(AT_QATO_IN_PASSTHROUGH, strlen((char*)AT_QATO_IN_PASSTHROUGH), RETURN_DATA_CKECK, BT_SEND_DATA_WAIT_TIME_300MS, BT_CHECK_RETURN_DATA_IS_OK);

	return ret;

}

int32_t fc41d_wifi_connect()
{

	int32_t ret = BT_WIFI_OK;
	ret = BtSendConfigData_( AT_QGETWIFISTATE_, strlen((char*)AT_QGETWIFISTATE_), RETURN_DATA_CKECK, BT_SEND_DATA_WAIT_TIME_300MS, BT_CHECK_RETURN_DATA_IS_OK);
	if( ret == BT_WIFI_OK )
	{
		goto out;
	}
	ret = BtSendConfigData_( AT_QEXIT_PASSTHROUGH, strlen((char*)AT_QEXIT_PASSTHROUGH), RETURN_DATA_UNCKECK, BT_SEND_DATA_WAIT_TIME_300MS, BT_CHECK_RETURN_DATA_IS_OK);
	if( ret != BT_WIFI_OK )
	{
		goto out;
	}

	ret = BtSendConfigData_( AT_QSTASTOP, strlen((char*)AT_QSTASTOP), RETURN_DATA_CKECK, BT_SEND_DATA_WAIT_TIME_300MS, BT_CHECK_RETURN_DATA_IS_OK);
	if( ret != BT_WIFI_OK )
	{
		goto out;
	}
	fc41d_create_response_ble(AT_QBLE_RESPONSE_SUCCESS,0);
	BtSendConfigData_( AT_QBLE_RESPONSE_SUCCESS, strlen((char*)AT_QBLE_RESPONSE_SUCCESS), RETURN_DATA_CKECK, BT_SEND_DATA_WAIT_TIME_300MS, "OK");
	fc41d_recreate_QSTAAPINFO(AT_QSTAAPINFO_,s_wifi);
	ret = BtSendConfigData_(AT_QSTAAPINFO_, strlen((char*)AT_QSTAAPINFO_), RETURN_DATA_CKECK, BT_SEND_DATA_WAIT_TIME_10S, "WLAN_CONNECTED");
	if( ret != BT_WIFI_OK )
	{
		fc41d_create_response_ble(AT_QBLE_RESPONSE_FAIL,1);
		BtSendConfigData_( AT_QBLE_RESPONSE_FAIL, strlen((char*)AT_QBLE_RESPONSE_FAIL), RETURN_DATA_CKECK, BT_SEND_DATA_WAIT_TIME_300MS, "OK");
		goto out;
	}

out:

	BtSendConfigData_(AT_QATO_IN_PASSTHROUGH, strlen((char*)AT_QATO_IN_PASSTHROUGH), RETURN_DATA_CKECK, BT_SEND_DATA_WAIT_TIME_300MS, BT_CHECK_RETURN_DATA_IS_OK);
	return ret;

}

int32_t fc41d_udp_connect()
{
	int32_t ret = BT_WIFI_OK;

	//fc41d_recreate_QIOPEN( AT_QIOPEN_UDP_REC , s_udp_rec_socket ) ;
	ret = BtSendConfigData_( AT_QIOPEN_UDP, strlen((char*)AT_QIOPEN_UDP), RETURN_DATA_CKECK, BT_SEND_DATA_WAIT_TIME_300MS, "QIOPEN");
	if( ret != BT_WIFI_OK )
	{
		goto out;
	}
	ret = BtSendConfigData_( AT_QIOPEN_UDP_SEND, strlen((char*)AT_QIOPEN_UDP_SEND), RETURN_DATA_CKECK, BT_SEND_DATA_WAIT_TIME_300MS, "CONNECT");
	if( ret != BT_WIFI_OK )
	{
		goto out;
	}

out:
	return ret;
}

int32_t fc41d_udp_socket_close()
{
	int32_t ret = BT_WIFI_OK;

	ret = BtSendConfigData_( AT_QICLOSE_UDP, strlen((char*)AT_QICLOSE_UDP), RETURN_DATA_UNCKECK, BT_SEND_DATA_WAIT_TIME_300MS, "close");
	if( ret != BT_WIFI_OK )
	{
		goto out;
	}
	ret = BtSendConfigData_( AT_QICLOSE_UDP_SEND, strlen((char*)AT_QICLOSE_UDP_SEND), RETURN_DATA_UNCKECK, BT_SEND_DATA_WAIT_TIME_300MS, "close");
	if( ret != BT_WIFI_OK )
	{
		goto out;
	}
	//  ret = BtSendConfigData_( AT_QICFG, strlen((char*)AT_QICFG), RETURN_DATA_CKECK, BT_SEND_DATA_WAIT_TIME_300MS, "OK");
	//  if( ret != BT_WIFI_OK )
	//  {
	//  	goto out;
	//  }
	// //fc41d_recreate_QIOPEN( AT_QIOPEN_UDP_SEND , s_udp_send_socket ) ;
	// ret = BtSendConfigData_( AT_QIOPEN_UDP_SEND, strlen((char*)AT_QIOPEN_UDP_SEND), RETURN_DATA_CKECK, BT_SEND_DATA_WAIT_TIME_300MS, "QIOPEN");
	// if( ret != BT_WIFI_OK )
	// {
	// 	goto out;
	// }
    
out:
	return ret;
}

int32_t fc41d_tcp_socket_close()
{
	int32_t ret = BT_WIFI_OK;

	fc41d_recreate_QICLOSE( AT_QIOPEN_, s_tcp_socket );

	ret = BtSendConfigData_( AT_QICLOSE, strlen((char*)AT_QICLOSE), RETURN_DATA_UNCKECK, BT_SEND_DATA_WAIT_TIME_300MS, "close");
	if( ret != BT_WIFI_OK )
	{
		goto out;
	}
	fc41d_inc_tcp_socekt(&s_tcp_socket);

out:
	return ret;
}


int32_t fc41d_tcp_connect()
{

	int32_t ret = BT_WIFI_OK;
	int32_t index = 0;

	ret = BtSendConfigData_( AT_QEXIT_PASSTHROUGH, strlen((char*)AT_QEXIT_PASSTHROUGH), RETURN_DATA_UNCKECK, BT_SEND_DATA_WAIT_TIME_300MS, BT_CHECK_RETURN_DATA_IS_OK);
	if( ret != BT_WIFI_OK )
	{
		goto out;
	}

	fc41d_recreate_QIOPEN( AT_QIOPEN_, s_tcp_socket );

	ret = BtSendConfigData_(AT_QIOPEN_, strlen((char*)AT_QIOPEN_), RETURN_DATA_CKECK, BT_SEND_DATA_WAIT_TIME_10S, "CONNECT");
	if( ret != BT_WIFI_OK )
	{
		goto out;
	}

    fc41d_change_listen_port(&s_tcp_socket);

	out:
		return ret;
}

btConfigStepState_e fc41d_state_check( )
{
	btConfigStepState_e ret = BT_STATE_CHECK ;
	uint8_t stateDataRec[ACK_BUFF_LEN] = {0};
	uint32_t data_num = 0x00;

	data_num = rec_ack_data( &stateDataRec[0] , ACK_BUFF_LEN );

	if( data_num <= 0x00 )
	{
		return ret ;
	}

	if ((strstr((char*)stateDataRec, "WLAN_CONNECTED") != NULL) || (strstr((char*)stateDataRec, "GOT_IP") != NULL))
	{
		ret = BT_STATE_CHECK ;
	}
	else if (strstr((char*)stateDataRec, "WLAN_DISCONNECTED") != NULL)
	{
		ret = BT_CONFIG_STEP_STATE_CONNECT_WIFI ;
		fc41d_tcp_socket_close();
	}

	else if ((strstr((char*)stateDataRec, "DISCONNECTED") != NULL)  || (strstr((char*)stateDataRec, "BLESTAT:DISCON") != NULL))
	{
		ret = BT_CONFIG_STEP_STATE_CONNECT_WIFI ;
		fc41d_tcp_socket_close();
	}
	else if (strstr((char*)stateDataRec, "CONNECTED") != NULL)
	{
		ret = BT_STATE_CHECK ;
	}
	else if (strstr((char*)stateDataRec, "CONNECT") != NULL)
	{
		ret = BT_STATE_CHECK ;
	}
	else if (strstr((char*)stateDataRec, "SCAN_NO_AP") != NULL)
	{
		ret = BT_CONFIG_STEP_STATE_CONNECT_WIFI ;
		fc41d_tcp_socket_close();
	}
	else if (strstr((char*)stateDataRec, "CARRIER") != NULL)
	{
//		BtSendConfigData_(AT_QICLOSE_, strlen((char*)AT_QICLOSE_), RETURN_DATA_CKECK, BT_SEND_DATA_WAIT_TIME_1000MS*5, "closed");
		ret = BT_CONFIG_STEP_STATE_CONNECT_WIFI ;
		fc41d_tcp_socket_close();

	}
	else if (strstr((char*)stateDataRec, "closed") != NULL)
	{
		ret = BT_STATE_CHECK ;
	}

	return ret ;
}

void init_fc41d_ack_buff()
{
	ring_init( &s_ack_Ring, s_ack_buffer, sizeof(s_ack_buffer) );
}

void fc41d_ack_process()
{

	uint16_t crc;
	uint32_t msg_data_len = 0x00 ;
	uint8_t data_buff[ACK_BUFF_LEN] = {0} ;
	uint32_t len = 0x00 ,total_len = 0x00 ;
	char * buf_ptr = NULL ;
	alink_msg_t *sAlinkRecv_msg = (alink_msg_t *)s_cmd_buffer;

	uint32_t i;
	uint32_t uRecvHeadCnt = 0;

	uint32_t loop = 0x00 ;


	len = output_wifi_uart_data( data_buff , sizeof(s_ack_Ring) );

	buf_ptr = strstr((char*)data_buff, (char*)BT_CHECK_RETURN_DATA_FLAG);

	if ( buf_ptr != NULL)
	{
		do
		{
			for( i = 0x00 ; i < len ; i++ )
			{
				if( data_buff[i] == 0xfd )
				{
					break;
				}
			}

			if( i != len )
			{
				total_len = len - i ;

				memset( s_cmd_buffer , 0x00 , sizeof(s_cmd_buffer) );
				memcpy( s_cmd_buffer , &data_buff[i] , total_len );
				break;
			}
			else
			{
				vTaskDelay(1);
				loop++;
				if( loop <= 5 )
				{
					memset( data_buff , 0x00 , sizeof(data_buff) );
					len = output_wifi_uart_data( &data_buff[0] , sizeof(data_buff) );
				}
				else
				{
					return ;
				}

			}

		}while(1);

		while(1)
		{
			len = output_wifi_uart_data( &s_cmd_buffer[total_len] , sizeof(s_cmd_buffer)-total_len );

			if( len == 0x00 )
			{
				vTaskDelay(1);
			}
			else
			{
				total_len +=  len;
				if( total_len >= sizeof(alink_msg_head_t) )
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
					if (total_len >= ( uRecvHeadCnt + msg_data_len + 2 ) )
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
				if( total_len >= ACK_BUFF_LEN)
				{
					break;
				}
			}
		}
	}
	else
	{
		ring_inpt_data( &s_ack_Ring, data_buff, len );
	}

}


