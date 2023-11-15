/*
 * heartbeat_packet.c
 *
 *  Created on: 2022-08-24
 *      Author: A19199
 */
/* FreeRTOS includes. */
#include "heartbeat_packet.h"
#include "FreeRTOS.h"
#include "task.h"
#include <queue.h>
#include <timers.h>

#include "../../cfg/shrd_config.h"
#include "../../srv/log/log.h"
#include "../alink/alink_msg_proc.h"
#include "../link_srv/servers/service_manager.h"
#include "../../app/battery/battery_info.h"
#include "../../hal/output/output.h"
#include "../../app/remote_id/remoteid_parse.h"
#include "../../app/remote_id/remoteid_app.h"
#include "../../app/sys_status_data/detection_target.h"
#include "../../app/sys_status_data/detection_param.h"
#include "../../app/process_task/orientation.h"
#include "../../app/alg/droneID/droneID_main.h"
#include "../../app/alg/tracking/tracking.h"
#include "../../srv/system_monitor/system_monitor.h"
#define DRONE_MAX_COUNT 30
static uint8_t uDroneTimeoutCnt = 0;

static alink_msg_t mTeartbeatPacketMsg;

extern void RemoteIdSeamphoreTake(void);
extern void RemoteIdSeamphoreGive(void);

remoteList_t mRemoteList[DRONE_MAX_COUNT] =
{
		{
				.productType = 0x08,
				.droneName = { "DJI Mavic2"},
				.serialNum = {"1234"},
				.droneLongitude = 0,
				.droneLatitude = 0,
				.droneHeight = 0,
				.droneDirection = 0,
				.droneSpeed = 0,
				.Speedderection = 0,
				.droneVerticalSpeed = 0,
				.droneSailLongitude = 0,
				.droneSailLatitude = 0,
				.uFreq = 2423,
				.uDistance = 0xFFFF,
				.uDangerLevels = 2,
		},
};

droneList_t mDroneList[DRONE_MAX_COUNT] =
{
		{
				.productType = 0x08,
				.droneName = { "DJI Mavic2"},
				.serialNum = {"123456789"},
				.droneLongitude = 0x7FFFFFFF,
				.droneLatitude = 0x7FFFFFFF,
				.droneHeight = 0x7FFF,
				.droneYawAngle = 0x7FFF,
				.droneSpeed = 0x7FFF,
				.droneVerticalSpeed = 0x7FFF,
				.pilotLongitude = 0x7FFFFFFF,
				.pilotLatitude = 0x7FFFFFFF,
				.droneuFreq = 2423,
				.droneuDistance = 0xFFFF,
				.droneuDangerLevels = 2,
		},
};
spectrumList_t mSpectrumList[DRONE_MAX_COUNT] =
{
		{
				.uav_number = 0x00,
				.droneName = { "DJI Mavic2"},
				.droneHorizon = 0,
				.uFreq = 2423,
				.uDangerLevels = 2,
				.Recerve= 0,
		},
};

static GpsInfo_t mGpsInfo = {0};
static GpsInfo_t mUAVGpsInfo = {0};
uint8_t uDroneNum = 0;// 侦测到无人机个数
uint8_t uSpectrumNum = 0;
static uint8_t AlarmLevel = 0;
static uint16_t DetectFreq = 0;
static int32_t Elevation = 0;
static int32_t Direction = 0;
static uint8_t bat_original_data = 0;

static uint8_t BuzerValue = 0;
static uint8_t MotorValue = 0;

uint8_t UpdataDroneInfo()
{
	uint32_t i = 0;
	uint32_t uDroneIdNum = 0;
	DroneID_OutputInfoList_t *droneidList = {0};

	DroneidSeamphoreTake();
//	droneidList = DroneID_GetOutputListInfo();
	droneidList = DroneID_GetOutputListInfoPredict();

	uDroneIdNum = droneidList->DroneIdNum;

//	LOG_INFO("**********DroneInfo**********\r\n");
	memset(mDroneList, 0, sizeof(mDroneList[0])*uDroneIdNum);
	for (i = 0; i < uDroneIdNum; i++)
	{
		mDroneList[i].productType = droneidList->DroneIdOutputInfoList[i].droneID.type_num;
		memcpy(mDroneList[i].droneName, droneidList->DroneIdOutputInfoList[i].droneID.type_name, strlen(droneidList->DroneIdOutputInfoList[i].droneID.type_name));
		memcpy(mDroneList[i].serialNum, droneidList->DroneIdOutputInfoList[i].droneID.serial_num, strlen(droneidList->DroneIdOutputInfoList[i].droneID.serial_num));
		mDroneList[i].droneLongitude = droneidList->DroneIdOutputInfoList[i].droneID.drone_longtitude;
		mDroneList[i].droneLatitude = droneidList->DroneIdOutputInfoList[i].droneID.drone_latitude;
		mDroneList[i].droneHeight = droneidList->DroneIdOutputInfoList[i].droneID.height;
		mDroneList[i].droneYawAngle = droneidList->DroneIdOutputInfoList[i].droneID.yaw_angle;
		mDroneList[i].droneSpeed = droneidList->DroneIdOutputInfoList[i].droneID.speed;
		mDroneList[i].droneVerticalSpeed = droneidList->DroneIdOutputInfoList[i].droneID.z_speed;
		mDroneList[i].pilotLongitude = droneidList->DroneIdOutputInfoList[i].droneID.pilot_longitude;
		mDroneList[i].pilotLatitude = droneidList->DroneIdOutputInfoList[i].droneID.pilot_latitude;
		mDroneList[i].droneuFreq = (uint32_t)(droneidList->DroneIdOutputInfoList[i].droneID.drone_Freq / 1e6);
		mDroneList[i].droneuDangerLevels = droneidList->DroneIdOutputInfoList[i].droneID.danger_levels;

//		LOG_INFO("droneid info : type:%d(%s), sn:%s\r\n",mDroneList[i].productType,mDroneList[i].droneName,mDroneList[i].serialNum);
	}
//	LOG_INFO("***************************\r\n");
	DroneidSeamphoreGive();
	return uDroneIdNum;

}

int32_t UpdataDroneInfoAlg2(struct droneResult *droneInfo, int nDrone, float cenFreq, uint8_t levels)
{
	int32_t ret = -1;


	if (nDrone > DRONE_MAX_COUNT)
	{
		ret = -1;
	}
	else
	{
		memset(mDroneList, 0, sizeof(mDroneList)*nDrone);
		for (uint8_t i = 0; i < nDrone; i++)
		{
			uDroneTimeoutCnt = 0;
			mDroneList[i].productType = 0xFF;
			memcpy(mDroneList[i].droneName, droneInfo[i].name, strlen(droneInfo[i].name));
			memset(mDroneList[i].serialNum, 0, sizeof(mDroneList[i].serialNum));
			mDroneList[i].droneLongitude = 0x7FFFFFFF;
			mDroneList[i].droneLatitude = 0x7FFFFFFF;
			mDroneList[i].droneHeight = 0x7FFF;
			mDroneList[i].droneYawAngle = 0x7FFF;
			mDroneList[i].droneSpeed = 0x7FFF;
			mDroneList[i].droneVerticalSpeed = 0x7FFF;
			mDroneList[i].pilotLongitude = 0x7FFFFFFF;
			mDroneList[i].pilotLatitude = 0x7FFFFFFF;
//			mDroneList[i].droneHorizon = GetLevelAngleCen();
//			mDroneList[i].dronePitch = GetPitchAngleCen();
			mDroneList[i].droneuFreq = (uint32_t)droneInfo[i].freq[0];
			mDroneList[i].droneuDistance = 0xFFFF;
			mDroneList[i].droneuDangerLevels = levels;
			DetectFreq = cenFreq;
//			AlarmLevel = levels;
		}
		uDroneNum = nDrone;
		ret = 0;
	}

	return ret;
}
static int32_t UpdataSpectrum(void)
{
	int32_t ret = -1;

	DroneReportResult_t *LocalDroneReportInfo = {0};

    SpectrumSeamphoreTake();
    LocalDroneReportInfo = SYS_GetDroneInfo();

	if (LocalDroneReportInfo->DroneCnt > DRONE_MAX_COUNT)
	{
		ret = -1;
	}
	else
	{
		memset(mSpectrumList, 0, sizeof(mSpectrumList[0])*LocalDroneReportInfo->DroneCnt);
		for (uint8_t i = 0; i < LocalDroneReportInfo->DroneCnt; i++)
		{
			uDroneTimeoutCnt = 0;
			mSpectrumList[i].uav_number = i+1;
			memset(mSpectrumList[i].droneName, 0, sizeof(mSpectrumList[i].droneName));
			memcpy(mSpectrumList[i].droneName, LocalDroneReportInfo->ReportResult[i].DroneResult.name, strlen(LocalDroneReportInfo->ReportResult[i].DroneResult.name));


			mSpectrumList[i].uFreq = (uint32_t)LocalDroneReportInfo->ReportResult[i].DroneResult.freq[0];

			mSpectrumList[i].uDangerLevels = 2;
			//			AlarmLevel = levels;

//			if ((GetOrientationMode() >= eWORK_MODE_SCANNING_HORI) && (GetOrientationMode() <= eWORK_MODE_AIM))
//			{
//				if (uDroneNum == 0)
//				{
//					uDroneNum = 1;
//				}
//			}

			mSpectrumList[i].droneHorizon = 0x7fffffff;
		}
//		mSpectrumList[GetUavNumber()].droneHorizon = GetLevelAngleCen();
		mSpectrumList[0].droneHorizon = GetLevelAngleCen();

		uSpectrumNum = LocalDroneReportInfo->DroneCnt;
		ret = 0;
	}
	SpectrumSeamphoreGive();

	return ret;
}
void UpdataGpsInfo(int32_t Longitude, int32_t Latitude, int32_t Altitude, int32_t Num)
{
	mGpsInfo.Longitude = Longitude;
	mGpsInfo.Latitude = Latitude;
	mGpsInfo.Altitude = Altitude;
	mGpsInfo.SatellitesNum = Num;
}

void UpdataUAVGpsInfo(int32_t Longitude, int32_t Latitude, int16_t Altitude)
{
	mUAVGpsInfo.Longitude = Longitude;
	mUAVGpsInfo.Latitude = Latitude;
	mUAVGpsInfo.Altitude = Altitude;
	mUAVGpsInfo.SatellitesNum = 0;
}

void UpdataGyroInfo(int32_t Elevation_ , int32_t Direction_)
{
	Elevation = Elevation_;
	Direction = Direction_;
}

void TimeoutClearData(int16_t Elevation_ , int16_t Direction_)
{
}
uint8_t UpdateRemoteInfo()
{
	uint8_t uRemoteNum = 0;
	uint8_t DeviceType = 0;
	char remote_lite[20] =  "lite rid1";
	char remote_parrot[20] =  "parrot rid1";
	char remote_other[20] =  "unknow rid1";

	RemoteID_OutputInfoList_t *remoteid_list = {0};
	RemoteIdSeamphoreTake();

	remoteid_list = RemoteID_GetOutputListInfo();

	uRemoteNum = remoteid_list->RemoteIdNum;

	memset(mRemoteList, 0, sizeof(mRemoteList[0])*uRemoteNum);
    for(int i = 0;i < uRemoteNum;i++){
    	mRemoteList[i].uFreq = 2437 ;
        DeviceType = remoteid_list->RemoteIdOutputInfoList[i].DeviceType;
		mRemoteList[i].productType = DeviceType;
		if(DeviceType == 0x01)
		{
			memcpy(&(mRemoteList[i].droneName), &remote_lite, sizeof(remote_lite));
			mRemoteList[i].droneHeight = (2000 - remoteid_list->RemoteIdOutputInfoList[i].UavHeightAgl) / 2 * 10 ;
			if((remoteid_list->RemoteIdOutputInfoList[i].UavSpeedMultip) == 0x00)
		    {
               mRemoteList[i].droneSpeed = remoteid_list->RemoteIdOutputInfoList[i].UavAbsolSpeed / 4 * 100 ;
			   mRemoteList[i].droneVerticalSpeed = 0;
		    }
		    else
		    {
                mRemoteList[i].droneSpeed = remoteid_list->RemoteIdOutputInfoList[i].UavAbsolSpeed / 4 * 300 ;
			    mRemoteList[i].droneVerticalSpeed = 0;
		    }
			mRemoteList[i].droneLongitude = (remoteid_list->RemoteIdOutputInfoList[i].PilotLongitude)  / 180 * PI_VALUE ;
		    mRemoteList[i].droneSailLatitude = (remoteid_list->RemoteIdOutputInfoList[i].PilotLatitude) / 180 * PI_VALUE ;
		}
        else if(DeviceType == 0x02)
		{
			mRemoteList[i].droneHeight = (remoteid_list->RemoteIdOutputInfoList[i].UavHeightAgl - 2000) / 2 * 10 ;
			memcpy(&(mRemoteList[i].droneName), &remote_parrot, sizeof(remote_parrot));
			if((remoteid_list->RemoteIdOutputInfoList[i].UavSpeedMultip) == 0x00)
		    {
               mRemoteList[i].droneSpeed = remoteid_list->RemoteIdOutputInfoList[i].UavAbsolSpeed / 4 * 100 ;
			   mRemoteList[i].droneVerticalSpeed = remoteid_list->RemoteIdOutputInfoList[i].UavVertSpeed / 2 * 100;
		    }
		    else
		    {
                mRemoteList[i].droneSpeed = remoteid_list->RemoteIdOutputInfoList[i].UavAbsolSpeed / 4 * 300 ;
			    mRemoteList[i].droneVerticalSpeed = remoteid_list->RemoteIdOutputInfoList[i].UavVertSpeed / 2 * 300;
		    }
			mRemoteList[i].droneSailLongitude = (remoteid_list->RemoteIdOutputInfoList[i].PilotLongitude)  / 180 * PI_VALUE ;
		    mRemoteList[i].droneSailLatitude = (remoteid_list->RemoteIdOutputInfoList[i].PilotLatitude) / 180 * PI_VALUE ;
		}
		else
		{
            memcpy(&(mRemoteList[i].droneName), &remote_other, sizeof(remote_other));
		}
		memcpy(mRemoteList[i].serialNum, remoteid_list->RemoteIdOutputInfoList[i].UavSnCode, sizeof(mRemoteList[i].serialNum));

		mRemoteList[i].droneLongitude = (remoteid_list->RemoteIdOutputInfoList[i].UavLongitude) / 180 * PI_VALUE ;
		mRemoteList[i].droneLatitude = (remoteid_list->RemoteIdOutputInfoList[i].UavLatitude)  / 180 * PI_VALUE ;


		mRemoteList[i].droneDirection = remoteid_list->RemoteIdOutputInfoList[i].UavDirection;

    }

	RemoteIdSeamphoreTake();
    return uRemoteNum ;
}
void PacketHearBeatData(alink_msg_t *response_msg , uint8_t destid )
{
	uint16_t length = 0;
	uint32_t cnt = 0;
	uint32_t TimeStamp;
	uint8_t lRemoteNum = 0;
	uint8_t lSpectrumNum = 0;
	uint8_t lDroneNum = 0;
	uint8_t lTotalNum = 0;

	memset(response_msg->buffer, 0, RECV_MAX_BUFF);

	DetectFreq = SYS_GetDetectFreq();

    TimeStamp = (uint32_t)xTaskGetTickCount();
    memcpy(&response_msg->buffer[cnt], &TimeStamp, sizeof(uint32_t));// 系统时间戳
    cnt += sizeof(uint32_t);
    response_msg->buffer[cnt++] = 0x01;// 亮屏/息屏
    bat_original_data = 99;//UpdataBatteryCapacityAndDisplayLedState(bat_original_data);
    response_msg->buffer[cnt++] = bat_original_data;
    response_msg->buffer[cnt++] = 100;// wifi
    response_msg->buffer[cnt++] = 0;// 
    response_msg->buffer[cnt++] = 0;// 
    response_msg->buffer[cnt++] = 0;// 
    memcpy(&response_msg->buffer[cnt], &DetectFreq, sizeof(uint16_t));// 拷贝LSB扫描频率
    cnt += sizeof(uint16_t);
//    memcpy(&response_msg->buffer[cnt], &Elevation, sizeof(uint32_t));// LSB枪俯仰角（0.01°）
    cnt += sizeof(uint32_t);
//    memcpy(&response_msg->buffer[cnt], &mGpsInfo.Longitude, sizeof(uint32_t));// 拷贝LSB枪经度
    cnt += sizeof(uint32_t);
//    memcpy(&response_msg->buffer[cnt], &mGpsInfo.Latitude, sizeof(uint32_t));// 拷贝LSB枪纬度
    cnt += sizeof(uint32_t);
//    memcpy(&response_msg->buffer[cnt], &mGpsInfo.Altitude, sizeof(uint32_t));// 拷贝LSB枪海拔高度
    cnt += sizeof(uint32_t);
//    memcpy(&response_msg->buffer[cnt], &mGpsInfo.SatellitesNum, sizeof(uint16_t));// 拷贝LSB枪卫星数量
    cnt += sizeof(uint16_t);
    memcpy(&response_msg->buffer[cnt], &Direction, sizeof(uint32_t));// 拷贝LSB枪方位
    cnt += sizeof(uint32_t);

    memcpy(&response_msg->buffer[cnt], get_device_sn_code(), DEVICE_SN_CODE_LEN);// sn code
    cnt += DEVICE_SN_CODE_LEN;

    //

    UpdataSpectrum();
	lSpectrumNum = uSpectrumNum;

	lRemoteNum = UpdateRemoteInfo();
	lDroneNum = UpdataDroneInfo();

	lTotalNum = lDroneNum + lRemoteNum + lSpectrumNum;

	length = 60 + sizeof(mDroneList[0]) * lTotalNum;
    response_msg->msg_head.magic = 0xFD;
    response_msg->msg_head.len_lo = (length & 0x00FF);
	response_msg->msg_head.len_hi = (length & 0xFF00) >> 8;
	response_msg->msg_head.seq = 0;
    response_msg->msg_head.destid = destid;
    response_msg->msg_head.sourceid = DEV_DRONEID;
    response_msg->msg_head.msgid = 0xEF;
    response_msg->msg_head.ans = 0;

	response_msg->buffer[cnt++] = lTotalNum;//
    memcpy(&response_msg->buffer[cnt], mDroneList, sizeof(mDroneList[0]) * lDroneNum);
	cnt += sizeof(mDroneList[0]) * lDroneNum;
	memcpy(&response_msg->buffer[cnt], mRemoteList, sizeof(mRemoteList[0]) * lRemoteNum);
	cnt += sizeof(mDroneList[0]) * lRemoteNum;
	memcpy(&response_msg->buffer[cnt], mSpectrumList, sizeof(mSpectrumList[0]) * lSpectrumNum);
	cnt += sizeof(mDroneList[0]) * lSpectrumNum;
    response_msg->buffer[cnt ] = '\0';

}

void PacketRemoteInfo(alink_msg_t *response_msg , uint8_t destid )
{
	uint16_t length = 0;
	uint32_t cnt = 0;
	uint8_t lRemoteNum = 0;
	uint8_t lTotalNum = 0;

	memset(response_msg->buffer, 0, RECV_MAX_BUFF);

    memcpy(&response_msg->buffer[cnt], get_device_sn_code(), DEVICE_SN_CODE_LEN);// SN_name
    cnt += DEVICE_SN_CODE_LEN;

	lRemoteNum = UpdateRemoteInfo();

	lTotalNum = lRemoteNum;

	length = 26 + sizeof(mRemoteList[0]) * lTotalNum;
    response_msg->msg_head.magic = 0xFD;
    response_msg->msg_head.len_lo = (length & 0x00FF);
	response_msg->msg_head.len_hi = (length & 0xFF00) >> 8;
	response_msg->msg_head.seq = 0;
    response_msg->msg_head.destid = destid;
    response_msg->msg_head.sourceid = DEV_DRONEID;
    response_msg->msg_head.msgid = 0xE1;
    response_msg->msg_head.ans = 0;

	response_msg->buffer[cnt++] = lTotalNum;//
	memcpy(&response_msg->buffer[cnt], mRemoteList, sizeof(mRemoteList[0]) * lRemoteNum);
	cnt += sizeof(mRemoteList[0]) * lRemoteNum;

    response_msg->buffer[cnt ] = '\0';

}

void PacketSpectrumInfo(alink_msg_t *response_msg , uint8_t destid )
{
	uint16_t length = 0;
	uint32_t cnt = 0;
	uint8_t lSpectrumNum = 0;
	uint8_t lTotalNum = 0;
	uint16_t l_QxPower ;
	uint16_t l_DxPower ;
	uint32_t l_dDxAngle;

	get_dir_para(&l_QxPower,&l_DxPower,&l_dDxAngle);

	memset(response_msg->buffer, 0, RECV_MAX_BUFF);

    memcpy(&response_msg->buffer[cnt], get_device_sn_code(), DEVICE_SN_CODE_LEN);// SN_name
    cnt += DEVICE_SN_CODE_LEN;

    memcpy(&response_msg->buffer[cnt], &l_QxPower, sizeof(uint16_t));// 全向天线功率
    cnt += sizeof(uint16_t);

    memcpy(&response_msg->buffer[cnt], &l_DxPower, sizeof(uint16_t));// 定向天线功率
    cnt += sizeof(uint16_t);

    memcpy(&response_msg->buffer[cnt], &l_dDxAngle, sizeof(uint32_t));// 定向天线水平角(0.01°)
    cnt += sizeof(uint32_t);

    //
    UpdataSpectrum();
	lSpectrumNum = uSpectrumNum;

	lTotalNum = lSpectrumNum;

	length = 34 + sizeof(mSpectrumList[0]) * lTotalNum;
    response_msg->msg_head.magic = 0xFD;
    response_msg->msg_head.len_lo = (length & 0x00FF);
	response_msg->msg_head.len_hi = (length & 0xFF00) >> 8;
	response_msg->msg_head.seq = 0;
    response_msg->msg_head.destid = destid;
    response_msg->msg_head.sourceid = DEV_DRONEID;
    response_msg->msg_head.msgid = 0xE2;
    response_msg->msg_head.ans = 0;

	response_msg->buffer[cnt++] = lTotalNum;//
	memcpy(&response_msg->buffer[cnt], mSpectrumList, sizeof(mSpectrumList[0]) * lSpectrumNum);
	cnt += sizeof(mSpectrumList[0]) * lSpectrumNum;
    response_msg->buffer[cnt ] = '\0';

}

void PacketDroneidInfo(alink_msg_t *response_msg , uint8_t destid )
{
	uint16_t length = 0;
	uint32_t cnt = 0;
	uint8_t lDroneNum = 0;
	uint8_t lTotalNum = 0;

	memset(response_msg->buffer, 0, RECV_MAX_BUFF);

    memcpy(&response_msg->buffer[cnt], get_device_sn_code(), DEVICE_SN_CODE_LEN);// SN_name
    cnt += DEVICE_SN_CODE_LEN;

	lDroneNum = UpdataDroneInfo();

	lTotalNum = lDroneNum ;

	length = 26 + sizeof(mDroneList[0]) * lTotalNum;
    response_msg->msg_head.magic = 0xFD;
    response_msg->msg_head.len_lo = (length & 0x00FF);
	response_msg->msg_head.len_hi = (length & 0xFF00) >> 8;
	response_msg->msg_head.seq = 0;
    response_msg->msg_head.destid = destid;
    response_msg->msg_head.sourceid = DEV_DRONEID;
    response_msg->msg_head.msgid = 0xE0;
    response_msg->msg_head.ans = 0;

	response_msg->buffer[cnt++] = lTotalNum;//
    memcpy(&response_msg->buffer[cnt], mDroneList, sizeof(mDroneList[0]) * lDroneNum);
	cnt += sizeof(mDroneList[0]) * lDroneNum;
    response_msg->buffer[cnt ] = '\0';

}

void PacketDevInfo(alink_msg_t *response_msg , uint8_t destid )
{
	uint16_t length = 0;
	uint32_t cnt = 0;
	uint32_t TimeStamp;
	uint8_t lRemoteNum = 0;
	uint8_t lSpectrumNum = 0;
	uint8_t lDroneNum = 0;
	uint8_t lTotalNum = 0;

	memset(response_msg->buffer, 0, RECV_MAX_BUFF);

	DetectFreq = SYS_GetDetectFreq();

    TimeStamp = (uint32_t)xTaskGetTickCount();
    memcpy(&response_msg->buffer[cnt], &TimeStamp, sizeof(uint32_t));// timeStamp
    cnt += sizeof(uint32_t);

    bat_original_data = 99;//UpdataBatteryCapacityAndDisplayLedState(bat_original_data);
    response_msg->buffer[cnt++] = bat_original_data;//electricity

    response_msg->buffer[cnt++] = GetBatCap();// BatteryStatus

    response_msg->buffer[cnt++] = SYS_GetWorkMode();// workmode

    response_msg->buffer[cnt++] = 0;// workStatus

    response_msg->buffer[cnt++] = 0;// alarmLevel

    memcpy(&response_msg->buffer[cnt], get_device_sn_code(), DEVICE_SN_CODE_LEN);// SN_name
    cnt += DEVICE_SN_CODE_LEN;

	length = cnt ;
    response_msg->msg_head.magic = 0xFD;
    response_msg->msg_head.len_lo = (length & 0x00FF);
	response_msg->msg_head.len_hi = (length & 0xFF00) >> 8;
	response_msg->msg_head.seq = 0;
    response_msg->msg_head.destid = destid;
    response_msg->msg_head.sourceid = DEV_DRONEID;
    response_msg->msg_head.msgid = 0xEF;
    response_msg->msg_head.ans = 0;

    response_msg->buffer[cnt ] = '\0';

}

void TeartbeatPacket_Task(void *p_arg)
{

	static uint32_t count = 0 ;
	vTaskDelay(1000);

	while (1)
	{
		count++;
		vTaskDelay(100);

		if( (count%2) == 0x01 )
		{
#if( WORKMODE == DRONEID)
			PacketDroneidInfo(&mTeartbeatPacketMsg , DEV_C2 );
			AlinkMsgSendRecv(0, &mTeartbeatPacketMsg, NULL, 0);


			PacketRemoteInfo(&mTeartbeatPacketMsg , DEV_C2 );
			AlinkMsgSendRecv(0, &mTeartbeatPacketMsg, NULL, 0);
#else
			PacketSpectrumInfo(&mTeartbeatPacketMsg , DEV_C2 );
			AlinkMsgSendRecv(0, &mTeartbeatPacketMsg, NULL, 0);
#endif

		}

		if( (count%10) == 0x01 )
		{
			PacketDevInfo(&mTeartbeatPacketMsg , DEV_C2 );
			AlinkMsgSendRecv(0, &mTeartbeatPacketMsg, NULL, 0);
		}


	}
}

int32_t InitTeartbeatPacket_Task()
{
	int32_t retVal = -1;


	if (xTaskCreate(TeartbeatPacket_Task, "TeartbeatPacket_Task", TASK_STACK_SIZE_TEARTBEATPACKET, NULL, TASK_PRI_TEARTBEATPACKET, NULL) == 1)
	{
		retVal = 0;
	}

	return retVal;
}

//buzzer:  1  open, 0  close;      motor: 1  open,  0  close
uint8_t GetalarmLevel(uint8_t alarm, uint8_t buzzer, uint8_t motor)
{
	uint8_t AlarmLevelValue = 0;
	AlarmLevelValue = alarm & (0x0f);
	if (buzzer > 0)
	{
		AlarmLevelValue |= 0x10;
	}
	if (motor > 0)
	{
		AlarmLevelValue |= 0x20;
	}

	return AlarmLevelValue;
}

void SetBuzerStatus(IO_STATUS value)
{
	BuzerValue = 0;
	GPIO_OutputCtrl(EMIO_BUZZER_EN, value);
	BuzerValue = value;
}

void SetMotorStatus(IO_STATUS value)
{
	MotorValue = value;
}

void SetAlarmLevel(uint8_t value)
{
	AlarmLevel = value;
}
