#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "preProcess.h"
#include "utils.h"
#include "droneSniffer.h"
#include "feature.h"
#include "downLoadSniffer.h"

//图传检测主函数-外部调用
void sfDownLoadSniffer(struct blockGroup *droneBlock, int *nBlock, unsigned short ***downMat, int chn, int nRows, int nCols, float cenFreq, float gain, float dT, float dF, struct DroneLib *UAVtypes, int nUav);
// 脉冲参数转目标参数
void sfPulseToTarget(struct blockGroup *droneBlock, int nBlock, struct BlockRow aBlkRow, int fstart, int fend, float cenFreq, float dT, float dF, struct DroneLib *UAVtypes);
//图传图块检测
int sfVidBlocksDetect(struct BlockRow *aBlkRow, float *s, int n, int *q1, int *w, int nw, float *colSnr, int antIndex, int colIndexS, int colIndexE, float cenFreq, float dT, float dF, struct DroneLib *UAVtypes, int nUAV);
//对外接口
void sfSetResult(struct droneResult *droneInfo, int *nDrone, struct DroneLib *droneLIB, struct blockGroup *uavPulse, int nUavPulse, struct blockGroup *uavCtrPulse, int nUavCtrPulse);
// 计算幅度-多行平均法
float sfDxDronePowerV1(unsigned short ***AmPower, int StartLine, int EndLine, struct DroneLib *UAVtypes, int numUav, int calRows);
// 计算模板下匹配幅度
float sfDxNewCalculateAm(unsigned short ***AmPower, int StartLine, int EndLine, int calRows, int *ps, int *pw, int pnum);

//特征库，包含图传和遥控
static struct DroneLib droneLIB[30]=
{
	//download
	{"DJI OcuSync2         ",{0,1,2,3,5,6},6,0,{2400,2483.5},2,0,0,{2.1,3.1,4.1,5.1},4,0,{4,6},2,{10,18},2,{0.2,0.2,0.2,0.2},{1,3,0,3},0.2,8,0,1,0,0.758},
	{"DJI OcuSync2         ",{0,1,2,3,5,6},6,0,{5725,5850},2,0,0,{2.1,3.1,4.1,5.1},4,0,{4,6},2,{10,18},2,{0.2,0.2,0.2,0.2},{1,3,0,3},0.2,8,0,1,0,0.758},

	{"DJI OcuSync3         ",{1,2,5,6},4,0,{2400,2483.5},2,0,0,{1.1,2.1},2,0,{2,4,4},3,{10,18},2,{0.2,0.2},{14,4},0.2,30,0,1,0,0.41},
	{"DJI OcuSync3         ",{1,2,5,6},4,0,{5725,5850},2,0,0,{1.1,2.1},2,0,{2,4,4},3,{10,18},2,{0.2,0.2},{14,4},0.2,20,0,1,0,0.41},

	{"Autel EVO II         ",{14},1,0,{2400,2483.5},2,0,0,{4},1,1,{5},1,{9.4},1,{0.15},{12},0.2,16,0,1,0,0.5},// 20230624更改数据库参数
	{"Autel EVO II         ",{14},1,0,{5725,5850},2,0,0,{4},1,1,{5},1,{9.4},1,{0.15},{12},0.2,16,0,1,0,0.5},// 20230624更改数据库参数

	{"Autel EVO II V2      ",{15},1,0,{2400,2483.5},2,0,0,{10.27},1,1,{14},1,{10},1,{0.3},{5},0.2,4,0,1,0,0.7},
	{"Autel EVO II V2      ",{15},1,0,{5725,5850},2,0,0,{10.27},1,1,{14},1,{10},1,{0.3},{5},0.2,4,0,1,0,0.7},


	{"Autel Max4T          ",{20},2,0,{1427.9,1447.9},2,0,0,{0.9},1,1,{1,1,1,2},4,{9.4},1,{0.15},{40},0.15,25,0,1,0,0.5},// 20230624更改数据库参数
	{"Autel V3/M4T         ",{18,20},2,0,{832,862},2,0,0,{0.9},1,1,{1,1,1,2},4,{9.4},1,{0.15},{40},0.15,25,0,1,0,0.5},// 20230624更改数据库参数
	{"Autel V3/M4T         ",{18,20},2,0,{902,928},2,0,0,{0.9},1,1,{1,1,1,2},4,{9.4},1,{0.15},{40},0.15,25,0,1,0,0.5},// 20230624更改数据库参数
	{"Autel Lt/V3/M4T      ",{18,18,20},1,0,{2400,2483.5},2,0,0,{0.9},1,1,{1,1,1,2},4,{9.4},1,{0.15},{40},0.15,25,0,1,0,0.5},// 20230624更改数据库参数
	{"Autel Lt/V3/M4T      ",{18,18,20},1,0,{5725,5850},2,0,0,{0.9},1,1,{1,1,1,2},4,{9.4},1,{0.15},{40},0.15,25,0,1,0,0.5},// 20230624更改数据库参数
	{"Autel Lt/V3/M4T      ",{18,18,20},1,0,{5150,5250},2,0,0,{0.9},1,1,{1,1,1,2},4,{9.4},1,{0.15},{40},0.15,25,0,1,0,0.5},// 20230624更改数据库参数
	{"Autel V3/M4T         ",{18,20},1,0,{4850,5150},2,0,0,{0.9},1,1,{1,1,1,2},4,{9.4},1,{0.15},{40},0.15,25,0,1,0,0.5},// 20230624更改数据库参数
	{"Autel V3/M4T         ",{18,20},1,0,{5250,5725},2,0,0,{0.9},1,1,{1,1,1,2},4,{9.4},1,{0.15},{40},0.15,25,0,1,0,0.5},// 20230624更改数据库参数
	{"Autel V3/M4T         ",{18,20},1,0,{5850,6000},2,0,0,{0.9},1,1,{1,1,1,2},4,{9.4},1,{0.15},{40},0.15,25,0,1,0,0.5},// 20230624更改数据库参数

	{"Autel EVO Nano       ",{18,19},2,0,{2400,2483.5},2,0,0,{0.9},1,1,{1,1,3},3,{9.4},1,{0.15},{40},0.15,25,0,1,0,0.5},// 20230624更改数据库参数
	{"Autel EVO Nano       ",{18,19},2,0,{5725,5850},2,0,0,{0.9},1,1,{1,1,3},3,{9.4},1,{0.15},{40},0.15,25,0,1,0,0.5},// 20230624更改数据库参数
	{"Autel EVO Nano       ",{18,19},2,0,{5150,5250},2,0,0,{0.9},1,1,{1,1,3},3,{9.4},1,{0.15},{40},0.15,25,0,1,0,0.5},// 20230624更改数据库参数

	{"DJI Phantom4 V2      ",{12},6,0,{2400,2483.5},2,0,0,{3.5,5.5},2,0,{4,6},2,{10,18},2,{0.15,0.15},{8,8},0.2,16,0,1,0,0.758},//David20230807增加
	{"DJI Phantom4 V2      ",{12},6,0,{5725,5850  },2,0,0,{3.5,5.5},2,0,{4,6},2,{10,18},2,{0.15,0.15},{8,8},0.2,16,0,1,0,0.758},//David20230807增加
	{"DJI Phantom4 RTK     ",{13},1,0,{2400,2483.5},2,0,0,{1,2,4,5},4,0,{3,17},2,{8.5},1,{0.2,0.2,0.2,0.2},{1,2,0,3},0.5,8,0,1,0,0.758},//David20230808增加
	{"DJI Phantom4 RTK     ",{13},1,0,{5725,5850  },2,0,0,{1,2,4,5},4,0,{3,17},2,{8.5},1,{0.2,0.2,0.2,0.2},{1,2,0,3},0.5,8,0,1,0,0.758},//David20230808增加
	
	//20230821增加数字图传报文
	{"Digital FPV-1        ",{21},1,0,{5635,5950  },2,0,0,{5.3},1,1,{7.2},1,{18},1,{0.2},{12},0.2,12,0,1,0,0.5},

	//David20230807，模拟图传数据库-自采FPV数据库-仅用于数据调用，频段值无法在正常数字图传中匹配
	{"Modular FPV          ",{21},1,0,{5999,6000},2,0,0,{1,2,4,5},4,0,{3,17},2,{10,18},2,{0.2,0.2,0.2,0.2},{1,3,1,3},0.15,7,0,1,0,0.35},// David20230807更改数据库参数

};

static int nLib = 25;  //20230821,增加数字FPV一个24+1，不包括FPV描述报文
static float UAVtypesSNR = 0.0f;

// 20230816 FPV机型频点合并
static float listFrequence[30] = { 842,915, 1080,1120,1160,1200,1240,1280,1320,1360,1437,2422,2437,2462,5160,5180,5200,5220,5240,5655,5695,5725,5745,5765,5785,5805,5825,5850,5890,5930};
static float listNoice[30] = {12410,12375, 12445,13795,13595,14170,13805,13755,13545,13570,13645,13080,13055,13525,12190,12155,12180,12230,12165,12235,12240,12255,12250,12250,12235,12215,12225,12280,12260,12295};
static int listNum = 30;

static float corQD = 0 * 85;// 定向路与全向路增益差，定向路高则为正

// 20230821-R-扩展40个点
// 20230816 模拟图传处理-增加1.2G频段
static float moniF[88] = { 1080,1120,1160,1200,1240,1280,1320,1360,5642.5,5655.5,5662.5,5682.5,5692.5,5702.5,5722.5,5729.5,5730.5,5737.5,5742.5,5749.5,5757.5,5762.5,5766.5,5768.5,5777.5,5782.5,5787.5,5797.5,5802.5,5803.5,5806.5,5817.5,5822.5,5825.5,5837.5,5840.5,5842.5,5844.5,5857.5,5862.5,5863.5,5877.5,5877.5,5882.5,5902.5,5914.5,5922.5,5942.5, 5865,5733,5705,5740,5658,5845,5752,5685,5760,5695,5825,5771,5665,5780,5732,5805,5790,5645,5800,5769,5785,5809,5885,5820,5806,5765,5828,5905,5840,5843,5745,5847,5925,5860,5880,5725,5866,5945,5880,5917 };
// 20230816 针对每个频点单独设置噪底值
static float moniT[88] = { 13300, 13300, 13300, 13300, 13300, 13300, 13300, 13300,11170,11170,11170,11170,11170,11170,11170,11170,11170,11170,11170,11170,11170,11170,11170,11170,11170,11170,11170,11170,11170,11170,11170,11170,11170,11170,11170,11170,11170,11170,11170,11170,11170,11170,11170,11170,11170,11170,11170,11170,  11170,11170,11170,11170,11170,11170,11170,11170,11170,11170,11170,11170,11170,11170,11170,11170,11170,11170,11170,11170,11170,11170,11170,11170,11170,11170,11170,11170,11170,11170,11170,11170,11170,11170,11170,11170,11170,11170,11170,11170 };
static int moniNum = 88;


//打印参数
void printResult(struct droneResult *droneInfo, int nDrone)
{
	for (int i = 0; i < nDrone; i++)
	{
		printf("---------------------------------\n");
		if (droneInfo[i].flag == 0)
		{
			//continue;
			printf("drone:\n");
		}
		else
		{
			//continue;
			printf("remote:\n");
		}

		printf("ID=%d\n", droneInfo[i].ID);
		printf("name=%s\n", droneInfo[i].name);

		printf("freq=%.1f,bandwidth=%.1f,amp=%.1f,range=%.1f\n", droneInfo[i].freq[0], droneInfo[i].bw, droneInfo[i].amp, droneInfo[i].range);
	}
}


/* 新算法封装 */
void sfDroneSnifferV0(struct droneResult *droneInfo, int *nDrone, unsigned short ***downMat, unsigned short ***upMat, float cenFreq, float fs, float bw, float gain, int calRows)
{
	int nRows = calRows; //确定本次计算用了何种大小的数据
	int nCols = NFFT / N;
	float dF = fs / NFFT * N;
	float dt = M * NFFT / fs / 1000.0f;

	// download部分
	struct blockGroup *uavPulse = (struct blockGroup *)malloc(MaxDrone * sizeof(struct blockGroup));
	int                nUavPulse = 0;
	// upload部分
	struct blockGroup *uavCtrPulse = (struct blockGroup *)malloc(MaxDrone * sizeof(struct blockGroup));
	int                nUavCtrPulse = 0;

	// download信号处理
	sfDownLoadSniffer(uavPulse, &nUavPulse, downMat, 0, nRows, nCols, cenFreq, gain, dt, dF, droneLIB, nLib);
	// 写入处理信息
	sfSetResult(droneInfo, nDrone, droneLIB, uavPulse, nUavPulse, uavCtrPulse, nUavCtrPulse);

	free(uavPulse);
	free(uavCtrPulse);
}
/* 新算法封装 */
void sfDroneSnifferV1(struct droneResult *droneInfo, int *nDrone, unsigned short ***downMat, unsigned short ***upMat, float cenFreq, float fs, float bw, float gain, int calRows)
{
	int nRows = calRows; //确定本次计算用了何种大小的数据
	int nCols = NFFT / N;
	float dF = fs / NFFT * N;
	float dt = M * NFFT / fs / 1000.0f;


	unsigned short(*pBuf)[NRow / M][NFFT / N] = NULL;
	pBuf = (unsigned short*)downMat;

	// download部分
	struct blockGroup *uavPulse = (struct blockGroup *)malloc(MaxDrone * sizeof(struct blockGroup));
	int                nUavPulse = 0;
	// upload部分
	struct blockGroup *uavCtrPulse = (struct blockGroup *)malloc(MaxDrone * sizeof(struct blockGroup));
	int                nUavCtrPulse = 0;

	// download信号处理
	// 20230816增加-属于数字图传检测频段才执行
	if ((cenFreq >= 842 && cenFreq <= 915) || (cenFreq >= 1437 && cenFreq <= 5240) || (cenFreq >= 5745 && cenFreq <= 5825))
	{
		sfDownLoadSniffer(uavPulse, &nUavPulse, pBuf, 0, nRows, nCols, cenFreq, gain, dt, dF, droneLIB, nLib);
	}
	// 模拟图传信号处理
	// 20230816增加-属于模拟图传检测频段才执行
	if ((cenFreq >= 1080 && cenFreq <= 1360) || (cenFreq >= 5655 && cenFreq <= 5930))
	{
		sfMoniSniffer(uavPulse, &nUavPulse, pBuf, 0, nRows, nCols, cenFreq, gain, dt, dF, moniF, moniNum, 5, 10);
	}
	// 写入处理信息
	sfSetResult(droneInfo, nDrone, droneLIB, uavPulse, nUavPulse, uavCtrPulse, nUavCtrPulse);

	free(uavPulse);
	free(uavCtrPulse);
}


//图传检测主函数-外部调用
void sfDownLoadSniffer(struct blockGroup *droneBlock, int *nBlock, unsigned short ***downMat, int chn, int nRows, int nCols, float cenFreq, float gain, float dT, float dF, struct DroneLib *UAVtypes, int nUav)
{
	int i = 0, j = 0, k = 0;

	// 计算全图门限
	static float DT = 0.1;
	float threshold1 = sfAutoThreshOnMapV1(downMat, chn, nCols, nRows, DT, 6);
	// 门限增加固定值保护-20230802修改
	if (cenFreq == 915)
	{
		sfThresholdCompare(&threshold1, cenFreq, 3, 3 + 6);
	}
	else if (cenFreq == 842)
	{
		sfThresholdCompare(&threshold1, cenFreq, 3, 3 + 6);
	}
	else
	{
		sfThresholdCompare(&threshold1, cenFreq, 3, 3 + 36);
	}

	// 计算频率维度过门限的点
	static int mPass1[sfM1];
	for (i = 0; i < sfM1; i++)
	{
		mPass1[i] = 0;
	}
	sfFrequenceHist(downMat, 0, mPass1, nCols, nRows, threshold1);
	// 边带保护-20230802更新
	if (cenFreq == 915)
	{
		for (i = 0; i < 29; i++)
		{
			mPass1[i] = 600;
		}
		for (i = 99; i < 128; i++)
		{
			mPass1[i] = 600;
		}
	}
	else if (cenFreq == 842)
	{
		for (i = 0; i < 39; i++)
		{
			mPass1[i] = 600;
		}
		for (i = 114; i < 128; i++)
		{
			mPass1[i] = 600;
		}
	}
	// 计算频率维点数门限
	float threshold2 = sfAutoThreshOnLineI(mPass1, nCols, DT, 6);
	if (threshold2 > 1500)
	{
		threshold2 = 1500;
	}
	// 获取可能的频率
	static int fStart[sfM1];
	static int fEnd[sfM1];
	int fnum = 0;
	fnum = sfCalculateFrequenceSE(mPass1, nCols, threshold2, 6, fStart, fEnd);
	// 循环计算脉冲
	static float mPass2[sfM2];
	static int q1[sfM2];
	static int w[sfM2];
	static float colMeanAmp[sfM2];
	static float colSnr[sfM2];

	if (fnum > 0)
	{
		for (i = 0; i < fnum; i++)
		{
			// 生成合并脉冲列
			for (j = 0; j < sfM2; j++)
			{
				mPass2[j] = 0;
				for (k = fStart[i]; k <= fEnd[i]; k++)
				{
					mPass2[j] = mPass2[j] + downMat[chn][j][k];
				}
				mPass2[j] = mPass2[j] / (fEnd[i] - fStart[i] + 1);
			}
			// 计算幅度门限
			float threshold3 = sfAutoThreshOnLineF(mPass2, nRows, DT, 6);
			// 门限增加固定值保护-20230802更新
			if (cenFreq == 915)
			{
				sfThresholdCompare(&threshold3, cenFreq, 3, 3 + 6);
			}
			else if (cenFreq == 842)
			{
				sfThresholdCompare(&threshold3, cenFreq, 3, 3 + 6);
			}
			else
			{
				sfThresholdCompare(&threshold3, cenFreq, 3, 3 + 36);
			}
			// 获取脉冲描述信息
			int	nw;
			int Delt = 1;
			int MinW = 0.5f / dT; //0.7f / dT; // 20230626,窄脉冲识别门限由0.7下降至0.5
			nw = sfThresholdToPulse(mPass2, nRows, threshold3, Delt, MinW, q1, w, colSnr, colMeanAmp);
			// 进行目标数据库匹配
			static struct BlockRow aBlkRow;
			int det = sfVidBlocksDetect(&aBlkRow, mPass2, nRows, q1, w, nw, colSnr, chn, fStart[i], fEnd[i], cenFreq, dT, dF, UAVtypes, nUav);
			if (det == 1)
			{
				// 最后进行检测幅度校验
				float threshold4 = 0;
				sfThresholdCompare(&threshold4, cenFreq, 3, 3 + 6);
				if (aBlkRow.meanColAmp < threshold4)
				{
					continue;
				}
				// 数据完成ablock至droneBlock的添加
				int tmpBlock = *nBlock;
				sfPulseToTarget(droneBlock, tmpBlock, aBlkRow, fStart[i], fEnd[i], cenFreq, dT, dF, UAVtypes);
				*nBlock = *nBlock + 1;
			}
			else
			{
				continue;
			}
		}
	}
}

//获取区分信号和噪声的阈值（二维）
float sfAutoThreshOnMapV0(unsigned short ***downMat, int chnum, int d128, int d3000, float dT, int maxIter)
{
	int   i = 0, j = 0, k = 0;
	int   n1 = 0;
	float t, tnew = 0.0f;
	float t1 = 0.0f, t2 = 0.0f;

	// 计算功率值总和-时间域左右退1，频率域左右退2
	for (i = 1; i < d3000-1; i++)
	{
		for (j = 2; j < d128-2; j++)
		{
			tnew += downMat[chnum][i][j];
		}
	}
	tnew = tnew / ((d3000-2)*(d128-4));
	t = tnew + 2 * dT;

	while (fabs(tnew - t) > dT && k < maxIter)
	{
		t = tnew;
		printf("MAP:%f \n", t);

		for (i = 1; i < d3000-1; i++)
		{
			for (j = 2; j < d128-2; j++)
			{
				if (downMat[chnum][i][j] > t)
				{
					t1 += downMat[chnum][i][j];
					n1++;
				}
				else
					t2 += downMat[chnum][i][j];
			}
		}

		if ((d3000 - 2)*(d128 - 4) > n1)
			tnew = (t1 / n1 + t2 / ((d3000 - 2)*(d128 - 4) - n1)) / 2.0f;
		else
			tnew = t;

		t1 = 0.0f;
		t2 = 0.0f;
		n1 = 0;
		k++;
	}

	return tnew;
}

//获取区分信号和噪声的阈值（二维）
float sfAutoThreshOnMapV1(unsigned short ***downMat, int chnum, int d128, int d3000, float dT, int maxIter)
{
	// 先做加和，再做自动门限
	int   i = 0, j = 0, k = 0;
	int   n1 = 0;
	float t, tnew = 0.0f;
	float t1 = 0.0f, t2 = 0.0f;

	float line[128];

	// 20230706-除数值域限制
	if (d3000 < 100 || d128 < 32)
	{
		return 0;
	}

	for (j = 0; j < d128; j++)
	{
		line[j] = 0;
		for (i = 0; i < d3000; i++)
		{
			line[j] += downMat[chnum][i][j];		
		}
		line[j] = line[j] / d3000;
	}

	// 计算功率值总和

	for (j = 2; j < d128-2; j++)
	{
		tnew += line[j] / (d128-4);
	}
	t = tnew + 2 * dT;


	while (fabs(tnew - t) > dT && k < maxIter)
	{
		t = tnew;

		for (j = 2; j < d128-2; j++)
		{
			if (line[j] > t)
			{
				t1 += line[j];
				n1++;
			}
			else
				t2 += line[j];
		}

		// 20230706-修正除零风险
		if (n1 == 0)
			tnew = t;
		else if (d128-4 > n1)
			tnew = t1 / n1 / 4 * 3 + t2 / (d128-4 - n1) / 4 * 1;// 门限要往大值倾斜
		else
			tnew = t;

		t1 = 0.0f;
		t2 = 0.0f;
		n1 = 0;
		k++;
	}
	return tnew;
}


// 获取过门限的采样统计信息
void sfFrequenceHist(unsigned short ***downMat, int chnum, int* hist, int d128, int d3000, float threshold)
{
	int i = 0, j = 0;

	for (i = 0; i < d3000; i++)
	{
		for (j = 0; j < d128; j++)
		{
			if (downMat[chnum][i][j] > threshold)
			{
				hist[j] = hist[j] + 1;
			}
		}
	}
	// 补最少点值-防止部分零结果将整个门限拉低
	for (i = 0; i < d128; i++)
	{
		if (hist[i] < (int)(d3000*0.2))
		{
			hist[i] = (int)(d3000*0.2);
		}
	}
	// 边带强制不检测赋值-20230612
	for (i = 0; i < 5; i++)
	{
		hist[i] = (int)(d3000*0.2);
		hist[d128 - 1 - i] = (int)(d3000*0.2);
	}

}

//获取区分信号和噪声的阈值（一维）
float sfAutoThreshOnLineI(int *line, int d128, float dT, int maxIter)
{
	int   i = 0, j = 0, k = 0;
	int   n1 = 0;
	float t, tnew = 0.0f;
	float t1 = 0.0f, t2 = 0.0f;


	// 20230706-除数值域限制
	if (d128 < 32)
	{
		return 0;
	}

	// 计算功率值总和

	for (j = 0; j < d128; j++)
	{
		tnew += line[j] / d128;
	}
	t = tnew + 2 * dT;

	while (fabs(tnew - t) > dT && k < maxIter)
	{
		t = tnew;

		for (j = 0; j < d128; j++)
		{
			if (line[j] > t)
			{
				t1 += line[j];
				n1++;
			}
			else
				t2 += line[j];
		}

		// 20230706-修正除零风险
		if (n1 == 0)
			tnew = t;
		if (d128 > n1)
			tnew = (t1 / n1 + t2 / (d128 - n1)) / 2.0f;
		else
			tnew = t;

		t1 = 0.0f;
		t2 = 0.0f;
		n1 = 0;
		k++;
	}
	return tnew;
}

float sfAutoThreshOnLineF(float *line, int d128, float dT, int maxIter)
{
	int   i = 0, j = 0, k = 0;
	int   n1 = 0;
	float t, tnew = 0.0f;
	float t1 = 0.0f, t2 = 0.0f;


	// 20230706-除数值域限制
	if (d128 < 32)
	{
		return 0;
	}

	// 计算功率值总和

	for (j = 0; j < d128; j++)
	{
		tnew += line[j] / d128;
	}
	t = tnew + 2 * dT;

	while (fabs(tnew - t) > dT && k < maxIter)
	{
		t = tnew;

		for (j = 0; j < d128; j++)
		{
			if (line[j] > t)
			{
				t1 += line[j];
				n1++;
			}
			else
				t2 += line[j];
		}

		// 20230706-修正除零风险
		if (n1 == 0)
			tnew = t;
		if (d128 > n1)
			tnew = t1 / n1 / 4 * 2 + t2 / (d128 - n1) / 4 * 2; // 改了门限分配比例
		else
			tnew = t;

		t1 = 0.0f;
		t2 = 0.0f;
		n1 = 0;
		k++;
	}
	return tnew;
}

// 计算需要进行脉冲计算的频率行起始信息
int sfCalculateFrequenceSE(int *line, int d128, float dT, int lineWidthT, int *lineS, int *lineE)
{
	int calM1[sfM1];
	int calM2[sfM1];
	int calM3[sfM1];
	int calM4[sfM1];
	int calCnt1 = 0;
	int calCnt2 = 0;
	int i = 0;

	for (i = 0; i < d128; i++)
	{
		calM1[i] = 0;
		calM2[i] = 0;
	}

	for (i = 0; i < d128; i++)
	{
		if (line[i] > dT)
		{
			calM1[i] = 1;
		}
	}
	// 数据结构上下为0，保证了在频率维度上检测一定有始有终
	calCnt1 = 0;
	for (i = 1; i < d128; i++)
	{
		if ((calM1[i] - calM1[i - 1]) == 1)
		{
			calM2[calCnt1] = i;
		}
		else if ((calM1[i] - calM1[i - 1]) == -1)
		{
			calM3[calCnt1] = i;
			calCnt1++;
		}
		else
		{

		}
	}
	// 短带宽检测-4M以下则不认为是有效的
	calCnt2 = 0;
	for (i = 0; i < calCnt1; i++)
	{
		if ((calM3[i] - calM2[i]) > 10)
		{
			lineS[calCnt2] = calM2[i];
			lineE[calCnt2] = calM3[i]-1;
			calCnt2++;
		}
	}
	return calCnt2;
}

// 分选脉冲流
int sfThresholdToPulse(float *s, int n, float thresh, int delt, int minw, int *q1, int *w, float *colSnr, float *colAm)
{
	int i, j;
	char  flag = 1;
	int   nFall = 0;
	int   nw = 0;
	float signalAmp = 0.0f, noiseAmp = 0.0f;

	q1[0] = -1;
	for (i = 0; i < n; i++)
	{
		if (s[i] > thresh)
		{
			if (!flag)
			{
				q1[nw] = i;
				flag = 1;
			}
			nFall = 0;
		}
		else
		{
			if (flag)
			{
				nFall++;
				if (nFall >= delt)
				{
					if (q1[0] > 0)
					{
						// dingyu, 20230202
						w[nw] = i - delt - q1[nw];
						if (w[nw] > minw)
						{
							nw++;
						}
					}
					flag = 0;
				}
			}
		}
	}

	if (nw > 0)
	{
		for (i = 0; i < nw; i++)
		{
			signalAmp = 0.0f;
			noiseAmp = 0.0f;
			for (j = q1[i]; j <= q1[i] + w[i]; j++)
			{
				signalAmp += s[j];
			}
			signalAmp /= w[i] + 1;
			if (i == 0)
			{
				for (j = q1[i] - 3; j < q1[i]; j++)
				{
					noiseAmp += s[j];
				}
				noiseAmp /= 3.0f;
			}
			else
			{
				for (j = q1[i - 1] + w[i - 1] + 1; j < q1[i]; j++)
				{
					noiseAmp += s[j];
				}
				noiseAmp /= q1[i] - q1[i - 1] - w[i - 1] - 1;
			}
			colSnr[i] = signalAmp - noiseAmp;
		}
	}
	return nw;
}

void sfThresholdCompare(float* thresh, float cenFreq, int tdB, int mdB)
{
	// 根据频点判定固定门限保护值
	for (int i = 0; i < listNum; i++)
	{
		if (listFrequence[i] == cenFreq)
		{
			if (*thresh < (listNoice[i] + tdB * 85))
			{
				*thresh = listNoice[i] + tdB * 85;
			}
			else if (*thresh > (listNoice[i] + mdB * 85))
			{
				*thresh = listNoice[i] + mdB * 85;
			}
			break;
		}
	}
}


//图传图块检测
int sfVidBlocksDetect(struct BlockRow *aBlkRow, float *s, int n, int *q1, int *w, int nw, float *colSnr, int antIndex, int colIndexS, int colIndexE, float cenFreq, float dT, float dF, struct DroneLib *UAVtypes, int nUAV)
{
	int   i, j, k;
	char  isUAV;
	char  isInFreqSpan;
	int   mPulseW[maxPulseWNumInLib];
	int * Indexq = (int *)malloc(n * sizeof(int));
	int   nIndex;
	float freqL, freqR;
	int   freqIndexL, freqIndexR;
	int   count = 0;

	int **match = (int**)malloc(MaxVidPulse * sizeof(int*));
	for (i = 0; i < MaxVidPulse; i++)
		match[i] = (int*)malloc(MaxNumVidT * sizeof(int));

	int   meetPulseTCount = 0;
	float minSnr = 0.0;

	float *t2 = (float *)malloc(nw * sizeof(float));
	for (i = 0; i < nw; i++)
		t2[i] = (q1[i] + w[i]) * dT;

	for (i = 0; i < nUAV; i++) 
	{
		if (UAVtypes[i].method != 1)
			continue;

		isUAV = 1;

		if (UAVtypes[i].isFixedFreq)
		{
			isInFreqSpan = 0;
			for (j = 0; j < UAVtypes[i].nfreq; j++)
			{
				freqL = UAVtypes[i].freqPoints[j] - UAVtypes[i].pulseBW[UAVtypes[i].nPulseBW - 1] / 2;
				freqR = UAVtypes[i].freqPoints[j] + UAVtypes[i].pulseBW[UAVtypes[i].nPulseBW - 1] / 2;
				freqIndexL = (freqL - cenFreq) / dF + NFFT / N / 2;
				freqIndexR = (freqR - cenFreq) / dF + NFFT / N / 2;
				if (colIndexS > freqIndexL && colIndexE <= freqIndexR)
				{
					isInFreqSpan = 1;
					break;
				}
			}
			if (!isInFreqSpan)
				continue;
		}
		else
		{
			freqL = UAVtypes[i].freqPoints[0];
			freqR = UAVtypes[i].freqPoints[1];
			freqIndexL = (freqL - cenFreq) / dF + NFFT / N / 2;
			freqIndexR = (freqR - cenFreq) / dF + NFFT / N / 2;

			/*
			if (colIndex <= freqIndexL || colIndex > freqIndexR)
				continue;
			*/
			//上下界有交叠则认为该目标可参与计算-此处后续可考虑做更精细的处理
			if (colIndexE <= freqIndexL || colIndexS > freqIndexR)
				continue;


		}

		for (j = 0; j < maxPulseWNumInLib; j++)
			mPulseW[j] = 0;
		nIndex = 0;

		for (j = 0; j < nw; j++)
		{
			for (k = 0; k < UAVtypes[i].nPulseW; k++)
			{
				if (fabs(UAVtypes[i].pulseW[k] - w[j] * dT) < UAVtypes[i].pulseWErr[k])
				{
					Indexq[nIndex] = j;
					nIndex++;
					mPulseW[k]++;
					break;
				}
			}
		}

		for (j = 0; j < UAVtypes[i].nPulseW; j++)
		{
			if (UAVtypes[i].meetPulseW[j] > 0 && mPulseW[j] < UAVtypes[i].meetPulseW[j])
			{
				isUAV = 0;
				break;
			}
			if (UAVtypes[i].meetPulseW[j] < 0 && mPulseW[j] >= -UAVtypes[i].meetPulseW[j])
			{
				isUAV = 0;
				break;
			}
		}

		if (isUAV && UAVtypes[i].nPulseT > 0 && UAVtypes[i].pulseT[0] > 0)
		{
			isUAV = 0;

			sfMatchPulseT(match, t2, Indexq, nIndex, UAVtypes[i].pulseT, UAVtypes[i].nPulseT, UAVtypes[i].pulseTErr);

			/*
			// 测试代码
			int csmatch[MaxVidPulse][MaxVidPulse];
			for (i = 0; i < MaxVidPulse; i++)
				for (j = 0; j < MaxNumVidT; j++)
					csmatch[i][j] = match[i][j];
			*/

			for (j = 0; j < nIndex; j++)
			{
				if (j > MaxNumVidT - 1)
					break;
				meetPulseTCount = 0;
				for (k = 0; k < MaxNumVidT; k++)
				{
					if (match[j][k] > 0)
					{
						meetPulseTCount++;
					}
				}

				
				if (meetPulseTCount > 0 && meetPulseTCount >= UAVtypes[i].hoppCnt)
				{

					//-----修改从此开始

					// 20230523误报处理：针对O3误报做一个最大脉冲数与满足分选的比较
					// 风险点为如果有干扰，可能就无法分选出来了
					if (i == 2)
					{
						// 求最大脉冲编号
						int maxpn = -1;
						for (int tmi = 0; tmi < 50; tmi++)
						{
							if (match[j][tmi] > maxpn)
							{
								maxpn = match[j][tmi];
							}
						}
						// 计算本参数平均脉冲个数
						float sumPulseT = 0.0f;
						for (int tmi = 0; tmi < UAVtypes[i].nPulseT; tmi++)
							sumPulseT += UAVtypes[i].pulseT[tmi];
						int tmpn = (120 / sumPulseT)*UAVtypes[i].nPulseT;
						if (tmpn<50 && maxpn>tmpn*1.75)
						{
							continue;// 脉冲过密则认为不判断
						}
					}

					// 20230624误报处理：针对EVOII误报做一个频率分选比较统计
					// 风险点为如果有干扰，可能就无法分选出来了
					if (i >= 4 && i <= 5)
					{
						// 求信号脉宽
						float tmpbw = (colIndexE - colIndexS)*0.4;
						if (tmpbw>16)
						{
							continue;// 信号太宽则认为不判断
						}
					}

					// 20230522误报处理：针对8-16 Autel Lt/V3/M4T 误报处理
					// P1：检测120ms是否存在一次及以上的7ms中断
					// P2：检测[1 1 1 2]序列是否存在一次及以上
					// P3：[1 1 1 2]序列中，2ms周期中间应不存在其他脉冲
					// P4：7ms中断不足一次，严格的[1 1 1 2]逻辑不足3个，则认为是虚警
					if (i >= 8 && i <= 13) // 索引号严格核对库信息
					{
						float timePRT[50];
						int timecnt = 0;
						int prtcnt = 0;
						int numPw[50]; // 记录所有满足分选的有效脉冲编号

						for (int tmi = 0; tmi < 50; tmi++)//获取有效到达时间信息队列
						{
							if (match[j][tmi] > 0)
							{
								timePRT[timecnt] = t2[match[j][tmi]];
								numPw[timecnt] = match[j][tmi];
								timecnt++;
							}
						}
						//计算周期与7ms的对应关系-6~11，同时记录重复周期的变化规律
						float timeJG[50];
						for (int tmi = 1; tmi < timecnt; tmi++)
						{
							timeJG[tmi - 1] = (timePRT[tmi] - timePRT[tmi - 1]);
							if ((timePRT[tmi] - timePRT[tmi - 1]) > 6 && (timePRT[tmi] - timePRT[tmi - 1]) < 11)
							{
								prtcnt++; // 正常应该有3次，要不要检测两次-50里
							}

						}
						// 计算重复周期满足[1 1 1 2] 规律，且比较2ms周期中间是否有脉冲
						int jgcnt1 = 0;
						int jgcnt2 = 0;
						for (int tmi = 0; tmi < timecnt - 1 - 3; tmi++)
						{
							jgcnt2 = 0;
							for (int tmj = 0; tmj < 4; tmj++)
							{
								if (tmj < 3 && (timeJG[tmi + tmj] - 1) < 0.2)
								{
									jgcnt2++;
								}
								else if (tmj == 3 && (timeJG[tmi + tmj] - 2) < 0.2)
								{
									jgcnt2++;
								}
							}
							if (jgcnt2 == 4 && (numPw[tmi + 4] - numPw[tmi + 3]) == 1) // 判第四个周期间有没有脉冲
							{
								jgcnt1++;
							}
						}
						if (prtcnt < 1 || jgcnt1 < 5)//如果7ms大周期不足1个（只有50个脉冲）或者满足的[1112]周期数量不足3个，则认为是虚警
						{
							continue;
						}
						else
						{

						}
					}

					// 20230522误报处理：针对17-19 非常规Autel V3/M4T 误报处理-后续可能会单独修改
					// P1：检测120ms是否存在一次及以上的7ms中断
					// P2：检测[1 1 1 2]序列是否存在一次及以上
					// P3：[1 1 1 2]序列中，2ms周期中间应不存在其他脉冲
					// P4：7ms中断不足一次，严格的[1 1 1 2]逻辑不足3个，则认为是虚警
					if (i >= 14 && i <= 16) // 索引号严格核对库信息
					{
						float timePRT[50];
						int timecnt = 0;
						int prtcnt = 0;
						int numPw[50]; // 记录所有满足分选的有效脉冲编号

						for (int tmi = 0; tmi < 50; tmi++)//获取有效到达时间信息队列
						{
							if (match[j][tmi] > 0)
							{
								timePRT[timecnt] = t2[match[j][tmi]];
								numPw[timecnt] = match[j][tmi];
								timecnt++;
							}
						}
						//计算周期与7ms的对应关系-6~11，同时记录重复周期的变化规律
						float timeJG[50];
						for (int tmi = 1; tmi < timecnt; tmi++)
						{
							timeJG[tmi - 1] = (timePRT[tmi] - timePRT[tmi - 1]);
							if ((timePRT[tmi] - timePRT[tmi - 1]) > 6 && (timePRT[tmi] - timePRT[tmi - 1]) < 10)
							{
								prtcnt++; // 正常应该有1-2次
							}

						}
						// 计算重复周期满足[1 1 1 2] 规律，且比较2ms周期中间是否有脉冲
						int jgcnt1 = 0;
						int jgcnt2 = 0;
						for (int tmi = 0; tmi < timecnt - 1 - 3; tmi++)
						{
							jgcnt2 = 0;
							for (int tmj = 0; tmj < 4; tmj++)
							{
								if (tmj < 3 && (timeJG[tmi + tmj] - 1) < 0.2)
								{
									jgcnt2++;
								}
								else if (tmj == 3 && (timeJG[tmi + tmj] - 2) < 0.2)
								{
									jgcnt2++;
								}
							}
							if (jgcnt2 == 4 && (numPw[tmi + 4] - numPw[tmi + 3]) == 1) // 判第四个周期间有没有脉冲
							{
								jgcnt1++;
							}
						}
						if (prtcnt < 1 || jgcnt1 < 5)//如果7ms大周期不足1个（只有50个脉冲）或者满足的[1112]周期数量不足3个，则认为是虚警
						{
							continue;
						}
						else
						{

						}
					}

					// 20230522误报处理：针对17-19 nano 误报处理
					// P1：检测120ms是否存在一次及以上的7ms中断
					// P2：检测[1 1 3]序列是否存在一次及以上
					// P3：[1 1 3]序列中，3ms周期中间应不存在其他脉冲
					// P4：7ms中断不足一次，严格的[1 1 3]逻辑不足3个，则认为是虚警
					if (i >= 17 && i <= 19) // 索引号严格核对库信息
					{
						float timePRT[50];
						int timecnt = 0;
						int prtcnt = 0;
						int numPw[50]; // 记录所有满足分选的有效脉冲编号

						for (int tmi = 0; tmi < 50; tmi++)//获取有效到达时间信息队列
						{
							if (match[j][tmi] > 0)
							{
								timePRT[timecnt] = t2[match[j][tmi]];
								numPw[timecnt] = match[j][tmi];
								timecnt++;
							}
						}
						//计算周期与7ms的对应关系-6~11，同时记录重复周期的变化规律
						float timeJG[50];
						for (int tmi = 1; tmi < timecnt; tmi++)
						{
							timeJG[tmi - 1] = (timePRT[tmi] - timePRT[tmi - 1]);
							if ((timePRT[tmi] - timePRT[tmi - 1]) > 6 && (timePRT[tmi] - timePRT[tmi - 1]) < 10)
							{
								prtcnt++; // 正常应该有1-2次
							}

						}
						// 计算重复周期满足[1 1 3] 规律，且比较3ms周期中间是否有脉冲
						int jgcnt1 = 0;
						int jgcnt2 = 0;
						for (int tmi = 0; tmi < timecnt - 1 - 2; tmi++)
						{
							jgcnt2 = 0;
							for (int tmj = 0; tmj < 3; tmj++)
							{
								if (tmj < 2 && (timeJG[tmi + tmj] - 1) < 0.2)
								{
									jgcnt2++;
								}
								else if (tmj == 2 && (timeJG[tmi + tmj] - 3) < 0.2)
								{
									jgcnt2++;
								}
							}
							if (jgcnt2 == 3 && (numPw[tmi + 3] - numPw[tmi + 2]) == 1) // 判第三个周期间有没有脉冲
							{
								jgcnt1++;
							}
						}
						if (prtcnt < 1 || jgcnt1 < 5)//如果7ms大周期不足1个（只有50个脉冲）或者满足的[113]周期数量不足3个，则认为是虚警
						{
							continue;
						}
						else
						{

						}
					}

					//-----修改到此结束

					isUAV = 1;
					break;
				}
				
			}

			if (isUAV)
			{
				nIndex = 0;
				for (k = 0; k < MaxNumVidT; k++)
				{
					if (match[j][k] >= 0)
					{
						Indexq[nIndex] = match[j][k];
						nIndex++;
					}
				}
			}
		}

		if (isUAV)
		{
			minSnr = colSnr[Indexq[0]];
			for (j = 0; j < nIndex; j++)
			{
				if (colSnr[Indexq[j]] < minSnr)
					minSnr = colSnr[Indexq[j]];
				if (colSnr[Indexq[j]] < UAVtypesSNR)
				{
					isUAV = 0;
					break;
				}
			}
		}

		if (isUAV)
		{
			(*aBlkRow).id = 0;
			(*aBlkRow).uavIndex = i;
			(*aBlkRow).antIndex = antIndex;
			(*aBlkRow).colIndex = (int)((colIndexE - colIndexS)/2);
			count = 0;
			for (j = colIndexS; j <= colIndexE; j++)
			{
				(*aBlkRow).allColIndex[count] = j;
				count++;
			}
			(*aBlkRow).snr = minSnr;
			(*aBlkRow).nCol = colIndexE- colIndexS+1;
			(*aBlkRow).meanColAmp = 0.0;
			count = 0;
			for (j = 0; j < nIndex; j++)
			{
				(*aBlkRow).q1[j] = q1[Indexq[j]];
				(*aBlkRow).w[j] = w[Indexq[j]];
				for (k = (*aBlkRow).q1[j]; k < (*aBlkRow).q1[j] + (*aBlkRow).w[j]; k++)
				{
					(*aBlkRow).meanColAmp += s[k];
				}
				count += (*aBlkRow).w[j];
			}
			(*aBlkRow).meanColAmp /= (float)count;
			(*aBlkRow).nw = nIndex;
			free(Indexq);
			free(t2);

			for (i = 0; i < MaxVidPulse; i++)
				free(match[i]);
			free(match);

			return 1;
		}
	}
	free(Indexq);
	free(t2);

	for (i = 0; i < MaxVidPulse; i++)
		free(match[i]);
	free(match);

	return 0;
}

//周期匹配
void sfMatchPulseT(int **match, float *t2, int *index, int n, float *pulseT, int nPulseT, float pulseTErr)
{
	float diffTime, resTime, TErr;
	float sumPulseT = 0.0f;
	int   nt = 0, nT = 0, nBigT = 0;
	int   count = 0, maxCount = 0, maxIndex = 0;
	int i, j, k, p, m;
	int   oneMatch[5][MaxNumVidT];
	float oneMatchTimeErr[5][MaxNumVidT];

	for (i = 0; i < nPulseT; i++)
		sumPulseT += pulseT[i];

	for (i = 0; i < n; i++)
	{
		if (i > MaxNumVidT - 1)
			break;
		for (k = 0; k < nPulseT; k++)
		{
			oneMatch[k][0] = index[i];
			for (m = 1; m < MaxNumVidT; m++)
				oneMatch[k][m] = -1;

			for (j = 0; j < n; j++)
			{
				if (j > MaxNumVidT - 1)
					break;
				if (i == j)
					continue;

				diffTime = t2[index[j]] - t2[index[i]];
				if (diffTime < 0)
					continue;

				nBigT = diffTime / sumPulseT;
				resTime = diffTime - nBigT * sumPulseT;

				nt = 0;
				while (1)
				{
					TErr = fabs(resTime);
					if (TErr < pulseTErr)
					{
						nT = nPulseT * nBigT + nt;
						if (nT > 0 && nT < MaxNumVidT)
						{
							if (oneMatch[k][nT] < 0)
							{
								oneMatch[k][nT] = index[j];
								oneMatchTimeErr[k][nT] = TErr;
							}
							else if (TErr < oneMatchTimeErr[k][nT])
							{
								oneMatch[k][nT] = index[j];
								oneMatchTimeErr[k][nT] = TErr;
							}
						}
						break;
					}
					if (resTime < 0)
						break;
					resTime -= pulseT[(k + nt) % nPulseT];
					nt++;
				}
			}
		}

		maxCount = 0;
		maxIndex = 0;
		for (k = 0; k < nPulseT; k++)
		{
			count = 0;
			for (p = 0; p < MaxNumVidT; p++)
			{
				if (oneMatch[k][p] > 0)
				{
					count++;
				}
			}
			if (count > maxCount)
			{
				maxCount = count;
				maxIndex = k;
			}
		}

		for (k = 0; k < MaxNumVidT; k++)
		{
			match[i][k] = oneMatch[maxIndex][k];
		}
	}
}

// 脉冲参数转目标参数
void sfPulseToTarget(struct blockGroup *droneBlock, int nBlock, struct BlockRow aBlkRow, int fstart, int fend, float cenFreq, float dT, float dF, struct DroneLib *UAVtypes)
{
	int i = 0, j = 0, k = 0;
	if (nBlock >= MaxDrone)
		return;

	droneBlock[nBlock].id = nBlock;
	droneBlock[nBlock].index = aBlkRow.uavIndex;
	droneBlock[nBlock].range = 0;
	droneBlock[nBlock].bw = (fend - fstart)*dF;

	float freq[MaxPulseInGroup];
	float bw;

	droneBlock[nBlock].n = aBlkRow.nw;
	for (j = 0; j < NCh; j++)
	{
		droneBlock[nBlock].inCh[j] = aBlkRow.inCh[j];
	}
	for (j = 0; j < aBlkRow.nw; j++)
	{
		droneBlock[nBlock].freq[j] = cenFreq + ((float)(fend + fstart) / 2 - (NFFT / N / 2))*dF;
		droneBlock[nBlock].burstTime[j] = aBlkRow.q1[j] * dT;
		droneBlock[nBlock].width[j] = aBlkRow.w[j] * dT;
		droneBlock[nBlock].dist[j] = 0;
		for (k = 0; k < NCh; k++)
		{
			droneBlock[nBlock].amp[k][j] = aBlkRow.meanColAmp;
		}
	}
	nBlock++;
}

//对外接口
void sfSetResult(struct droneResult *droneInfo, int *nDrone, struct DroneLib *droneLIB, struct blockGroup *uavPulse, int nUavPulse, struct blockGroup *uavCtrPulse, int nUavCtrPulse)
{
	*nDrone = nUavPulse + nUavCtrPulse;
	float maxAmp = 0.0f;
	for (int i = 0; i < nUavPulse; i++)
	{
		droneInfo[i].ID = uavPulse[i].id;
		strcpy(droneInfo[i].name, droneLIB[uavPulse[i].index].name);
		for (int j = 0; j < droneLIB[uavPulse[i].index].nPsbID; j++)
		{
			droneInfo[i].psbID[j] = droneLIB[uavPulse[i].index].psbID[j];
		}
		droneInfo[i].nPsbID = droneLIB[uavPulse[i].index].nPsbID;
		droneInfo[i].flag = uavPulse[i].index;
		droneInfo[i].amp = 0.0f;
		for (int j = 0; j < uavPulse[i].n; j++)
		{
			droneInfo[i].freq[j] = uavPulse[i].freq[j];
			droneInfo[i].burstTime[j] = uavPulse[i].burstTime[j];
			droneInfo[i].width[j] = uavPulse[i].width[j];
			maxAmp = uavPulse[i].amp[0][j];
			for (int k = 0; k < NCh; k++)
			{
				if (uavPulse[i].amp[k][j] > maxAmp)
					maxAmp = uavPulse[i].amp[k][j];
			}
			droneInfo[i].amp += maxAmp;
		}
		droneInfo[i].bw = uavPulse[i].bw;
		droneInfo[i].amp = droneInfo[i].amp / (float)uavPulse[i].n;
		droneInfo[i].range = uavPulse[i].range;
	}

	for (int i = 0; i < nUavCtrPulse; i++)
	{
		droneInfo[nUavPulse + i].ID = uavCtrPulse[i].id;
		strcpy(droneInfo[nUavPulse + i].name, droneLIB[uavCtrPulse[i].index].name);
		for (int j = 0; j < droneLIB[uavCtrPulse[i].index].nPsbID; j++)
		{
			droneInfo[nUavPulse + i].psbID[j] = droneLIB[uavCtrPulse[i].index].psbID[j];
		}
		droneInfo[nUavPulse + i].nPsbID = droneLIB[uavCtrPulse[i].index].nPsbID;
		droneInfo[nUavPulse + i].flag = 1;
		droneInfo[nUavPulse + i].amp = 0.0f;
		for (int j = 0; j <= uavCtrPulse[i].n; j++)
		{
			droneInfo[nUavPulse + i].freq[j] = uavCtrPulse[i].freq[j];
			droneInfo[nUavPulse + i].burstTime[j] = uavCtrPulse[i].burstTime[j];
			droneInfo[nUavPulse + i].width[j] = uavCtrPulse[i].width[j];
			maxAmp = uavCtrPulse[i].amp[0][j];
			for (int k = 0; k < NCh; k++)
			{
				if (uavCtrPulse[i].amp[k][j] > maxAmp)
					maxAmp = uavCtrPulse[i].amp[k][j];
			}
			droneInfo[nUavPulse + i].amp += maxAmp;
		}
		droneInfo[nUavPulse + i].bw = uavCtrPulse[i].bw;
		droneInfo[nUavPulse + i].amp = droneInfo[nUavPulse + i].amp / (float)uavCtrPulse[i].n;
		droneInfo[nUavPulse + i].range = uavCtrPulse[i].range;
	}
}




/* 定向阶段幅度计算*/

// 计算幅度-外部调用
float sfDxCalCulateAm(unsigned short ***AmPower, int StartLine, int EndLine, int numUav, int calRows)
{
	float tmp = sfDxDronePowerV1(AmPower, StartLine, EndLine, droneLIB, numUav, calRows);
	return tmp;
}
// 计算幅度-逐行计算法
float sfDxDronePowerV0(unsigned short ***AmPower, int StartLine, int EndLine, struct DroneLib *UAVtypes, int numUav, int calRows)
{
	float GlobalAm[NFFT / N];
	int GlobalNm[NFFT / N];
	int GlobalCnt = 0;
	float FinalAm = 0;
	int FinalCnt = 0;
	float FinalMeanAm = 0;

	// 数据初始化
	for (int i = 0; i < NFFT / N; i++)
	{
		GlobalAm[i] = 0;
		GlobalNm[i] = 0;
	}

	// 逐行计算
	for (int i = StartLine; i < EndLine; i++)
	{
		// 求解单行自动门限-求取均值
		int nRows = calRows;
		float colVecFilted[NRow / M];
		int q1[NRow / M];
		int w[NRow / M];
		float colMeanAmp[NRow / M];
		float colSnr[NRow / M];
		int  Indexq[NRow / M];

		for (int j = 0; j < NRow / M; j++)
		{
			colVecFilted[j] = 0;
			q1[j] = 0;
			w[j] = 0;
			colMeanAmp[j] = 0;
			Indexq[j] = 0;
		}
		for (int j = 0; j < nRows; j++)
		{
			colVecFilted[j] = AmPower[0][j][i];
		}
		float thresh = 0;
		static float DT = 0.1;
		static int   MaxIter = 6;

		thresh = sfAutoThreshOnLineF(colVecFilted, nRows, DT, MaxIter);

		int nw = 0;
		static int   Delt = 1;
		float fs = 51.2f;
		float dF = fs / NFFT * N;
		float dT = M * NFFT / fs / 1000.0f;
		int   MinW = 0.7f / dT;
		int   q1IndexErr = 0.5f / dT;


		// 分离脉冲及对应的平均功率
		nw = sfDxThresholdWithAm(colVecFilted, nRows, thresh, Delt, MinW, q1, w, colMeanAmp);
		// 判定脉冲脉宽是否符合要求
		int   mPulseW[maxPulseWNumInLib];
		for (int j = 0; j < maxPulseWNumInLib; j++)
			mPulseW[j] = 0;

		int   nIndex = 0;
		for (int j = 0; j < nw; j++)
		{
			for (int k = 0; k < UAVtypes[numUav].nPulseW; k++)
			{
				if (fabs(UAVtypes[numUav].pulseW[k] - w[j] * dT) < UAVtypes[numUav].pulseWErr[k])
				{
					Indexq[nIndex] = j;
					nIndex++;
					mPulseW[k]++;
					break;
				}
			}
		}
		// 计算输出的功率
		if (nIndex > 0)
		{
			for (int j = 0; j < nIndex; j++)
			{
				GlobalAm[GlobalCnt] = GlobalAm[GlobalCnt] + colMeanAmp[Indexq[j]];
				GlobalNm[GlobalCnt] ++;
			}
			GlobalCnt++;
		}
	}
	// 计算最终平均功率
	for (int i = 0; i < GlobalCnt; i++)
	{
		FinalAm += GlobalAm[i];
		FinalCnt += GlobalNm[i];
	}
	if (FinalCnt > 0)
	{
		FinalMeanAm = FinalAm / FinalCnt;
		return FinalMeanAm;
	}
	else
	{
		return 0;
	}
}
// 计算幅度-多行平均法
float sfDxDronePowerV1(unsigned short ***AmPower, int StartLine, int EndLine, struct DroneLib *UAVtypes, int numUav, int calRows)
{
	float GlobalAm = 0;
	int GlobalNm = 0;

	float FinalAm = 0;

	// 求解多行自动门限-求取均值
	int nRows = calRows;
	static float colVecFilted[NRow / M];
	static int q1[NRow / M];
	static int w[NRow / M];
	static float colMeanAmp[NRow / M];
	static float colSnr[NRow / M];
	static int  Indexq[NRow / M];

	for (int j = 0; j < NRow / M; j++)
	{
		colVecFilted[j] = 0;
		q1[j] = 0;
		w[j] = 0;
		colMeanAmp[j] = 0;
		Indexq[j] = 0;
	}

	for (int j = 0; j < nRows; j++)
	{
		// 逐行计算
		for (int k = StartLine; k < EndLine; k++)
		{
			colVecFilted[j] += AmPower[0][j][k];
		}
		colVecFilted[j] = colVecFilted[j] / (EndLine - StartLine);
	}

	float thresh = 0;
	static float DT = 0.1;
	static int   MaxIter = 6;

	thresh = sfAutoThreshOnLineF(colVecFilted, nRows, DT, MaxIter);

	int nw = 0;
	static int   Delt = 1;
	float fs = 51.2f;
	float dF = fs / NFFT * N;
	float dT = M * NFFT / fs / 1000.0f;
	int   MinW = 0.7f / dT;
	int   q1IndexErr = 0.5f / dT;


	// 分离脉冲及对应的平均功率
	nw = sfDxThresholdWithAm(colVecFilted, nRows, thresh, Delt, MinW, q1, w, colMeanAmp);
	// 判定脉冲脉宽是否符合要求
	int   mPulseW[maxPulseWNumInLib];
	for (int j = 0; j < maxPulseWNumInLib; j++)
		mPulseW[j] = 0;

	int   nIndex = 0;
	for (int j = 0; j < nw; j++)
	{
		for (int k = 0; k < UAVtypes[numUav].nPulseW; k++)
		{
			if (fabs(UAVtypes[numUav].pulseW[k] - w[j] * dT) < UAVtypes[numUav].pulseWErr[k])
			{
				Indexq[nIndex] = j;
				nIndex++;
				mPulseW[k]++;
				break;
			}
		}
	}
	// 脉冲个数判定-求和半数通过原则
	int isUAV = 1;
	int sumNeed = 0;
	int sumGet = 0;
	for (int j = 0; j < UAVtypes[numUav].nPulseW; j++)
	{
		sumNeed = sumNeed + UAVtypes[numUav].meetPulseW[j];
		sumGet = sumGet + mPulseW[j];
	}
	if (sumGet < (sumNeed / 2))
	{
		isUAV = 0;
	}

	/*
	for (int j = 0; j < UAVtypes[numUav].nPulseW; j++)
	{
		if (UAVtypes[numUav].meetPulseW[j] > 0 && mPulseW[j] < (UAVtypes[numUav].meetPulseW[j] / 2))
		{
			isUAV = 0;
			break;
		}
		if (UAVtypes[numUav].meetPulseW[j] < 0 && mPulseW[j] >= -(UAVtypes[numUav].meetPulseW[j] / 2))
		{
			isUAV = 0;
			break;
		}
	}
	*/

	// 计算幅度
	if (isUAV == 1)
	{
		for (int j = 0; j < nIndex; j++)
		{
			GlobalAm = GlobalAm + colMeanAmp[Indexq[j]];
			GlobalNm++;
		}
		FinalAm = GlobalAm / GlobalNm;
		return FinalAm;
	}
	else
	{
		return 0;
	}
}

// 分离出脉冲及幅度
int sfDxThresholdWithAm(float *s, int n, float thresh, int delt, int minw, int *q1, int *w, float *colAm)
{
	int i, j;
	char  flag = 1;
	int   nFall = 0;
	int   nw = 0;
	float signalAmp = 0.0f, noiseAmp = 0.0f;

	q1[0] = -1;
	for (i = 0; i < n; i++)
	{
		if (s[i] > thresh)
		{
			if (!flag)
			{
				q1[nw] = i;
				flag = 1;
			}
			nFall = 0;
		}
		else
		{
			if (flag)
			{
				nFall++;
				if (nFall >= delt)
				{
					if (q1[0] > 0)
					{
						// dingyu, 20230202
						w[nw] = i - delt - q1[nw];
						if (w[nw] > minw)
						{
							nw++;
						}
					}
					flag = 0;
				}
			}
		}
	}

	if (nw > 0)
	{
		for (i = 0; i < nw; i++)
		{
			signalAmp = 0.0f;
			for (j = q1[i]; j <= q1[i] + w[i]; j++)
			{
				signalAmp += s[j];
			}
			signalAmp /= w[i] + 1;
			colAm[i] = signalAmp;
		}
	}

	return nw;
}


/* 新定向系统函数开发 */

// 主进程函数
int sfDxNewProcessing(unsigned short ***AmPowerQ, unsigned short ***AmPowerD, int StartLine, int EndLine, int numUav, int calRows, float *listAngle, float *listAmQ, float *listAmD, float *listAmC, int *listFP, int numDx)
{
	/* 
	输入描述
	AmPowerQ：全向天线输入时频图
	AmPowerD：定向天线输入时频图
	StartLine：信号起始行号
	EndLine：信号结束行号
	numUav：匹配目标编号
	calRows：计算列数，默认3000（时间列）
	listAngle：每次定向时机对应方位角度索引(实际没用上)
	numDx：定向计数
	输出
	listAmQ：每次定向时机对应全向天线幅度索引
	listAmD：每次定向时机对应定向天线幅度索引
	listAmC：每次定向时机对应定向天线幅度索引
	listFP：每次定向时机对应计算的匹配情况索引
	*/

	// 计算全向天线分选模板
	static int pulseSQ1[sfM2];
	static int pulseWQ1[sfM2];
	int pulseNQ1 = 0;
	static int pulseSQ2[sfM2];
	static int pulseWQ2[sfM2];
	int pulseNQ2 = 0;
	int pulseFQ = 0;
	pulseFQ = sfDxNewGetIndex(AmPowerQ, StartLine, EndLine, droneLIB, numUav, calRows, pulseSQ1, pulseWQ1, &pulseNQ1, pulseSQ2, pulseWQ2, &pulseNQ2);
	// 计算定向天线分选模板
	static int pulseSD1[sfM2];
	static int pulseWD1[sfM2];
	int pulseND1 = 0;
	static int pulseSD2[sfM2];
	static int pulseWD2[sfM2];
	int pulseND2 = 0;
	int pulseFD = 0;
	pulseFD = sfDxNewGetIndex(AmPowerD, StartLine, EndLine, droneLIB, numUav, calRows, pulseSD1, pulseWD1, &pulseND1, pulseSD2, pulseWD2, &pulseND2);

	// 分选失败则赋0值，返回0值
	float tmpAm1 = 0;
	float tmpAm2 = 0;
	if (pulseFQ == 0 && pulseFD == 0)
	{
		listAmQ[numDx] = 0;
		listAmD[numDx] = 0;
		listAmC[numDx] = 0;
		listFP[numDx] = -1;
		return 0;
	}
	else if (pulseFQ == 2)
	{
		// 计算全向定向天线幅度
		tmpAm1 = sfDxNewCalculateAm(AmPowerQ, StartLine, EndLine, calRows, pulseSQ2, pulseWQ2, pulseNQ2);
		tmpAm2 = sfDxNewCalculateAm(AmPowerD, StartLine, EndLine, calRows, pulseSQ2, pulseWQ2, pulseNQ2);
		listAmC[numDx] = 1;
	}
	else if (pulseFD == 2)
	{
		// 计算全向定向天线幅度
		tmpAm1 = sfDxNewCalculateAm(AmPowerQ, StartLine, EndLine, calRows, pulseSD2, pulseWD2, pulseND2);
		tmpAm2 = sfDxNewCalculateAm(AmPowerD, StartLine, EndLine, calRows, pulseSD2, pulseWD2, pulseND2);
		listAmC[numDx] = 2;
	}
	else if (pulseFQ == 1)
	{
		// 计算全向定向天线幅度
		tmpAm1 = sfDxNewCalculateAm(AmPowerQ, StartLine, EndLine, calRows, pulseSQ1, pulseWQ1, pulseNQ1);
		tmpAm2 = sfDxNewCalculateAm(AmPowerD, StartLine, EndLine, calRows, pulseSQ1, pulseWQ1, pulseNQ1);
		listAmC[numDx] = 3;
	}
	else if (pulseFD == 1)
	{
		// 计算全向定向天线幅度
		tmpAm1 = sfDxNewCalculateAm(AmPowerQ, StartLine, EndLine, calRows, pulseSD1, pulseWD1, pulseND1);
		tmpAm2 = sfDxNewCalculateAm(AmPowerD, StartLine, EndLine, calRows, pulseSD1, pulseWD1, pulseND1);
		listAmC[numDx] = 4;
	}
	else
	{
		listAmQ[numDx] = 0;
		listAmD[numDx] = 0;
		listAmC[numDx] = 5;
		listFP[numDx] = -1;
		return 0;
	}
	// 更新定向阶段信息数据流
	listAmQ[numDx] = tmpAm1;
	listAmD[numDx] = tmpAm2;
	float tmpAmC = 0;
	if (listAmQ[numDx] > 10000 && listAmD[numDx] > 10000)
	{
		tmpAmC = tmpAm2 - tmpAm1 - corQD;// 要做匹配修正
	}
	else
	{
		tmpAmC = 0;
	}
	// 根据信息流队列计算目标匹配结果
	if (tmpAmC > 750)
	{
		listFP[numDx] = 2;
	}
	else if (tmpAmC > 550)
	{
		listFP[numDx] = 1;
	}
	else
	{
		listFP[numDx] = 0;
	}

	return 0;
}



// 计算全向幅度匹配模板
int sfDxNewGetIndex(unsigned short ***AmPower, int StartLine, int EndLine, struct DroneLib *UAVtypes, int numUav, int calRows, int *ps1, int *pw1, int *pn1, int *ps2, int *pw2, int *pn2)
{
	// 求解多行自动门限-求取均值
	int nRows = calRows;
	float colVecFilted[NRow / M];
	int q1[NRow / M];
	int w[NRow / M];
	float colMeanAmp[NRow / M];
	float colSnr[NRow / M];
	int  Indexq[NRow / M];
	int status = 0;

	for (int j = 0; j < NRow / M; j++)
	{
		colVecFilted[j] = 0;
		q1[j] = 0;
		w[j] = 0;
		colMeanAmp[j] = 0;
		Indexq[j] = 0;
	}

	for (int j = 0; j < nRows; j++)
	{
		// 逐行计算
		for (int k = StartLine; k < EndLine; k++)
		{
			colVecFilted[j] += AmPower[0][j][k];
		}
		colVecFilted[j] = colVecFilted[j] / (EndLine - StartLine);
	}

	float thresh = 0;
	static float DT = 0.1;
	static int   MaxIter = 6;

	thresh = sfAutoThreshOnLineF(colVecFilted, nRows, DT, MaxIter);

	int nw = 0;
	static int   Delt = 1;
	float fs = 51.2f;
	float dF = fs / NFFT * N;
	float dT = M * NFFT / fs / 1000.0f;
	int   MinW = 0.7f / dT;
	int   q1IndexErr = 0.5f / dT;


	// 分离脉冲及对应的平均功率
	nw = sfDxThresholdWithAm(colVecFilted, nRows, thresh, Delt, MinW, q1, w, colMeanAmp);
	// 判定脉冲脉宽是否符合要求
	int   mPulseW[maxPulseWNumInLib];
	for (int j = 0; j < maxPulseWNumInLib; j++)
		mPulseW[j] = 0;

	int   nIndex = 0;
	for (int j = 0; j < nw; j++)
	{
		for (int k = 0; k < UAVtypes[numUav].nPulseW; k++)
		{
			if (fabs(UAVtypes[numUav].pulseW[k] - w[j] * dT) < UAVtypes[numUav].pulseWErr[k])
			{
				Indexq[nIndex] = j;
				nIndex++;
				mPulseW[k]++;
				break;
			}
		}
	}
	// 脉冲个数判定-匹配通过原则
	int isUAV = 1;

	for (int j = 0; j < UAVtypes[numUav].nPulseW; j++)
	{
		if (UAVtypes[numUav].meetPulseW[j] > 0 && mPulseW[j] < (UAVtypes[numUav].meetPulseW[j]))
		{
			isUAV = 0;
			break;
		}
		if (UAVtypes[numUav].meetPulseW[j] < 0 && mPulseW[j] >= -(UAVtypes[numUav].meetPulseW[j]))
		{
			isUAV = 0;
			break;
		}
	}

	// 如果不启用周期分选，只用脉宽分选 20230629
	if (isUAV)
	{
		for (int j = 0; j < nIndex; j++)
		{
			ps1[j] = q1[Indexq[j]];
			pw1[j] = w[Indexq[j]];
		}
		*pn1 = nIndex;
		status = 1;
	}
	else
	{
		*pn1 = 0;
		return status;
	}

	// 周期满足判定
	float *t2 = (float *)malloc(nw * sizeof(float));
	for (int j = 0; j < nw; j++)
		t2[j] = (q1[j] + w[j]) * dT;

	int **match = (int**)malloc(MaxVidPulse * sizeof(int*));
	for (int j = 0; j < MaxVidPulse; j++)
		match[j] = (int*)malloc(MaxNumVidT * sizeof(int));

	int meetPulseTCount = 0;
	int i, j, k = 0;


	if (isUAV && UAVtypes[numUav].nPulseT > 0 && UAVtypes[numUav].pulseT[0] > 0)
	{
		isUAV = 0;
		sfMatchPulseT(match, t2, Indexq, nIndex, UAVtypes[numUav].pulseT, UAVtypes[numUav].nPulseT, UAVtypes[numUav].pulseTErr);
		/*
		// 测试代码
		int csmatch[MaxVidPulse][MaxVidPulse];
		for (i = 0; i < MaxVidPulse; i++)
			for (j = 0; j < MaxNumVidT; j++)
				csmatch[i][j] = match[i][j];
		*/
		for (j = 0; j < nIndex; j++)
		{
			if (j > MaxNumVidT - 1)
				break;
			meetPulseTCount = 0;
			for (k = 0; k < MaxNumVidT; k++)
			{
				if (match[j][k] > 0)
				{
					meetPulseTCount++;
				}
			}
			if (meetPulseTCount > 0 && meetPulseTCount >= UAVtypes[numUav].hoppCnt)
			{
				isUAV = 1; 
				break;
			}
		}
		if (isUAV)
		{
			nIndex = 0;
			for (k = 0; k < MaxNumVidT; k++)
			{
				if (match[j][k] >= 0)
				{
					Indexq[nIndex] = match[j][k];
					nIndex++;
				}
			}
		}
	}

	// 模板转换输出
	if (isUAV)
	{
		for (j = 0; j < nIndex; j++)
		{
			ps2[j] = q1[Indexq[j]];
			pw2[j] = w[Indexq[j]];
		}
		*pn2 = nIndex;
		status = 2;

		free(t2);
		for (i = 0; i < MaxVidPulse; i++)
			free(match[i]);
		free(match);

		return status;
	}
	else
	{
		free(t2);
		for (i = 0; i < MaxVidPulse; i++)
			free(match[i]);
		free(match);

		return status;
	}

}


// 计算模板下匹配幅度
float sfDxNewCalculateAm(unsigned short ***AmPower, int StartLine, int EndLine, int calRows, int *ps, int *pw, int pnum)
{
	// 行均值获取
	int nRows = calRows;
	float colVecFilted[NRow / M];

	for (int j = 0; j < NRow / M; j++)
	{
		colVecFilted[j] = 0;
	}

	for (int j = 0; j < nRows; j++)
	{
		// 逐行计算
		for (int k = StartLine; k < EndLine; k++)
		{
			colVecFilted[j] += AmPower[0][j][k];
		}
		colVecFilted[j] = colVecFilted[j] / (EndLine - StartLine);
	}

	// 根据点号计算求和
	float finalAm = 0;
	float sumAm = 0;
	int sumP = 0;
	for (int j = 0; j < pnum; j++)
	{
		for (int k = 0; k < pw[j]; k++)
		{
			sumAm = sumAm + colVecFilted[ps[j] + k];
			sumP++;
		}
	}
	finalAm = sumAm / sumP;
	return finalAm;
}

int sfDxStatusTransform(float *listAngleAz, float *listAngleEl, float *listAmQ, float *listAmD, float *listAmC, int *listFP, int numDx, int *statusN, float *statusFW1, float *statusFW2, float *statusFW3)
{
	/*
	输入描述
	listAngleAz：每次定向时机对应方位角度索引
	listAngleEl：每次定向时机对应俯仰角度索引
	listAmQ：每次定向时机对应全向天线幅度索引
	listAmD：每次定向时机对应定向天线幅度索引
	listAmC：每次定向时机对应定向天线幅度索引
	listFP：每次定向时机对应计算的匹配情况索引
	输出
	statusN：当前工作状态阶段,0\5\6\8\9
	statusFW1：方位瞄准范围下界
	statusFW2：方位瞄准范围下界
	statusFW3：方位瞄准值
	*/

	// 默认进入5状态-方位扫描
	if (*statusN == 0)
	{
		*statusN = 5;
	}
	// 扫描阶段若报出识别，则转入方位瞄准-给出瞄准范围
	if (*statusN == 5)
	{
		if (listFP[numDx] >= 1)
		{
			*statusN = 6;
			// 查找10帧内角度与当前角度的方向性关系，确定方位瞄准范围
			int ti = numDx - 10;
			float ta = 0;
			if (ti < 0)
			{
				ti = 0;
			}
			for (int i = ti; i <= numDx; i++)
			{
				ta =  listAngleAz[numDx]- listAngleAz[i];
				// 转换值域范围
				if (ta > 180)
				{
					ta = ta - 360;
				}
				else if (ta < -180)
				{
					ta = ta + 360;
				}
				// 值域有效性判断
				if (ta > 5)
				{
					*statusFW1 = listAngleAz[numDx];
					*statusFW2 = listAngleAz[numDx] + 40;
					break;
				}
				else if (ta < -5)
				{
					*statusFW1 = listAngleAz[numDx];
					*statusFW2 = listAngleAz[numDx] - 40;
					break;
				}
				if (i == numDx)
				{
					*statusFW1 = listAngleAz[numDx] - 40;
					*statusFW2 = listAngleAz[numDx] + 40;
					break;
				}
			}
			// 输出值域转换
			if (*statusFW1 > 360)
			{
				*statusFW1 = *statusFW1 - 360;
			}
			if (*statusFW1 < 0)
			{
				*statusFW1 = *statusFW1 + 360;
			}
			if (*statusFW2 > 360)
			{
				*statusFW2 = *statusFW2 - 360;
			}
			if (*statusFW2 < 0)
			{
				*statusFW2 = *statusFW2 + 360;
			}
		}
	}
	if (*statusN == 6)
	{
		if (listFP[numDx] == 2)
		{
			// 报2的存在,直接完成
			*statusN = 8;
			*statusFW3 = listAngleAz[numDx];
		}
		else
		{
			// 如果当前角度超过上边界90度，且内部角度大于10个有效测量，取最大值进行测量
			float ta1 = *statusFW2 - *statusFW1;
			float ta2 = 0;
			float ta2p = 0;
			float ta3 = 0;
			float ta4 = 0;
			// ta1取值归一
			if (ta1 > 180)
			{
				ta1 = ta1 - 360;
			}
			else if (ta1 < -180)
			{
				ta1 = ta1 + 360;
			}
			// 根据ta1方向确定结束区间
			if (ta1 < 0)
			{
				ta2 = listAngleAz[numDx] - *statusFW2;
				if (ta2 > 180)
				{
					ta2 = ta2 - 360;
				}
				else if (ta2 < -180)
				{
					ta2 = ta2 + 360;
				}
				// 瞄准判决
				if (ta2 < 0 && ta2 > -90)
				{
					// 区间内角度点数查找，并计算最大值位置
					int tc = 0;
					float maxAm = 0;
					float maxAngle = 0;
					for (int i = 0; i <= numDx; i++)
					{
						ta3 = listAngleAz[i] - *statusFW2;
						if (ta3 > 180)
						{
							ta3 = ta3 - 360;
						}
						else if (ta3 < -180)
						{
							ta3 = ta3 + 360;
						}
						ta4 = listAngleAz[i] - *statusFW1;
						if (ta4 > 180)
						{
							ta4 = ta4 - 360;
						}
						else if (ta4 < -180)
						{
							ta4 = ta4 + 360;
						}
						if (ta3 >= 0 && ta4 <= 0)
						{
							tc++;
							if (listAmD[i] > maxAm)
							{
								maxAm = listAmD[i];
								maxAngle = listAngleAz[i];
							}
						}
					}
					if (tc >= 10 && maxAm > 0)
					{
						//状态转移
						*statusN = 8;
						*statusFW3 = maxAngle;
					}
				}
			}
			else if (ta1 > 0 && ta1 < 70)
			{
				ta2 = listAngleAz[numDx] - *statusFW2;
				if (ta2 > 180)
				{
					ta2 = ta2 - 360;
				}
				else if (ta2 < -180)
				{
					ta2 = ta2 + 360;
				}
				// 瞄准判决
				if (ta2 > 0 && ta2 < 90)
				{
					// 区间内角度点数查找，并计算最大值位置
					int tc = 0;
					float maxAm = 0;
					float maxAngle = 0;
					for (int i = 0; i <= numDx; i++)
					{
						ta3 = listAngleAz[i] - *statusFW1;
						if (ta3 > 180)
						{
							ta3 = ta3 - 360;
						}
						else if (ta3 < -180)
						{
							ta3 = ta3 + 360;
						}
						ta4 = listAngleAz[i] - *statusFW2;
						if (ta4 > 180)
						{
							ta4 = ta4 - 360;
						}
						else if (ta4 < -180)
						{
							ta4 = ta4 + 360;
						}
						if (ta3 >= 0 && ta4 <= 0)
						{
							tc++;
							if (listAmD[i] > maxAm)
							{
								maxAm = listAmD[i];
								maxAngle = listAngleAz[i];
							}
						}
					}
					if (tc >= 10 && maxAm > 0)
					{
						//状态转移
						*statusN = 8;
						*statusFW3 = maxAngle;
					}
				}
			}
			else
			{
				ta2 = listAngleAz[numDx] - *statusFW2;
				if (ta2 > 180)
				{
					ta2 = ta2 - 360;
				}
				else if (ta2 < -180)
				{
					ta2 = ta2 + 360;
				}
				ta2p = listAngleAz[numDx] - *statusFW1;
				if (ta2p > 180)
				{
					ta2p = ta2p - 360;
				}
				else if (ta2p < -180)
				{
					ta2p = ta2p + 360;
				}
				// 瞄准判决
				if (ta2 > 0 || ta2p < 0)
				{
					// 区间内角度点数查找，并计算最大值位置
					int tc = 0;
					float maxAm = 0;
					float maxAngle = 0;
					for (int i = 0; i <= numDx; i++)
					{
						ta3 = listAngleAz[i] - *statusFW1;
						if (ta3 > 180)
						{
							ta3 = ta3 - 360;
						}
						else if (ta3 < -180)
						{
							ta3 = ta3 + 360;
						}
						ta4 = listAngleAz[i] - *statusFW2;
						if (ta4 > 180)
						{
							ta4 = ta4 - 360;
						}
						else if (ta4 < -180)
						{
							ta4 = ta4 + 360;
						}
						if (ta3 >= 0 && ta4 <= 0)
						{
							tc++;
							if (listAmD[i] > maxAm)
							{
								maxAm = listAmD[i];
								maxAngle = listAngleAz[i];
							}
						}
					}
					if (tc >= 10 && maxAm > 0)
					{
						//状态转移
						*statusN = 8;
						*statusFW3 = maxAngle;
					}
				}
			}
		}
	}
	if (*statusN == 8)
	{
		if (listAngleEl[numDx] >= 5 && listAngleEl[numDx] <= 15)
		{
			*statusN = 9;
		}
	}
	return 1;
}

// David20230807主进程函数-Wifi定向
int sfDxNewWifiProcessing(float ampQ, float ampD, float *listAngle, float *listAmQ, float *listAmD, float *listAmC, int *listFP, int numDx)
{
	/*
	输入描述
	ampQ：全向天线幅度，未识别则输入0数组
	ampD：定向天线幅度，未识别则输入0数组
	listAngle：每次定向时机对应方位角度索引(实际没用上)
	numDx：定向计数
	输出
	listAmQ：每次定向时机对应全向天线幅度索引
	listAmD：每次定向时机对应定向天线幅度索引
	listAmC：每次定向时机对应定向天线幅度索引
	listFP：每次定向时机对应计算的匹配情况索引
	*/

	// 都为空，则退出
	float tmpAm1 = ampQ;
	float tmpAm2 = ampD;

	if (tmpAm1 == 0 && tmpAm2 == 0)
	{
		listAmQ[numDx] = 0;
		listAmD[numDx] = 0;
		listAmC[numDx] = 0;
		listFP[numDx] = -1;
		return 0;
	}
	else
	{
		listAmC[numDx] = 1;
	}

	// 更新定向阶段信息数据流
	listAmQ[numDx] = tmpAm1;
	listAmD[numDx] = tmpAm2;
	float tmpAmC = 0;
	if (listAmQ[numDx] != 0 && listAmD[numDx] != 0)
	{
		tmpAmC = tmpAm2 - tmpAm1 - corQD;// 要做匹配修正
	}
	else
	{
		tmpAmC = 0;
	}
	// 根据信息流队列计算目标匹配结果
	if (tmpAmC > 750)
	{
		listFP[numDx] = 2;
	}
	else if (tmpAmC > 550)
	{
		listFP[numDx] = 1;
	}
	else
	{
		listFP[numDx] = 0;
	}

	return 0;
}

// 20230816-图传检测主函数-外部调用
void sfMoniSniffer(struct blockGroup *droneBlock, int *nBlock, unsigned short ***downMat, int chn, int nRows, int nCols, float cenFreq, float gain, float dT, float dF, float *moniF, int nMoni, float bwSignal, float bwDetect)
{
	int i = 0, j = 0, k = 0;

	// 模拟标志及相关计算函数参数定义
	int flagDet = 0;
	int indexF = 0;
	int passBot = 0;
	int passTop = 0;
	int passMean = 0;
	float meanAm = 0;
	float inBwSignal = 0;
	float inBwDetect = 0;
	int detF1 = 0;
	int detF2 = 0;

	// 设计初始门限
	float thresholdD1 = 0;
	float thresholdD2 = 0;

	// 模拟图传逐个频点比较
	for (i = 0; i < nMoni; i++)
	{
		// 门限由表格获取
		thresholdD1 = moniT[i] + 6 * 85;
		// 初始化
		flagDet = 0;
		indexF = 0;
		passBot = 0;
		passTop = 0;
		passMean = 0;
		meanAm = 0;

		// 信号带宽及检测带宽数值定义
		if (moniF[i] > 4000)
		{
			inBwSignal = 8;
			inBwDetect = 12;
		}
		else
		{
			inBwSignal = 14;
			inBwDetect = 20;// 20230816根据实测结果调整1.2G检测边带范围，单边向外扩2M
		}
		// 比较模拟图传频段是否在当前带宽内
		indexF = 64 + (int)round((moniF[i] - cenFreq) / 0.4);
		detF1 = (int)(inBwSignal / 2 / 0.4);
		detF2 = (int)(inBwDetect / 2 / 0.4);

		if (indexF - detF1 < 14 || indexF + detF1 > 114)
		{
			continue;
		}
		// 计算带宽内最高功率过门限点数
		static float Cal2[3000];
		for (j = 0 + 1; j < 3000 - 1; j++)
		{
			Cal2[j] = 0;
			for (k = indexF - detF1; k <= indexF + detF1; k++)
			{
				if (Cal2[j] < downMat[chn][j][k])
				{
					Cal2[j] = downMat[chn][j][k];
				}
			}
			// 过门限判定
			if (Cal2[j] > thresholdD1) 
			{
				passMean = passMean + 1;
			}
			meanAm = meanAm + Cal2[j];
		}
		meanAm = meanAm / 2998;
		// 计算上下边带过门限点数-修改门限为17dB 20230821
		if (meanAm - 85 * 17 > thresholdD1)
		{
			thresholdD2 = meanAm - 85 * 17;
		}
		else
		{
			thresholdD2 = thresholdD1;
		}
		// 重新计算上下边带门限及过门限点数-20230814
		passBot = 0;
		passTop = 0;
		for (j = 0; j < 3000; j++)
		{
			if (downMat[chn][j][indexF - detF2] > thresholdD2)
			{
				passBot = passBot + 1;
			}
			if (downMat[chn][j][indexF + detF2] > thresholdD2)
			{
				passTop = passTop + 1;
			}
		}
		// 当前目标识别判定
		if (passBot > 400 || passTop > 400 || passMean < 2700)
		{
			continue;
		}
		// 识别参数填写
		int tmpBlock = *nBlock;
		sfMoniToTarget(droneBlock, tmpBlock, cenFreq, moniF[i], bwSignal, meanAm);
		*nBlock = *nBlock + 1;
	}
}


//David20230808-模拟图传参数转目标参数
void sfMoniToTarget(struct blockGroup *droneBlock, int nBlock, float cenFreq, float tarFreq, float tarBW, float tarAm)
{
	int i = 0, j = 0, k = 0;
	if (nBlock >= MaxDrone)
		return;

	droneBlock[nBlock].id = nBlock;
	droneBlock[nBlock].index = 24; // 第24个对应FPV报文
	droneBlock[nBlock].range = 100;
	droneBlock[nBlock].bw = tarBW;

	droneBlock[nBlock].n = 2;
	for (j = 0; j < NCh; j++)
	{
		droneBlock[nBlock].inCh[j] = 0;
	}
	for (j = 0; j < 2; j++)
	{
		droneBlock[nBlock].freq[j] = tarFreq;
		droneBlock[nBlock].burstTime[j] = 0;
		droneBlock[nBlock].width[j] = 120;
		droneBlock[nBlock].dist[j] = 0;
		for (k = 0; k < NCh; k++)
		{
			droneBlock[nBlock].amp[k][j] = tarAm;
		}
	}
	nBlock++;
}
