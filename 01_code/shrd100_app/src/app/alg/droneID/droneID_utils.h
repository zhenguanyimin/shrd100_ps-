#ifndef _DRONEID_UTILS_H_
#define _DRONEID_UTILS_H_

#include "droneID_system_param.h"
#include "../../../srv/log/log.h"

/*==================================================================================================
*                                      DEFINES AND MACROS
==================================================================================================*/
#define DEFAULT_DRONEID_REMOVE_THRE		4 /* default continue lost count, threshold of to remove droneID in multiple list */
#define DEFAULT_DRONEID_REMOVE_THRE_LOST_DUR		6000 /* default continue lost duration(unit = 1ms) threshold, to remove droneID in multiple list */
#define DEFAULT_TURBO_ITERATIONS		4


/*=================================================================================================
*                                STRUCTURES AND OTHER TYPEDEFS
=================================================================================================*/
typedef enum
{
	NONE 							= 0,	/* do not print algorithm result */
	PRINT_DRONEID_RIGHT_ONLY 		= 1,	/* print droneID right result only */
	PRINT_DRONEID_RIGHT_AND_WRONG 	= 2,	/* print droneID right and wrong result */
	PRINT_MULTI_DRONEID				= 3,	/* print multiple droneID result */
	PRINT_BURST_PROCESS_TIME_COST	= 4,	/* print burst process time cost only */
	PRINT_HEART_BEAT_DATA			= 5,	/* print heart beat data */
	ALG_PRINT_MODE_NUM
} eAlgPrintMode;

typedef enum
{
	ALL 							= 0,	/* all of MAX_NUM_DRONE_ID */
	DRONEID_OUTPUT_SN_EXHIBITION	= 1,	/* sn of ABU DHABI exhibition */
	DRONEID_OUTPUT_SN_INTERTEST		= 2,	/* sn of internal test */
	DRONEID_OUTPUT_SN_MODE_NUM
} eDroneIDOutputSnMode;

typedef enum
{
	DRONEID_OUTPUT_LIST_ORIGIN 		= 0,	/* output origin parse droneid list */
	DRONEID_OUTPUT_LIST_PREDICT		= 1,	/* output after predict droneid list */
	DRONEID_OUTPUT_LIST_MODE_NUM
} eDroneIDOutputListMode;

typedef enum
{
	WHITE_LIST 						= 0,	/* white list */
	BLACK_LIST						= 1,	/* black list */
} eBlackWhiteList;


/*==================================================================================================
*                                GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/
extern uint32_t g_droneIDProcFrameCnt;
extern uint32_t g_droneIDCrcErrorCnt;


/*==================================================================================================
*                                    FUNCTION PROTOTYPES
==================================================================================================*/
void SetAlgorithmRunFlag(uint8_t value);
uint8_t GetAlgorithmRunFlag(void);
void SetFreqOffsetHz(int32_t freqOffset);
int32_t GetFreqOffsetHz(void);
void SetDroneIDSigFreqHz(uint64_t freq);
uint64_t GetDroneIDSigFreqHz(void);
void SetAlgDebugPrintMode(int32_t printMode);
int32_t GetAlgDebugPrintMode(void);
void SetDroneIDOutputSnMode(int32_t snMode);
int32_t GetDroneIDOutputSnMode(void);
void SetDroneIDOutputListMode(int32_t listMode);
int32_t GetDroneIDOutputListMode(void);
void SetDroneIDRemoveThre(int32_t threValue);
int32_t GetDroneIDRemoveThre(void);
void SetDroneIDRemoveThreLostDur(int32_t threValue);
int32_t GetDroneIDRemoveThreLostDur(void);
void SetDroneIDTurboDecIterNum(int32_t iterNum);
int32_t GetDroneIDTurboDecIterNum(void);
void ResetDroneIDProcFrameCnt(void);
uint32_t GetDroneIDProcFrameCnt(void);
void ResetDroneIDCrcErrorCnt(void);
uint32_t GetDroneIDCrcErrorCnt(void);
Cplxint32_t InnerProductInt32(Cplxint32_t *x, Cplxint32_t *y, uint32_t length);
Cplxf_t InnerProductFloat(Cplxf_t *x, Cplxf_t *y, uint32_t length);
Cplxf_t CalcCplxfVectorMean(Cplxf_t *vector, uint32_t length);
float CalcDegToRad(float degree);
float GetDistance(float lat1, float lon1, float alt1, float lat2, float lon2, float alt2);
float CalcCplxfVectorVar(Cplxf_t *vector, uint32_t length);


#endif /* _DRONEID_UTILS_H_ */
