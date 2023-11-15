
#ifndef PROTOCOL_PARAMETER_H
#define PROTOCOL_PARAMETER_H

#include "protocol_common.h"

#define PROTOCOL_BEAM_SCHEDULING_VER (0x00)
#define PROTOCOL_CFG_PARAM_VER (0x00)

#pragma pack(1)
typedef struct protocol_beam_scheduling
{
	protocol_info_head_t stInfoHeader;

	uint16_t aziBeamID;		// ��λ�����ţ�0~29����30�����ֱ��Ӧ-58�㣺4�㣺58��
	uint16_t eleBeamID;		// ���������ţ�0~3����4�����ֱ��Ӧ-15�㣺10�㣺15��
	int16_t aziBeamSin;		// ��λ����ָ�������ֵ INV_ONE15_SCALE
	int16_t eleBeamSin;		// ��������ָ�������ֵ INV_ONE15_SCALE
	uint16_t tasBeamTotal;	// TAS��λɨ���ܸ�������TAS����ģʽ��Ч
	uint16_t tasBeamCntCur; // TAS��λɨ�赱ǰ��������TAS����ģʽ��Ч

	uint16_t tasObjId;				 // TASĿ���ţ���TAS����ģʽ��Ч
	uint16_t tasObjFilterNum;		 // TASĿ���˲�����TAS�˲����������ģ�TWSʱ��Ч ���˲����ţ�
	uint16_t tasObjRange;			 // TASĿ����뵥Ԫ��,Ŀ����뵥Ԫ��=��Ŀ��ʵ�ʾ���¾��뵥Ԫ��
	uint16_t samplePntStart;		 // ��ʼ�����㣬��ʼ����λ�ã���PRI�������ؿ�ʼ��ʱ����λ��0.2us
	uint16_t samplePntDepth;		 // ������ȣ�����ʼ�����㿪ʼ������Ĭ��4096
	uint16_t beamSwitchTime;		 // �����л�ʱ�䣬Ԥ��״̬ת��ʱ�䣬��λ��0.2us
	uint32_t wholeSpaceScanCycleCnt; // ȫ����ɨ�����ڼ�������0��ʼ��ÿ��������һ��ȫ�������ڼ�����1

	uint16_t trackTwsTasFlag; // ���ٱ�ʶ��0��TWS���٣�1��TAS����
	uint16_t reserve[3];	  // ���ã�Ĭ��Ϊ0

	protocol_info_tail_t stInfoTail;
} protocol_beam_scheduling_t;

typedef struct protocol_cfg_param
{
	protocol_info_head_t stInfoHeader;

	uint8_t trSwitchCtrl; // ������ؿ��ƣ�FF��ȫ����00��ȫ�أ�01��ֻ���м�1/8��ͨ����02�����м�1/4��ͨ����04����1/2��ͨ����08��������ͨ����������ȫ�أ�
	uint8_t workMode;	  // ������ʽ��0x00������ģʽ�� 0x01������ģʽ��
	uint8_t workWaveCode; // ����������
	uint8_t workFrqCode;  // ����Ƶ�㣬0-2����������Ƶ�ʵ㣻
	uint8_t prfPeriod;	  // PRF���ڣ�0x00��1#PRF�ţ�0x01��2#PRF��
	uint8_t accuNum;	  // ���۵������ƣ�0Ϊ64�㣬1Ϊ32�㣬Ĭ��1

	uint16_t noiseCoef;	  // ��������ϵ�� INV_ONE7_SCALE
	uint16_t clutterCoef; // �Ӳ�ͼ����ϵ�� INV_ONE7_SCALE
	uint16_t cfarCoef;	  // ���龯����ϵ�� INV_ONE7_SCALE

	uint16_t focusRangeMin; // �ص��ע���뷶Χ���ޣ���С���þ��뵥Ԫ
	uint16_t focusRangeMax; // �ص��ע���뷶Χ���ޣ�������þ��뵥Ԫ

	int16_t clutterCurveNum; // ���Ӳ�������ֵ��Χ[0,63]
	int16_t lobeCompCoef;	 // ����ѹ����������С0.5�����4 INV_ONE7_SCALE

	uint8_t cohesionVelThre;	  // �����ٶ����ޣ��Ŵ������ٶ��˲������ޣ�ֵ��Χ[0,63]
	uint8_t cohesionRgnThre;	  // ���۾������ޣ��Ŵ������������ޣ���λ�����뵥Ԫ��
	uint8_t clutterMapSwitch;	  // �Ӳ�ͼ���أ�0Ϊ�򿪣�1Ϊ�رգ�Ĭ��1
	uint8_t clutterMapUpdateCoef; // �Ӳ�ͼ����ϵ�����޷�������ֵ��Χ[0,255]

	int8_t aziCalcSlope; // ��λ���б�ʣ�ֵ��Χ[-128,127]
	int8_t aziCalcPhase; // ��λ�����λ��ֵ��Χ[0,36]
	int8_t eleCalcSlope; // �������б�ʣ�ֵ��Χ[-128,127]
	int8_t eleCalcPhase; // ���������λ��ֵ��Χ[0,36]

	int8_t aziScanCenter; // ��λɨ�����ģ� -60�㡫+60�㣬Ĭ��Ϊ0
	uint8_t aziScanScope; // ��λɨ�跶Χ�� 0��20��40��90��120��Ĭ��Ϊ120

	int8_t eleScanCenter; // ����ɨ�����ģ�-20�㡫+20�㣬Ĭ��Ϊ0
	uint8_t eleScanScope; // ����ɨ�跶Χ��0��10��20��40��Ĭ��Ϊ40

	uint16_t coherentDetectSwitch; // ��ؼ�⿪�أ�СĿ����ؼ�⿪�أ� 1��������غ��ϱ�����0����أ�����أ�ֱ���ϱ�����Ĭ��Ϊ0

	uint16_t reserve[3]; // ���ã�Ĭ��Ϊ0

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
