
#ifndef PROTOCOL_PARAMETER_H
#define PROTOCOL_PARAMETER_H

#include "protocol_common.h"

#define PROTOCOL_BEAM_SCHEDULING_VER (0x00)
#define PROTOCOL_CFG_PARAM_VER (0x00)

#pragma pack(1)
typedef struct protocol_beam_scheduling
{
	protocol_info_head_t stInfoHeader;

	uint16_t aziBeamID;		// 方位波束号，0~29，共30个，分别对应-58°：4°：58°
	uint16_t eleBeamID;		// 俯仰波束号，0~3，共4个，分别对应-15°：10°：15°
	int16_t aziBeamSin;		// 方位波束指向的正弦值 INV_ONE15_SCALE
	int16_t eleBeamSin;		// 俯仰波束指向的正弦值 INV_ONE15_SCALE
	uint16_t tasBeamTotal;	// TAS波位扫描总个数，仅TAS跟踪模式有效
	uint16_t tasBeamCntCur; // TAS波位扫描当前个数，仅TAS跟踪模式有效

	uint16_t tasObjId;				 // TAS目标编号，仅TAS跟踪模式有效
	uint16_t tasObjFilterNum;		 // TAS目标滤波器，TAS滤波器波门中心，TWS时无效 （滤波器号）
	uint16_t tasObjRange;			 // TAS目标距离单元数,目标距离单元数=（目标实际距离÷距离单元）
	uint16_t samplePntStart;		 // 起始采样点，起始采样位置，从PRI的上升沿开始计时，单位：0.2us
	uint16_t samplePntDepth;		 // 采样深度，从起始采样点开始计数，默认4096
	uint16_t beamSwitchTime;		 // 波束切换时间，预留状态转换时间，单位：0.2us
	uint32_t wholeSpaceScanCycleCnt; // 全空域扫描周期计数，从0开始，每次搜索完一个全空域周期计数加1

	uint16_t trackTwsTasFlag; // 跟踪标识，0：TWS跟踪；1：TAS跟踪
	uint16_t reserve[3];	  // 备用，默认为0

	protocol_info_tail_t stInfoTail;
} protocol_beam_scheduling_t;

typedef struct protocol_cfg_param
{
	protocol_info_head_t stInfoHeader;

	uint8_t trSwitchCtrl; // 组件开关控制，FF：全开，00：全关；01：只打开中间1/8个通道；02：打开中间1/4个通道；04：打开1/2个通道；08：打开所有通道；其他：全关；
	uint8_t workMode;	  // 工作方式，0x00：搜索模式， 0x01：跟踪模式；
	uint8_t workWaveCode; // 工作波形码
	uint8_t workFrqCode;  // 工作频点，0-2，共计三个频率点；
	uint8_t prfPeriod;	  // PRF周期，0x00：1#PRF号；0x01：2#PRF号
	uint8_t accuNum;	  // 积累点数控制，0为64点，1为32点，默认1

	uint16_t noiseCoef;	  // 噪声门限系数 INV_ONE7_SCALE
	uint16_t clutterCoef; // 杂波图门限系数 INV_ONE7_SCALE
	uint16_t cfarCoef;	  // 恒虚警门限系数 INV_ONE7_SCALE

	uint16_t focusRangeMin; // 重点关注距离范围下限，最小作用距离单元
	uint16_t focusRangeMax; // 重点关注距离范围上限，最大作用距离单元

	int16_t clutterCurveNum; // 抠杂波点数，值域范围[0,63]
	int16_t lobeCompCoef;	 // 波瓣压缩，输入最小0.5，最大4 INV_ONE7_SCALE

	uint8_t cohesionVelThre;	  // 凝聚速度门限，信处凝聚速度滤波器门限，值域范围[0,63]
	uint8_t cohesionRgnThre;	  // 凝聚距离门限，信处距离凝聚门限，单位：距离单元；
	uint8_t clutterMapSwitch;	  // 杂波图开关，0为打开，1为关闭，默认1
	uint8_t clutterMapUpdateCoef; // 杂波图更新系数，无符号数，值域范围[0,255]

	int8_t aziCalcSlope; // 方位测角斜率，值域范围[-128,127]
	int8_t aziCalcPhase; // 方位测角相位，值域范围[0,36]
	int8_t eleCalcSlope; // 俯仰测角斜率，值域范围[-128,127]
	int8_t eleCalcPhase; // 俯仰测角相位，值域范围[0,36]

	int8_t aziScanCenter; // 方位扫描中心， -60°～+60°，默认为0
	uint8_t aziScanScope; // 方位扫描范围， 0、20、40、90、120，默认为120

	int8_t eleScanCenter; // 俯仰扫描中心，-20°～+20°，默认为0
	uint8_t eleScanScope; // 俯仰扫描范围，0、10、20、40，默认为40

	uint16_t coherentDetectSwitch; // 相关检测开关，小目标相关检测开关： 1代表开（相关后上报），0代表关（不相关，直接上报），默认为0

	uint16_t reserve[3]; // 备用，默认为0

	protocol_info_tail_t stInfoTail;
} protocol_cfg_param_t;
#pragma pack()

typedef void (*cfg_param_cb_fp)(protocol_cfg_param_t *cfg);

/**
 * caller: data processing module
 * description: send beam scheduling to host
 * */
ret_code_t protocol_send_beam_scheduling(protocol_beam_scheduling_t *beam_scheduling);

/**
 * caller: data processing module
 * description: register (set) the callback function to process the configuration from the host
 * */
ret_code_t protocol_cfg_param_reg_cb(cfg_param_cb_fp cb_fp);

#endif /* PROTOCOL_PARAMETER_H */
