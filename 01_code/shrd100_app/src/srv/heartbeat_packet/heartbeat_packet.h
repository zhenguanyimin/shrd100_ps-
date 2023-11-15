/*
 * heartbeat_packet.h
 *
 *  Created on: 2022-09-20
 *      Author: A19199
 */
#ifndef HEARTBEAT_PACKET_H
#define HEARTBEAT_PACKET_H
/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../alink/alink_msg_proc.h"
#include "../../app/alg/det_alg/droneSniffer.h"
#include "../../app/alg/droneID/droneID_system_param.h"
#include "../../hal/hal.h"
#include "../../app/remote_id/remoteid_parse.h"

#pragma pack(1)
typedef struct droneList
{
	uint8_t productType;		/* 无人机类型 */
	uint8_t droneName[25];		/* 字符串 （品牌+机型） */
	uint8_t serialNum[32];		/* 字符串 （无人机SN码） */
	int32_t droneLongitude;		/* LSB无人机经度(/1e7/pi*180) */
	int32_t droneLatitude;		/* LSB无人机纬度(/1e7/pi*180) */
	int16_t droneHeight;		/* LSB无人机与飞手的相对高度(0.1m) */
	int16_t droneYawAngle;		/* LSB无人机偏航角度(0.01deg) */
	int16_t droneSpeed;			/* LSB无人机绝对速度(0.01m/s) */
	int16_t droneVerticalSpeed;	/* LSB无人机垂直速度(0.01m/s) */
	int32_t pilotLongitude;		/* LSB飞手经度(/1e7/pi*180) */
	int32_t pilotLatitude;		/* LSB飞手纬度(/1e7/pi*180) */
	uint32_t droneuFreq;		/* LSB 无人机信号频率(mHz) */
	uint16_t droneuDistance;	/* LSB 无人机与侦测设备的相对距离(m) */
	uint16_t droneuDangerLevels;/* LSB 危险等级 */
} droneList_t;


typedef struct spectrumList
{
	uint8_t uav_number;			/* 无人机编号 */
	uint8_t droneName[25];		/* 字符串 无人机名称 */
	int32_t droneHorizon;		/* LSB目标水平角(0.01°) */
	int32_t uFreq;				/* LSB 无人机信号频率(mHz) */
	int16_t uDangerLevels;		/* LSB 危险等级 */
	int32_t Recerve;			/* 保留 */
} spectrumList_t;



typedef struct remoteList
{

	uint8_t productType;//		无人机类型（lite/parrot/…）
	char droneName[25];//		字符串 无人机名称
	char serialNum[32];//		字符串
	int32_t droneLongitude;//		无人机经度(/1e7)
	int32_t droneLatitude;//		无人机纬度(/1e7)
	int16_t droneHeight;//		无人机相对地面高度(0.1m)
	int16_t droneDirection;//		无人机角度(0.01deg)
	int16_t droneSpeed;//		无人机绝对速度(0.01m/s)
	uint8_t Speedderection;//		0：无人机水平向前  lite (1：无人机水平向后  lite; 2：无人机水平向左  lite;3：无人机水平向右  lite;4：无人机垂直)
	int16_t droneVerticalSpeed;//		无人机水平/垂直速度(0.01m/s)
	int32_t droneSailLongitude;//		无人机航点经度(/1e7)
	int32_t droneSailLatitude;//		无人机航点经度(/1e7)
	uint32_t uFreq;//		无人机信号频率(mHz)
	uint16_t uDistance;//		无人机与TRACER的距离(m)
	uint16_t uDangerLevels;//		危险等级

} remoteList_t;

//typedef struct DroneID_ParseInfo_t_t
//{
//	uint8_t     type_num;
//    uint8_t     type_name[20];
//    uint8_t     serial_num[16];
//    int32_t     drone_longtitude;
//    int32_t     drone_latitude;
//    int16_t     altitude;
//    int16_t     yaw_angle;
//    int16_t     speed;
//    int16_t     z_speed;
//    int32_t     pilot_longitude;
//    int32_t     pilot_latitude;
//} DroneID_ParseInfo_t;

typedef struct GpsInfo
{
	int32_t     Longitude;
	int32_t     Latitude;
	int16_t     Altitude;
	uint16_t    SatellitesNum;
} GpsInfo_t;


#pragma pack()
void UpdataGyroInfo(int32_t Elevation_ , int32_t Direction_);
void UpdataGpsInfo(int32_t Longitude, int32_t Latitude, int32_t Altitude, int32_t Num);
void UpdataUAVGpsInfo(int32_t Longitude, int32_t Latitude, int16_t Altitude);
int32_t InitTeartbeatPacket_Task();
uint8_t UpdataDroneInfo();
int32_t UpdataDroneInfoAlg2(struct droneResult *droneInfo, int nDrone, float cenFreq, uint8_t levels);
uint8_t GetalarmLevel(uint8_t alarm, uint8_t buzzer, uint8_t motor);
void SetBuzerStatus(IO_STATUS value);
void SetMotorStatus(IO_STATUS value);
void SetAlarmLevel(uint8_t value);
uint8_t UpdateRemoteInfo();
#endif /* HEARTBEAT_PACKET_H */
