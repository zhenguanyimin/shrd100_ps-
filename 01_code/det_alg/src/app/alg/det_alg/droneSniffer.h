#ifndef _DRONESNIFFER_H__
#define _DRONESNIFFER_H__

#include "feature.h"
#include "utils.h"
#include "preProcess.h"

#define MaxDrone 20
#define MaxPulseInGroup 120
#define MaxObjNum 20
#define MaxHistoryNum 1000
#define MaxPulseTimeNum 16

#define sfM1 128// 标准数据行数
#define sfM2 3000// 标准数据列数

#define PI 3.1415926535898

#define MaxDroneinLib 30
#define MaxPulseWNumInLib 5
#define MaxNumVidT 50
#define MaxPulseInObj 100
#define MaxVidPulse 50

//struct BlockRow
//{
//	int   id;
//	int   uavIndex;
//	int   antIndex;
//	char  inCh[NCh];
//	int   colIndex;
//	int   allColIndex[NFFT / N];
//	int   nCol;
//	int   q1[MaxPulseInGroup];
//	int   w[MaxPulseInGroup];
//	float meanColAmp;
//	float snr;
//	int   nw;
//};

struct blockGroup
{
    int   id;
    int   index;
    float range;
    char  inCh[NCh];
    float freq[MaxPulseInGroup];
    float bw;
    float burstTime[MaxPulseInGroup];
    float width[MaxPulseInGroup];
    float amp[NCh][MaxPulseInGroup];
    float dist[MaxPulseInGroup];
    int   n;
};

typedef struct droneResult
{
    int ID;             //
    char name[50];      //
	int psbID[10];		//可能的机型序号
	int nPsbID;			//机型序号可能性数量
    char flag;          //
    float freq[128];     //
    float burstTime[128];//
    float width[128];    //
    float bw;           //
    float amp;          //
    float range;        //
}droneResult_t;

void printResult(struct droneResult *droneInfo, int nDrone);


void sfDroneSnifferV0(struct droneResult *droneInfo, int *nDrone, unsigned short ***downMat, unsigned short ***upMat, float cenFreq, float fs, float bw, float gain, int calRows);
void sfDroneSnifferV1(struct droneResult *droneInfo, int *nDrone, unsigned short ***downMat, unsigned short ***upMat, float cenFreq, float fs, float bw, float gain, int calRows);

//void sfDownLoadSniffer(struct blockGroup *droneBlock, int *nBlock, unsigned short ***downMat, int chn, int nRows, int nCols, float cenFreq, float gain, float dT, float dF, struct DroneLib *UAVtypes, int nUav);
float sfAutoThreshOnMapV0(unsigned short ***downMat, int chnum, int d128, int d3000, float dT, int maxIter);
float sfAutoThreshOnMapV1(unsigned short ***downMat, int chnum, int d128, int d3000, float dT, int maxIter);
void sfThresholdCompare(float* thresh, float cenFreq, int tdB, int mdB);
void sfFrequenceHist(unsigned short ***downMat, int chnum, int* hist, int d128, int d3000, float threshold);
float sfAutoThreshOnLineI(int *line, int d128, float dT, int maxIter);
float sfAutoThreshOnLineF(float *line, int d128, float dT, int maxIter);
int sfCalculateFrequenceSE(int *line, int d128, float dT, int lineWidthT, int *lineS, int *lineE);
int sfThresholdToPulse(float *s, int n, float thresh, int delt, int minw, int *q1, int *w, float *colSnr, float *colAm);

//int sfVidBlocksDetect(struct BlockRow *aBlkRow, float *s, int n, int *q1, int *w, int nw, float *colSnr, int antIndex, int colIndexS, int colIndexE, float cenFreq, float dT, float dF, struct DroneLib *UAVtypes, int nUAV);
void sfMatchPulseT(int **match, float *t2, int *index, int n, float *pulseT, int nPulseT, float pulseTErr);
//void sfPulseToTarget(struct blockGroup *droneBlock, int nBlock, struct BlockRow aBlkRow, int fstart, int fend, float cenFreq, float dT, float dF, struct DroneLib *UAVtypes);
//void sfSetResult(struct droneResult *droneInfo, int *nDrone, struct DroneLib *droneLIB, struct blockGroup *uavPulse, int nUavPulse, struct blockGroup *uavCtrPulse, int nUavCtrPulse);


float sfDxCalCulateAm(unsigned short ***AmPower, int StartLine, int EndLine, int numUav, int calRows);
//float sfDxDronePowerV0(unsigned short ***AmPower, int StartLine, int EndLine, struct DroneLib *UAVtypes, int numUav, int calRows);
//float sfDxDronePowerV1(unsigned short ***AmPower, int StartLine, int EndLine, struct DroneLib *UAVtypes, int numUav, int calRows);
int sfDxThresholdWithAm(float *s, int n, float thresh, int delt, int minw, int *q1, int *w, float *colAm);


// 计算全向幅度匹配模板
int sfDxNewGetIndex(unsigned short ***AmPower, int StartLine, int EndLine, struct DroneLib *UAVtypes, int numUav, int calRows, int *ps1, int *pw1, int *pn1, int *ps2, int *pw2, int *pn2);
//int sfDxNewGetIndex(unsigned short ***AmPower, int StartLine, int EndLine, struct DroneLib *UAVtypes, int numUav, int calRows, int *ps, int *pw);
//float sfDxNewCalculateAm(unsigned short ***AmPower, int StartLine, int EndLine, int calRows, int *ps, int *pw, int pnum);
int sfDxStatusTransform(float *listAngleAz, float *listAngleEl, float *listAmQ, float *listAmD, float *listAmC, int *listFP, int numDx, int *statusN, float *statusFW1, float *statusFW2, float *statusFW3);
// 主进程函数
int sfDxNewProcessing(unsigned short ***AmPowerQ, unsigned short ***AmPowerD, int StartLine,
		int EndLine, int numUav, int calRows, float *listAngle, float *listAmQ, float *listAmD, float *listAmC, int *listFP, int numDx);

int sfDxNewWifiProcessing(float ampQ, float ampD, float *listAngle, float *listAmQ, float *listAmD, float *listAmC, int *listFP, int numDx);
void sfMoniSniffer(struct blockGroup *droneBlock, int *nBlock, unsigned short ***downMat, int chn, int nRows, int nCols, float cenFreq, float gain, float dT, float dF, float *moniF, int nMoni, float bwSignal, float bwDetect);
void sfMoniToTarget(struct blockGroup *droneBlock, int nBlock, float cenFreq, float tarFreq, float tarBW, float tarAm);

#endif

