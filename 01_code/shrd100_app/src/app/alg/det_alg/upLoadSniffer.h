#ifndef _UPLOADSNIFFER_H__
#define _UPLOADSNIFFER_H__

#include "feature.h"
#include "droneSniffer.h"

#define MaxBlockParam 2000//图块最大参数个数
#define MaxRmtBlock 2000//最多检测的图块个数,Rmt为遥控remote缩写
#define MaxNumRmtT 100//最大遥控图块周期个数

//遥控检测卷积核
static float HpMask[9]={-0.25,-0.25,-0.25,0.25,1.00,0.25,-0.25,-0.25,-0.25};

//卷积核高度
#define HpMaskH 1
//卷积核宽度
#define HpMaskW 9

//遥控图块结构体
struct RmtBlock
{
    int   antIndex;		//天线编号。多接收通道才用到，单通道就是0
    float freq;			//频率，MHz
    float burstTime;	//突发时间，ms
    float blkW;			//图块持续时间，ms
    float blkBW;		//图块对应信号的带宽，MHz
    float meanAmp[NCh];	//图块对应每个天线信号的幅度，dB
};

void printRemoteBlock(struct blockGroup *remoteBlock, int nBlocks, struct DroneLib *droneLib);

void upLoadSniffer(struct blockGroup *remoteBlock, int *nBlocks, float ***specMat, int nRows, int nCols, float cenFreq, float gain, float dt, float dF, struct DroneLib *droneLib, int nLibElem, int *rmtSpan);

void upLoadSniffer_noFilt(struct blockGroup *remoteBlock, int *nBlocks, float ***downMat, float ***upMat, int nRows, int nCols, float cenFreq, float gain, float dt, float dF, struct DroneLib *droneLib, int nLibElem, int *rmtSpan);

#endif

