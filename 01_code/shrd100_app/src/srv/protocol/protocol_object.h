
#ifndef PROTOCOL_OBJECT_H
#define PROTOCOL_OBJECT_H

#include "protocol_common.h"

#define MAX_TARGET_NUM RADAR_TARGET_NUM_MAX

#define PROTOCOL_OBJECT_LIST_DETECTED_VER (0x00)
#define PROTOCOL_OBJECT_LIST_TRACKED_VER (0x00)

#pragma pack(1)
typedef struct protocol_object_item_detected
{
	uint16_t id;
	int16_t azimuth;		 // unit:deg INV_ONE6_SCALE
	uint32_t range;			 // unit:m INV_ONE6_SCALE
	int16_t elevation;		 // unit:deg INV_ONE6_SCALE
	int16_t velocity;		 // unit:m/s INV_ONE6_SCALE
	int16_t dopplerChn;		 //目标多普勒号
	uint16_t mag;			 // unit:db INV_ONE6_SCALE
	uint16_t objConfidence;	 //目标置信度 INV_ONE6_SCALE
	uint8_t ambiguous;		 // 0速度无模糊 1速度存在模糊
	uint8_t classification;	 // 0未识别 1无人机 2单兵 3车辆 4鸟类 5直升机 其它无效
	uint8_t clasafyProb;	 //目标类别概率 INV_ONE6_SCALE
	uint8_t cohesionOkFlag;	 //凝聚完成标记，0：未完成；1：完成
	uint8_t cohesionPntNum;	 //凝聚点数
	uint8_t cohesionBeamNum; //凝聚波位个数
	uint16_t aziBeamID;		 //方位波束号，0~29 共30个，对应 -58°：4°：58°
	uint16_t eleBeamID;		 //俯仰波束号，0~3 共4个，对应 -15°：10°：15°
	uint16_t reserve[4];	 //备用，默认为0
} protocol_object_item_detected_t;

typedef struct protocol_object_list_detected
{
	protocol_info_head_t stInfoHeader;

	uint16_t detectObjNum;	  /* 检测目标个数，最大目标个数64 */
	uint16_t detectObjByte;	  /*单个检测目标长度，其长度由检测点信息定义决定 */
	uint16_t trackTwsTasFlag; /* 跟踪标识，0：TWS跟踪；1：TAS跟踪 */
	uint16_t reserved;

	protocol_object_item_detected_t detectPoint[MAX_TARGET_NUM];

	protocol_info_tail_t stInfoTail;
} protocol_object_list_detected_t;

typedef struct protocol_object_item_tracked
{
	uint16_t id;
	int16_t azimuth;		  // INV_ONE6_SCALE
	uint32_t range;			  // INV_ONE6_SCALE
	int16_t elevation;		  // INV_ONE6_SCALE
	int16_t velocity;		  //目标径向速度m/s INV_ONE6_SCALE
	int16_t dopplerChn;		  //目标多普勒号
	uint16_t mag;			  //目标幅度值 INV_ONE6_SCALE
	uint8_t ambiguous;		  // 0速度无模糊 1速度存在模糊
	uint8_t classification;	  // 0未识别 1无人机 2单兵 3车辆 4鸟类 5直升机 其它无效
	uint8_t classifyProb;	  //目标类别概率 INV_ONE6_SCALE
	uint8_t existingProb;	  //目标存在概率 INV_ONE6_SCALE
	int16_t absVel;			  //目标位移速度m/s INV_ONE6_SCALE
	int16_t orientationAngle; //目标航向,目标运动方向与真北夹角（°）INV_ONE6_SCALE
	uint16_t alive;			  //目标周期数
	uint16_t twsTasFlag;	  //跟踪标识，0：TWS跟踪；1：TAS跟踪

	int32_t x; // x相对坐标（m）INV_ONE6_SCALE
	int32_t y; // y相对坐标（m）INV_ONE6_SCALE
	int32_t z; // z相对坐标（m）INV_ONE6_SCALE

	int16_t vx; // x方向相对速度（m/s）INV_ONE6_SCALE
	int16_t vy; // y方向相对速度（m/s）INV_ONE6_SCALE
	int16_t vz; // z方向相对速度（m/s）INV_ONE6_SCALE
	int16_t ax; // x方向相对加速度（m/s²）INV_ONE6_SCALE
	int16_t ay; // y方向相对加速度（m/s²）INV_ONE6_SCALE
	int16_t az; // z方向相对加速度（m/s²）INV_ONE6_SCALE

	uint16_t x_variance;  // x相对坐标方差 INV_ONE6_SCALE
	uint16_t y_variance;  // y相对坐标方差 INV_ONE6_SCALE
	uint16_t z_variance;  // z相对坐标方差 INV_ONE6_SCALE
	uint16_t vx_variance; // x方向相对速度方差 INV_ONE6_SCALE
	uint16_t vy_variance; // y方向相对速度方差 INV_ONE6_SCALE
	uint16_t vz_variance; // z方向相对速度方差 INV_ONE6_SCALE
	uint16_t ax_variance; // x方向相对加速度方差 INV_ONE6_SCALE
	uint16_t ay_variance; // y方向相对加速度方差 INV_ONE6_SCALE
	uint16_t az_variance; // z方向相对加速度方差 INV_ONE6_SCALE

	uint8_t stateType;	//目标航迹类型，0：暂态航迹；1：稳态航迹
	uint8_t motionType; //运动类型 0未知 1静止 2悬停 3靠近 4远离

	uint16_t forcastFrameNum; //目标预测帧数
	uint16_t associationNum;  //目标关联的检测点个数
	uint32_t assocBit0;		  //关联检测点ID
	uint32_t assocBit1;		  //关联检测点ID
	uint16_t reserve[4];	  //备用
} protocol_object_item_tracked_t;

typedef struct protocol_object_list_tracked
{
	protocol_info_head_t stInfoHeader;

	uint16_t trackObjNum;
	uint16_t trackTwsNum;
	uint16_t trackTasNum;
	uint16_t trackObjByte;
	uint16_t trackTwsTasFlag; // 0 TWS跟踪  1 TAS跟踪
	uint16_t reserved;

	protocol_object_item_tracked_t trackPoint[MAX_TARGET_NUM];

	protocol_info_tail_t stInfoTail;
} protocol_object_list_tracked_t;
#pragma pack()

ret_code_t protocol_send_object_list_detected(protocol_object_list_detected_t *obj_list);
ret_code_t protocol_send_object_list_tracked(protocol_object_list_tracked_t *obj_list);

#endif /* PROTOCOL_OBJECT_H */
