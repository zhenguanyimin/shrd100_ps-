#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include "preProcess.h"
#include "upLoadSniffer.h"

typedef struct QNode
{
    int data;
    struct QNode *next;
}QNode;


typedef struct Queue
{
    struct QNode* first;
    struct QNode* last;
}Queue;


void pushQueue(Queue *queue, int data)
{
    QNode *p = NULL;
    p = (QNode*)malloc(sizeof(QNode));

    p->data = data;

    if(queue->first == NULL)
    {
        queue->first = p;
        queue->last = p;
        p->next = NULL;
    }
    else
    {
        p->next = NULL;
        queue->last->next = p;
        queue->last = p;
    }
}


int popQueue(Queue *queue)
{
    QNode *p = NULL;
    int data;
    if(queue->first == NULL)
    {
        return -1;
    }

    p = queue->first;
    data = p->data;
    if(queue->first->next == NULL)
    {
        queue->first = NULL;
        queue->last = NULL;
    }
    else
    {
        queue->first = p->next;
    }

    free(p);

    return data;
}


//搜索4连通邻域
void searchNeighbor(float **mat, int nRows, int nCols, int **matMark, int label, int ix, int iy, float ctrSNR, Queue *queue)
{
    int i, ix1, iy1;    
    int direc[4][2] = { { 0, 1 }, { 1, 0 }, { 0, -1 }, { -1, 0 } };
    
    matMark[ix][iy] = label;    

    for (i = 0; i < 4; i++)
    {
        ix1 = ix + direc[i][0];
        iy1 = iy + direc[i][1];
        if (ix1 >= 0 && ix1 < nRows && iy1 >= 0 && iy1 < nCols)
        {
            if(mat[ix1][iy1]>ctrSNR && matMark[ix1][iy1]==0)
            {
                matMark[ix1][iy1]=label;
                pushQueue(queue, ix1*nCols+iy1);
            }
        }
    }
}

//标记连通区域
int markConn(float **mat, int **matMark, int nRows, int nCols, int n1, int n2, float ctrSNR)
{    
	int i, j, ix1, iy1, popIndex, label = 0;
	Queue *queue = NULL;
	queue = (Queue*)malloc(sizeof(Queue));
	queue->first = NULL;
	queue->last = NULL;

	for(i = 0; i < nRows; i++)
	{
		for(j = 0; j < nCols; j++)
		{
			if(mat[i][j]>ctrSNR && matMark[i][j] == 0)
			{
				label++;
				searchNeighbor(mat, nRows, nCols, matMark, label, i, j, ctrSNR, queue);
				popIndex = popQueue(queue);
				while(popIndex > -1)
				{
                    ix1=popIndex/nCols;
                    iy1=popIndex % nCols;
					searchNeighbor(mat, nRows, nCols, matMark, label, ix1, iy1, ctrSNR, queue);
					popIndex = popQueue(queue);
				}
			}
		}
	}

	free(queue);
	return label;
}


//连通区域生成图块，并计算每个图块对应的信号频率、带宽、幅度等信息
int mark2Block(struct RmtBlock *blk, int **matMark, float ***specMat, int nRows, int nCols, int n1, int n2, int label, float dt, float dF, int antIndex, float cenFreq)
{
    static float minWidth = 0.1f;//最小不考虑的图块高度（对应信号持续时间），ms。
    static float maxWidth = 13.0f;//最大不考虑的图块高度，ms

    int nblkParam = 0;
    int marker = 0;
    struct RmtBlock blockParam[MaxBlockParam]={0.0f,};
    int nblk = 0;

    for (int i = 0; i < MaxBlockParam; i++)
    {
        blockParam[i].antIndex = antIndex;
        blockParam[i].burstTime = -1;
        blockParam[i].blkBW = 0.0;
        blockParam[i].freq = 0.0;
        for (int j = 0; j < NCh; j++)
            blockParam[i].meanAmp[j] = 0.0;
    }

    if (label > MaxBlockParam)
        nblkParam = MaxBlockParam;
    else
        nblkParam = label;

    for (int i = 0; i < nRows; i++)
    {
        for (int j = n1; j < n2; j++)
        {
            marker = matMark[i][j] - 1;
            if (marker >= 0 && marker < nblkParam)
            {
                if (blockParam[marker].burstTime < 0)
                    blockParam[marker].burstTime = i;
                blockParam[marker].blkW = i;
                blockParam[marker].blkBW++;
                blockParam[marker].freq += j * specMat[antIndex][i][j];
                for (int k = 0; k < NCh; k++)
                    blockParam[marker].meanAmp[k] += specMat[k][i][j];
            }
        }
    }

    for (int i = 0; i < nblkParam; i++)
    {
        if (blockParam[i].blkBW>0)
        {
            blockParam[i].freq = cenFreq - 0.5*nCols*dF+0.5*dF + dF * blockParam[i].freq / blockParam[i].meanAmp[antIndex];

            for (int j = 0; j < NCh; j++)
                blockParam[i].meanAmp[j] = blockParam[i].meanAmp[j] / blockParam[i].blkBW;
            blockParam[i].blkBW = blockParam[i].blkBW / (blockParam[i].blkW - blockParam[i].burstTime + 1) * dF;
            blockParam[i].blkW = (blockParam[i].blkW - blockParam[i].burstTime + 1) * dt;
            blockParam[i].burstTime = blockParam[i].burstTime * dt;
        }
    }

    for (int i = 0; i < nblkParam; i++)
    {
        if (nblk >= MaxRmtBlock)
            break;
        if (blockParam[i].blkW > minWidth && blockParam[i].blkW < maxWidth)
        {
            blk[nblk].antIndex = blockParam[i].antIndex;
            blk[nblk].freq = blockParam[i].freq;
            blk[nblk].blkBW = blockParam[i].blkBW;
            blk[nblk].burstTime = blockParam[i].burstTime;
            blk[nblk].blkW = blockParam[i].blkW;
            for (int j = 0; j < NCh; j++)
            	blk[nblk].meanAmp[j] = blockParam[i].meanAmp[j];
            nblk++;
        }
    }

    return nblk;
}

//是否满足周期规律
char isMatch(float t1, int nt1, int t1Index, float t2, int nt2, int t2Index, float *pulseT, int nPulseT, float pulseTErr)
{
    float diffTime = 0, tErr = 0;
    float sumPulseT = 0.0;
    int   nt = 0;
    for (int i = 0; i < nPulseT; i++)
        sumPulseT += pulseT[i];

    if (t1 <= t2)
    {
        diffTime = t2 - t1;
        diffTime = fmod(diffTime, sumPulseT);
        tErr = fabs(diffTime);
        while (1)
        {
            if (tErr < pulseTErr)
                return 1;
            if (diffTime < 0)
                break;
            diffTime -= pulseT[(t1Index + nt1 + nt) % nPulseT];
            nt++;
        }
        return 0;
    }
    else
        return isMatch(t2, nt2, t2Index, t1, nt1, t1Index, pulseT, nPulseT, pulseTErr);
}


//遥控信号图块检测
int rmtBlockDetect(struct RmtBlock *blk, float **CtrFiltedAmp, float ***specMat, int nRows, int nCols, float dT, float dF, int antIndex, float cenFreq, int *ctrSpan)
{
    static float ctrSNR = 500.0f;

    int label;
    int nblk = 0;
    int n1 = ctrSpan[0], n2 = ctrSpan[1];

    int **matMark = (int **)malloc(nRows * sizeof(int *));
    for (int i = 0; i < nRows; i++)
    {
        matMark[i] = (int *)malloc(nCols * sizeof(int));
        memset(*(matMark + i), 0, nCols * sizeof(int));
    }

    label = markConn(CtrFiltedAmp, matMark, nRows, nCols, n1, n2, ctrSNR);
    
    nblk = mark2Block(blk, matMark, specMat, nRows, nCols, n1, n2, label, dT, dF, antIndex, cenFreq);

    for (int i = 0; i < nRows; i++)
    {
        free(*(matMark + i));
    }
    free(matMark);

    return nblk;
}


//过滤图块对应的频率参数
int checkPulseFreq(struct RmtBlock *pulse, int nCtrPulse, int libIndex, int *freqIndex, int *checkedPulse, struct DroneLib *droneLib)
{
    int   j, k;
    int   nFreq, nChecked = 0;
    float freqErr = droneLib[libIndex].freqErr;
    float freqP, pulseBW0 = droneLib[libIndex].pulseBW[0];

    if (droneLib[libIndex].isFixedFreq)
    {
        nFreq = droneLib[libIndex].nfreq;
        for (j = 0; j < nCtrPulse; j++)
        {
            for (k = 0; k < nFreq; k++)
            {
                if (nChecked >= MaxRmtBlock)
                    return nChecked;
                freqP = droneLib[libIndex].freqPoints[k];
                if (pulseBW0 < 5)
                {
                    if (fabs(freqP - pulse[j].freq) < freqErr)
                    {
                        checkedPulse[nChecked] = j;
                        freqIndex[nChecked] = k;
                        nChecked++;
                        break;
                    }
                }
                else
                {
                    if (fabs(freqP + pulseBW0 / 2 - pulse[j].freq) < freqErr || fabs(freqP - pulseBW0 / 2 - pulse[j].freq) < freqErr || fabs(freqP - pulse[j].freq) < freqErr)
                    {
                        checkedPulse[nChecked] = j;
                        freqIndex[nChecked] = k;
                        nChecked++;
                        break;
                    }
                }
            }
        }
    }
    else
    {
        for (j = 0; j < nCtrPulse; j++)
        {
            if (pulse[j].freq > droneLib[libIndex].freqPoints[0] && pulse[j].freq < droneLib[libIndex].freqPoints[1])
            {
                checkedPulse[nChecked] = j;
                nChecked++;
            }
        }
    }
    return nChecked;
}


//幅度是否超差
char isAmpDiff(struct RmtBlock pulse1, struct RmtBlock pulse2)
{
    float ampErr = 300;

    if (fabs(pulse1.meanAmp[pulse1.antIndex] - pulse2.meanAmp[pulse2.antIndex]) > ampErr)
        return 1;
    else
        return 0;
}

//过滤图块对应的跳频和持续时长信息
int checkPulseCycleAndWidth(int *ctrMatch, int **match, struct RmtBlock *pulse, int libIndex, int *checkedPulse, int nChecked, int *freqIndex, int *maxnT, struct DroneLib *droneLib)
{
    int   j, k, p, q;
    float minPulseTime, maxPulseTime, sumPulseT;
    int   pj, pk;

    int   oneMatch[5][MaxNumRmtT];
    float oneMatchTimeErr[5][MaxNumRmtT];
    float oneMatchFreqErr[5][MaxNumRmtT];
    int   indexPulseT[MaxRmtBlock] = { 0, };

    float diffTime, resTime, TErr, freqErr;
    int   nt = 0, nT = 0, nBigT = 0;
    int   count = 0, maxCount = 0, maxIndex = 0;

    int  nFreq = droneLib[libIndex].nfreq;
    int  hoppCount, pulseWCount[MaxPulseInGroup];
    int  nCtrMatch = 0;
    char isNewCtr, isMeetPulseW;
    int  sameIndex, pulseNum1, pulseNum2;

    minPulseTime = pulse[checkedPulse[0]].burstTime;
    maxPulseTime = pulse[checkedPulse[0]].burstTime;
    for (j = 1; j < nChecked; j++)
    {
        pj = checkedPulse[j];
        if (pulse[pj].burstTime > maxPulseTime)
            maxPulseTime = pulse[pj].burstTime;
        if (pulse[pj].burstTime < minPulseTime)
            minPulseTime = pulse[pj].burstTime;
    }

    sumPulseT = 0.0;
    for (j = 0; j < droneLib[libIndex].nPulseT; j++)
    {
        sumPulseT += droneLib[libIndex].pulseT[j];
    }
    *maxnT = ((maxPulseTime - minPulseTime) / sumPulseT + 1) * droneLib[libIndex].nPulseT + 0.5;
    if (*maxnT > 99)
        *maxnT = 99;

    for (j = 0; j < nChecked; j++)
    {
        pj = checkedPulse[j];

        for (p = 0; p < droneLib[libIndex].nPulseT; p++)
        {
            oneMatch[p][0] = j;
            for (q = 1; q < MaxNumRmtT; q++)
                oneMatch[p][q] = -1;

            for (k = 0; k < nChecked; k++)
            {
                if (j != k)
                {
                    pk = checkedPulse[k];

                    if (isAmpDiff(pulse[pk], pulse[pj]))
                        continue;

                    diffTime = pulse[pk].burstTime - pulse[pj].burstTime;
                    if (diffTime < 0)
                        continue;

                    if (droneLib[libIndex].hoppType == 1)
                        freqErr = fabs(droneLib[libIndex].freqPoints[freqIndex[j]] - pulse[pk].freq);

                    nBigT = diffTime / sumPulseT;
                    resTime = diffTime - nBigT * sumPulseT;

                    nt = 0;
                    while (1)
                    {
                        TErr = fabs(resTime);
                        if (TErr < droneLib[libIndex].pulseTErr)
                        {
                            nT = droneLib[libIndex].nPulseT * nBigT + nt;
                            if (nT > 99)
                                continue;

                            if (droneLib[libIndex].hoppType == 0)
                                freqErr = fabs(droneLib[libIndex].freqPoints[(freqIndex[j] + nT) % nFreq] - pulse[pk].freq);

                            if (nT > 0 && nT < MaxNumRmtT && (freqErr < droneLib[libIndex].freqErr || droneLib[libIndex].hoppCnt < 1))
                            {
                                if (oneMatch[p][nT] < 0)
                                {
                                    oneMatch[p][nT] = k;
                                    oneMatchTimeErr[p][nT] = TErr;
                                    oneMatchFreqErr[p][nT] = freqErr;
                                }
                                else if (TErr < oneMatchTimeErr[p][nT] && freqErr < oneMatchFreqErr[p][nT])
                                {
                                    oneMatch[p][nT] = k;
                                    oneMatchTimeErr[p][nT] = TErr;
                                    oneMatchFreqErr[p][nT] = freqErr;
                                }
                            }
                            break;
                        }
                        if (resTime < 0)
                            break;
                        resTime -= droneLib[libIndex].pulseT[(p + nt) % droneLib[libIndex].nPulseT];
                        nt++;
                    }
                }
            }
        }

        maxCount = 0;
        maxIndex = 0;
        for (k = 0; k < droneLib[libIndex].nPulseT; k++)
        {
            count = 0;
            for (p = 0; p < MaxNumRmtT; p++)
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
        indexPulseT[j] = maxIndex;
        for (k = 0; k < MaxNumRmtT; k++)
            match[j][k] = oneMatch[maxIndex][k];
    }

    for (j = 0; j < nChecked; j++)
    {
        hoppCount = 0;
        for (k = 0; k < droneLib[libIndex].nPulseW; k++)
            pulseWCount[k] = 0;
        for (k = 0; k < (*maxnT) + 1; k++)
        {
            pk = match[j][k];
            if (match[j][k] >= 0)
            {
                hoppCount++;
                for (int p = 0; p < droneLib[libIndex].nPulseW; p++)
                {
                    if (fabs(droneLib[libIndex].pulseW[p] - pulse[checkedPulse[pk]].blkW) < droneLib[libIndex].pulseWErr[p])
                    {
                        pulseWCount[p]++;
                        break;
                    }
                }
            }
        }

        isMeetPulseW = 0;
        for (k = 0; k < droneLib[libIndex].nPulseW; k++)
        {
            if (pulseWCount[k] >= droneLib[libIndex].meetPulseW[k])
            {
                isMeetPulseW = 1;
                break;
            }
        }

        if (hoppCount >= droneLib[libIndex].hoppCnt && isMeetPulseW)
        {
            isNewCtr = 1;
            for (k = 0; k < nCtrMatch; k++)
            {
                for (p = 0; p < (*maxnT) + 1; p++)
                {
                    pj = match[j][p];
                    for (q = 0; q < (*maxnT) + 1; q++)
                    {
                        pk = match[ctrMatch[k]][q];
                        if (pj >= 0 && pk >= 0)
                        {
                            if (droneLib[libIndex].hoppType == 1)
                            {
                                if (isMatch(
                                        pulse[checkedPulse[pj]].burstTime, p, indexPulseT[j], pulse[checkedPulse[pk]].burstTime, q, indexPulseT[ctrMatch[k]], droneLib[libIndex].pulseT,
                                        droneLib[libIndex].nPulseT, droneLib[libIndex].pulseTErr))
                                {
                                    isNewCtr = 0;
                                    sameIndex = k;
                                    break;
                                }
                            }
                            else
                            {
                                if (fabs(pulse[checkedPulse[pj]].freq - pulse[checkedPulse[pk]].freq) < droneLib[libIndex].freqErr
                                    && isMatch(
                                           pulse[checkedPulse[pj]].burstTime, p, indexPulseT[j], pulse[checkedPulse[pk]].burstTime, q, indexPulseT[ctrMatch[k]], droneLib[libIndex].pulseT,
                                           droneLib[libIndex].nPulseT, droneLib[libIndex].pulseTErr))
                                {
                                    isNewCtr = 0;
                                    sameIndex = k;
                                    break;
                                }
                                else if (fabs(pulse[checkedPulse[pj]].burstTime - pulse[checkedPulse[pk]].burstTime) < droneLib[libIndex].pulseTErr)
                                {
                                    isNewCtr = 0;
                                    sameIndex = k;
                                    break;
                                }
                            }
                        }
                    }
                    if (!isNewCtr)
                        break;
                }
                if (!isNewCtr)
                    break;
            }

            if (isNewCtr)
            {
                ctrMatch[nCtrMatch] = j;
                nCtrMatch++;
                if (nCtrMatch >= MaxPulseInGroup)
                    break;
            }
            else
            {
                pulseNum1 = 0;
                pulseNum2 = 0;
                for (k = 0; k < (*maxnT) + 1; k++)
                {
                    if (match[j][k] >= 0)
                        pulseNum1++;
                    if (match[ctrMatch[sameIndex]][k] >= 0)
                        pulseNum2++;
                }
                if (pulseNum1 > pulseNum2)
                    ctrMatch[sameIndex] = j;
                else if (pulseNum1 == pulseNum2)
                {
                    if (pulse[checkedPulse[match[j][0]]].meanAmp[pulse[0].antIndex] > pulse[checkedPulse[match[ctrMatch[sameIndex]][0]]].meanAmp[pulse[0].antIndex])
                        ctrMatch[sameIndex] = j;
                }
            }
        }
    }

    return nCtrMatch;
}

//过滤图块对应的周期和持续时间信息
int checkPulseTAndWidth(int *ctrMatch, int **match, struct RmtBlock *pulse, int libIndex, int *checkedPulse, int nChecked, int *freqIndex, int *maxnT, struct DroneLib *droneLib)
{
    int j, k, p, q;
    int minPulseTime, maxPulseTime;
    int pj, pk;

    int   hoppCount, pulseWCount[MaxPulseInGroup];
    int   nCtrMatch = 0;
    char  isNewCtr, isMeetPulseW;
    int   sameIndex, pulseNum1, pulseNum2;

    int   oneMatch[5][MaxNumRmtT];
    float oneMatchTimeErr[5][MaxNumRmtT];
    int   indexPulseT[MaxRmtBlock] = {0,};

    float diffTime, resTime, TErr, sumPulseT = 0.0;
    int   nt = 0, nT = 0, nBigT = 0;
    int   count = 0, maxCount = 0, maxIndex = 0;

    minPulseTime = pulse[checkedPulse[0]].burstTime;
    maxPulseTime = pulse[checkedPulse[0]].burstTime;
    for (j = 1; j < nChecked; j++)
    {
        pj = checkedPulse[j];
        if (pulse[pj].burstTime > maxPulseTime)
            maxPulseTime = pulse[pj].burstTime;
        if (pulse[pj].burstTime < minPulseTime)
            minPulseTime = pulse[pj].burstTime;
    }

    sumPulseT = 0.0;
    for (j = 0; j < droneLib[libIndex].nPulseT; j++)
    {
        sumPulseT += droneLib[libIndex].pulseT[j];
    }
    *maxnT = ((maxPulseTime - minPulseTime) / sumPulseT + 1) * droneLib[libIndex].nPulseT + 0.5;

    for (j = 0; j < nChecked; j++)
    {
        pj = checkedPulse[j];

        for (p = 0; p < droneLib[libIndex].nPulseT; p++)
        {
            oneMatch[p][0] = j;
            for (q = 1; q < MaxNumRmtT; q++)
                oneMatch[p][q] = -1;

            for (k = 0; k < nChecked; k++)
            {
                if (j != k)
                {
                    pk = checkedPulse[k];

                    if (isAmpDiff(pulse[pk], pulse[pj]))
                        continue;

                    if (fabs(pulse[pk].freq - pulse[pj].freq) < 0.5)
                        continue;

                    diffTime = pulse[pk].burstTime - pulse[pj].burstTime;
                    if (diffTime < 0)
                        continue;

                    nBigT = diffTime / sumPulseT;
                    resTime = diffTime - nBigT * sumPulseT;

                    nt = 0;
                    while (1)
                    {
                        TErr = fabs(resTime);
                        if (TErr < droneLib[libIndex].pulseTErr)
                        {
                            nT = droneLib[libIndex].nPulseT * nBigT + nt;

                            if (nT > 0 && nT < MaxNumRmtT)
                            {
                                if (oneMatch[p][nT] < 0)
                                {
                                    oneMatch[p][nT] = k;
                                    oneMatchTimeErr[p][nT] = TErr;
                                }
                                else if (TErr < oneMatchTimeErr[p][nT])
                                {
                                    oneMatch[p][nT] = k;
                                    oneMatchTimeErr[p][nT] = TErr;
                                }
                            }
                            break;
                        }
                        if (resTime < 0)
                            break;

                        resTime -= droneLib[libIndex].pulseT[(p + nt) % droneLib[libIndex].nPulseT];
                        nt++;
                    }
                }
            }
        }

        maxCount = 0;
        maxIndex = 0;
        for (k = 0; k < droneLib[libIndex].nPulseT; k++)
        {
            count = 0;
            for (p = 0; p < MaxNumRmtT; p++)
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
        indexPulseT[j] = maxIndex;
        for (k = 0; k < MaxNumRmtT; k++)
            match[j][k] = oneMatch[maxIndex][k];
    }

    for (j = 0; j < nChecked; j++)
    {
        hoppCount = 0;
        for (k = 0; k < droneLib[libIndex].nPulseW; k++)
            pulseWCount[k] = 0;
        for (k = 0; k < (*maxnT) + 1; k++)
        {
            pk = match[j][k];
            if (match[j][k] >= 0)
            {
                hoppCount++;
                for (int p = 0; p < droneLib[libIndex].nPulseW; p++)
                {
                    if (fabs(droneLib[libIndex].pulseW[p] - pulse[checkedPulse[pk]].blkW) < droneLib[libIndex].pulseWErr[p])
                    {
                        pulseWCount[p]++;
                        break;
                    }
                }
            }
        }

        isMeetPulseW = 0;
        for (k = 0; k < droneLib[libIndex].nPulseW; k++)
        {
            if (pulseWCount[k] >= droneLib[libIndex].meetPulseW[k])
            {
                isMeetPulseW = 1;
                break;
            }
        }

        if (hoppCount >= droneLib[libIndex].hoppCnt && isMeetPulseW)
        {
            isNewCtr = 1;
            for (k = 0; k < nCtrMatch; k++)
            {
                for (p = 0; p < (*maxnT) + 1; p++)
                {
                    pj = match[j][p];
                    for (q = 0; q < (*maxnT) + 1; q++)
                    {
                        pk = match[ctrMatch[k]][q];
                        if (pj >= 0 && pk >= 0)
                        {
                            if (isMatch(
                                    pulse[checkedPulse[pj]].burstTime, p, indexPulseT[j], pulse[checkedPulse[pk]].burstTime, q, indexPulseT[ctrMatch[k]], droneLib[libIndex].pulseT,
                                    droneLib[libIndex].nPulseT, droneLib[libIndex].pulseTErr))
                            {
                                isNewCtr = 0;
                                sameIndex = k;
                                break;
                            }
                            else if (fabs(pulse[checkedPulse[pj]].burstTime - pulse[checkedPulse[pk]].burstTime) < droneLib[libIndex].pulseTErr)
                            {
                                isNewCtr = 0;
                                sameIndex = k;
                                break;
                            }
                        }
                    }
                    if (!isNewCtr)
                        break;
                }
                if (!isNewCtr)
                    break;
            }

            if (isNewCtr)
            {
                ctrMatch[nCtrMatch] = j;
                nCtrMatch++;
                if (nCtrMatch >= MaxPulseInGroup)
                    break;
            }
            else
            {
                pulseNum1 = 0;
                pulseNum2 = 0;
                for (k = 0; k < (*maxnT) + 1; k++)
                {
                    if (match[j][k] >= 0)
                        pulseNum1++;
                    if (match[ctrMatch[sameIndex]][k] >= 0)
                        pulseNum2++;
                }
                if (pulseNum1 > pulseNum2)
                    ctrMatch[sameIndex] = j;
                else if (pulseNum1 == pulseNum2)
                {
                    if (pulse[checkedPulse[match[j][0]]].meanAmp[pulse[0].antIndex] > pulse[checkedPulse[match[ctrMatch[sameIndex]][0]]].meanAmp[pulse[0].antIndex])
                        ctrMatch[sameIndex] = j;
                }
            }
        }
    }

    return nCtrMatch;
}

//过滤图块对应的持续时间信息
int checkPulseW(int *pulseWMatch, int *checkedFreqIndex, struct RmtBlock *pulse, int libIndex, int *checkedPulse, int nChecked, int *freqIndex, struct DroneLib *droneLib)
{
    int  j, k;
    int  pj;
    int  pulseWCount[maxPulseWNumInLib];
    int  nPulseWMatch = 0;
    char isNewCtr;


    for (j = 0; j < droneLib[libIndex].nPulseW; j++)
        pulseWCount[j] = 0;

    for (j = 0; j < nChecked; j++)
    {
        pj = checkedPulse[j];
        for (k = 0; k < droneLib[libIndex].nPulseW; k++)
        {

            if (fabs(pulse[pj].blkW - droneLib[libIndex].pulseW[k]) < droneLib[libIndex].pulseWErr[k] && pulse[pj].blkBW > 0.75 * droneLib[libIndex].pulseBW[0])
            {
                pulseWCount[k]++;
                pulseWMatch[nPulseWMatch] = pj;
                checkedFreqIndex[nPulseWMatch] = freqIndex[j];
                nPulseWMatch++;
            }
        }
    }

    isNewCtr = 1;
    for (int j = 0; j < droneLib[libIndex].nPulseW; j++)
    {
        if (pulseWCount[j] < droneLib[libIndex].meetPulseW[j])
        {
            isNewCtr = 0;
            break;
        }
    }
    if (!isNewCtr)
        nPulseWMatch = 0;
    return nPulseWMatch;
}

//从图块参数生成脉冲信号参数
int genUavCtrPulse(struct RmtBlock *pulse, int *checkRmtBlk, int nChecked, int *freqIndex, int libIndex, struct blockGroup *remoteBlock, int nBlocks, struct DroneLib *droneLib, int chNum)
{
    int j, k, pj;

    if (nBlocks >= MaxDrone)
        return nBlocks;

    remoteBlock[nBlocks].index = libIndex;
    remoteBlock[nBlocks].bw = droneLib[libIndex].pulseBW[0];
    for (k = 0; k < NCh; k++)
        remoteBlock[nBlocks].inCh[k] = 0;
    remoteBlock[nBlocks].inCh[pulse[checkRmtBlk[0]].antIndex] = 1;

    if (nChecked > MaxPulseInGroup)
        remoteBlock[nBlocks].n = MaxPulseInGroup;
    else
        remoteBlock[nBlocks].n = nChecked;
    for (j = 0; j < nChecked; j++)
    {
        if (j >= MaxPulseInGroup)
            break;
        pj = checkRmtBlk[j];
        if (droneLib[libIndex].hoppType == 1 && droneLib[libIndex].pulseBW[0] > 5)
            remoteBlock[nBlocks].freq[j] = droneLib[libIndex].freqPoints[freqIndex[j]];
        else
            remoteBlock[nBlocks].freq[j] = pulse[pj].freq;
        remoteBlock[nBlocks].burstTime[j] = pulse[pj].burstTime;
        remoteBlock[nBlocks].width[j] = pulse[pj].blkW;
        for (k = 0; k < chNum; k++)
        {
            remoteBlock[nBlocks].amp[k][j] = pulse[pj].meanAmp[k];
        }
    }
    nBlocks++;
    return nBlocks;
}

//识别遥控器类型
int checkRmtBlk(struct RmtBlock *pulse, int nCtrPulse, struct blockGroup *chCtrPulse, struct DroneLib *droneLib, int nUav, int chNum)
{
    if (nCtrPulse < 1)
    {
        return 0;
    }
    
    int   i, j, k;
    int   nFreq=0;
    int   checkedPulse[MaxRmtBlock];
    int   nChecked;
    int   pj, pk;
    int **match = (int **)malloc(MaxRmtBlock * sizeof(int *));
    for (i = 0; i < MaxRmtBlock; i++)
        match[i] = (int *)malloc(MaxNumRmtT * sizeof(int));

    int freqIndex[MaxRmtBlock];
    int checkedFreqIndex[MaxRmtBlock];
    int maxnT, nT;

    int nCtrMatch;
    int ctrMatch[MaxRmtBlock];
    int nPulseWMatch;
    int pulseWMatch[MaxRmtBlock];
    int nPulseC_WMatch;
    int PulseC_WMatch[MaxRmtBlock];

    int nChCtrPulse = 0;
    int count = 0;

    for (i = 0; i < nUav; i++)
    {
        if (droneLib[i].method != 2)
            continue;

        nFreq = droneLib[i].nfreq;
        nCtrMatch = 0;

        nChecked = checkPulseFreq(pulse, nCtrPulse, i, freqIndex, checkedPulse, droneLib);
        if (nChecked < 1)
            continue;

        if (droneLib[i].nPulseT > 0 && droneLib[i].pulseT[0] > 0)
        {
            if (droneLib[i].isFixedFreq && droneLib[i].hoppType < 2)
            {
                nCtrMatch = checkPulseCycleAndWidth(ctrMatch, match, pulse, i, checkedPulse, nChecked, freqIndex, &maxnT, droneLib);


                if (nCtrMatch >= 1 && droneLib[i].pulseT[0] > 30)
                {
                    for (j = 0; j < nCtrMatch; j++)
                    {
                        count = 0;
                        pj = ctrMatch[j];
                        for (k = 0; k < nChecked; k++)
                        {
                            pk = checkedPulse[k];
                            nT = fabs(pulse[checkedPulse[pj]].burstTime - pulse[pk].burstTime) / droneLib[i].pulseT[0] + 0.5;
                            if (fabs(droneLib[i].freqPoints[freqIndex[k]] - pulse[pk].freq) < droneLib[i].freqErr
                                && fabs(fabs(pulse[checkedPulse[pj]].burstTime - pulse[pk].burstTime) - nT * droneLib[i].pulseT[0]) > 5)
                            {
                                count++;
                            }
                        }
                        if (count > 2)
                        {
                            nCtrMatch = 0;
                            break;
                        }
                    }
                }
            }
            else
            {
                nCtrMatch = checkPulseTAndWidth(ctrMatch, match, pulse, i, checkedPulse, nChecked, freqIndex, &maxnT, droneLib);
            }

            for (j = 0; j < nCtrMatch; j++)
            {
                nPulseC_WMatch = 0;
                for (k = 0; k < maxnT; k++)
                {
                    pk = match[ctrMatch[j]][k];

                    if (pk >= 0)
                    {
                        PulseC_WMatch[nPulseC_WMatch] = checkedPulse[pk];
                        checkedFreqIndex[nPulseC_WMatch] = freqIndex[pk];
                        nPulseC_WMatch++;
                    }
                }
                nChCtrPulse = genUavCtrPulse(pulse, PulseC_WMatch, nPulseC_WMatch, checkedFreqIndex, i, chCtrPulse, nChCtrPulse, droneLib, chNum);
            }
        }
        else
        {
            nPulseWMatch = checkPulseW(pulseWMatch, checkedFreqIndex, pulse, i, checkedPulse, nChecked, freqIndex, droneLib);
            if (nPulseWMatch > 0)
            {
                nChCtrPulse = genUavCtrPulse(pulse, pulseWMatch, nPulseWMatch, checkedFreqIndex, i, chCtrPulse, nChCtrPulse, droneLib, chNum);
            }
        }
    }

    for (i = 0; i < MaxRmtBlock; i++)
    {
        free(match[i]);
        match[i] = NULL;
    }
    free(match);
    match = NULL;

    return nChCtrPulse;
}

//合并遥控图块参数，仅用于多个接收通道的情况
void mergeUavCtrPulse(struct blockGroup *remoteBlock, int *nBlocks, struct blockGroup *chCtrPulse, int nChCtrPulse, struct DroneLib *droneLib, int chNum)
{
    int j, k, p, q, r;

    char  isNew;
    int   uavCtrIndex;
    int   antCtrIndex;
    int   mergedFlag[MaxPulseInGroup];
    char  isMerged;
    float sumPulseT = 0.0;

    for (j = 0; j < nChCtrPulse; j++)
    {
        isNew = 1;
        for (k = 0; k < *nBlocks; k++)
        {
            if (chCtrPulse[j].index == remoteBlock[k].index)
            {
                for (p = 0; p < droneLib[chCtrPulse[j].index].nPulseT; p++)
                {
                    sumPulseT += droneLib[chCtrPulse[j].index].pulseT[p];
                }
                for (p = 0; p < chCtrPulse[j].n; p++)
                {
                    for (q = 0; q < remoteBlock[k].n; q++)
                    {
                        if (fabs(chCtrPulse[j].burstTime[p] - remoteBlock[k].burstTime[q]) < 2 && fabs(chCtrPulse[j].freq[p] - remoteBlock[k].freq[q]) < 1)
                        {
                            uavCtrIndex = k;
                            isNew = 0;
                            break;
                        }

                        if (droneLib[chCtrPulse[j].index].nPulseT > 0 && droneLib[chCtrPulse[j].index].pulseT[0] > 0)
                        {
                            if (fmod(fabs(chCtrPulse[j].burstTime[p] - remoteBlock[k].burstTime[q]), sumPulseT) < droneLib[chCtrPulse[j].index].pulseTErr)
                            {
                                if (droneLib[chCtrPulse[j].index].hoppType == 1)
                                {
                                    if (fabs(chCtrPulse[j].freq[p] - remoteBlock[k].freq[q]) < 1)
                                    {
                                        uavCtrIndex = k;
                                        isNew = 0;
                                        break;
                                    }
                                }
                                else
                                {
                                    uavCtrIndex = k;
                                    isNew = 0;
                                    break;
                                }
                            }
                        }
                    }
                    if (!isNew)
                        break;
                }
                if (!isNew)
                    break;
            }
        }
        if (isNew)
        {
            remoteBlock[*nBlocks].id = chCtrPulse[j].id;
            remoteBlock[*nBlocks].n = chCtrPulse[j].n;
            remoteBlock[*nBlocks].bw = chCtrPulse[j].bw;
            remoteBlock[*nBlocks].index = chCtrPulse[j].index;
            for (k = 0; k < chNum; k++)
            {
                remoteBlock[*nBlocks].inCh[k] = chCtrPulse[j].inCh[k];
            }
            for (k = 0; k < chCtrPulse[j].n; k++)
            {
                remoteBlock[*nBlocks].freq[k] = chCtrPulse[j].freq[k];
                remoteBlock[*nBlocks].burstTime[k] = chCtrPulse[j].burstTime[k];
                remoteBlock[*nBlocks].width[k] = chCtrPulse[j].width[k];
                for (p = 0; p < chNum; p++)
                {
                    remoteBlock[*nBlocks].amp[p][k] = chCtrPulse[j].amp[p][k];
                }
            }
            (*nBlocks)++;
        }
        else
        {
            for (k = 0; k < chNum; k++)
            {
                if (chCtrPulse[j].inCh[k])
                {
                    remoteBlock[uavCtrIndex].inCh[k] = 1;
                    antCtrIndex = k;
                    break;
                }
            }
            remoteBlock[uavCtrIndex].bw = (remoteBlock[uavCtrIndex].bw + chCtrPulse[j].bw) / 2.0;
            for (k = 0; k < MaxPulseInGroup; k++)
                mergedFlag[k] = -1;

            for (k = 0; k < remoteBlock[uavCtrIndex].n; k++)
            {
                for (p = 0; p < chCtrPulse[j].n; p++)
                {
                    if (droneLib[remoteBlock[uavCtrIndex].index].isFixedFreq > 0)
                    {
                        if (mergedFlag[p] < 0 && fabs(remoteBlock[uavCtrIndex].burstTime[k] - chCtrPulse[j].burstTime[p]) < 2 && fabs(remoteBlock[uavCtrIndex].freq[k] - chCtrPulse[j].freq[p]) < 1)
                        {
                            mergedFlag[p] = 1;
                            remoteBlock[uavCtrIndex].burstTime[k] = (remoteBlock[uavCtrIndex].burstTime[k] + chCtrPulse[j].burstTime[p]) / 2.0;
                            remoteBlock[uavCtrIndex].width[k] = (remoteBlock[uavCtrIndex].width[k] + chCtrPulse[j].width[p]) / 2.0;
                            remoteBlock[uavCtrIndex].freq[k] = (remoteBlock[uavCtrIndex].freq[k] + chCtrPulse[j].freq[p]) / 2.0;
                            remoteBlock[uavCtrIndex].amp[antCtrIndex][k] = (remoteBlock[uavCtrIndex].amp[antCtrIndex][k] + chCtrPulse[j].amp[antCtrIndex][p]) / 2.0;
                        }
                    }
                    else
                    {
                        if (mergedFlag[p] < 0 && fabs(remoteBlock[uavCtrIndex].burstTime[k] - chCtrPulse[j].burstTime[p]) < 2)
                        {
                            mergedFlag[p] = 1;
                            if (remoteBlock[uavCtrIndex].amp[antCtrIndex][k] < chCtrPulse[j].amp[antCtrIndex][p])
                            {
                                remoteBlock[uavCtrIndex].burstTime[k] = chCtrPulse[j].burstTime[p];
                                remoteBlock[uavCtrIndex].width[k] = chCtrPulse[j].width[p];
                                remoteBlock[uavCtrIndex].freq[k] = chCtrPulse[j].freq[p];
                                remoteBlock[uavCtrIndex].amp[antCtrIndex][k] = chCtrPulse[j].amp[antCtrIndex][p];
                            }
                        }
                    }
                }
            }

            for (k = 0; k < chCtrPulse[j].n; k++)
            {
                if (remoteBlock[uavCtrIndex].n >= MaxPulseInGroup)
                {
                    break;
                }
                if (mergedFlag[k] < 0)
                {
                    isMerged = 0;
                    for (p = 0; p < remoteBlock[uavCtrIndex].n; p++)
                    {
                        if (remoteBlock[uavCtrIndex].burstTime[p] > chCtrPulse[j].burstTime[k])
                        {
                            isMerged = 1;
                            if (remoteBlock[uavCtrIndex].n >= MaxPulseInGroup)
                            {
                                break;
                            }
                            remoteBlock[uavCtrIndex].n++;

                            for (q = remoteBlock[uavCtrIndex].n - 1; q > p; q--)
                            {
                                remoteBlock[uavCtrIndex].burstTime[q] = remoteBlock[uavCtrIndex].burstTime[q - 1];
                                remoteBlock[uavCtrIndex].width[q] = remoteBlock[uavCtrIndex].width[q - 1];
                                remoteBlock[uavCtrIndex].freq[q] = remoteBlock[uavCtrIndex].freq[q - 1];
                                for (r = 0; r < chNum; r++)
                                {
                                    remoteBlock[uavCtrIndex].amp[r][q] = remoteBlock[uavCtrIndex].amp[r][q - 1];
                                }
                            }

                            remoteBlock[uavCtrIndex].burstTime[p] = chCtrPulse[j].burstTime[k];
                            remoteBlock[uavCtrIndex].width[p] = chCtrPulse[j].width[k];
                            remoteBlock[uavCtrIndex].freq[p] = chCtrPulse[j].freq[k];
                            for (r = 0; r < chNum; r++)
                            {
                                remoteBlock[uavCtrIndex].amp[r][p] = chCtrPulse[j].amp[r][k];
                            }
                            break;
                        }
                    }
                    if (!isMerged)
                    {
                        if (remoteBlock[uavCtrIndex].n >= MaxPulseInGroup)
                        {
                            break;
                        }
                        remoteBlock[uavCtrIndex].burstTime[remoteBlock[uavCtrIndex].n] = chCtrPulse[j].burstTime[k];
                        remoteBlock[uavCtrIndex].width[remoteBlock[uavCtrIndex].n] = chCtrPulse[j].width[k];
                        remoteBlock[uavCtrIndex].freq[remoteBlock[uavCtrIndex].n] = chCtrPulse[j].freq[k];
                        for (r = 0; r < chNum; r++)
                        {
                            remoteBlock[uavCtrIndex].amp[r][remoteBlock[uavCtrIndex].n] = chCtrPulse[j].amp[r][k];
                        }
                        remoteBlock[uavCtrIndex].n++;
                    }
                }
            }
        }
    }
}


//计算遥控的距离参数
void calcUavCtrPulse(
    struct blockGroup *remoteBlock, int nBlocks, float ***specMat, int nRows, int nCols, float dt, float dF, float cenFreq, float gain, struct DroneLib *droneLib)
{
    int   i, j, k;

    float maxAmp=-1000.0;
    float range = 0.0;

    for (i = 0; i < nBlocks; i++)
    {
        for (j = 0; j < remoteBlock[i].n; j++)
        {
            for (k = 0; k < NCh; k++)
            {
                if(remoteBlock[i].amp[k][j]>maxAmp)
                    maxAmp=remoteBlock[i].amp[k][j];
            }

            remoteBlock[i].dist[j] = amp2dist(maxAmp, remoteBlock[i].freq[j], gain);
            range += remoteBlock[i].dist[j];
        }

        remoteBlock[i].range = range / remoteBlock[i].n;

        remoteBlock[i].id = i;
    }
}

//信息打印
void printRemoteBlock(struct blockGroup *remoteBlock, int nBlocks, struct DroneLib *droneLib)
{
    for (int i = 0; i < nBlocks; i++)
    {
        printf("**********************remote*********************\n");
        printf("ID=%d,%s\n", remoteBlock[i].id, droneLib[remoteBlock[i].index].name);
        printf("bandWidth=%.1f\n", remoteBlock[i].bw);
        printf("inCh:");
        for (int j = 0; j < NCh; j++)
        {
            if (remoteBlock[i].inCh[j])
                printf("%d,", j + 1);
        }
        printf("\n");
        for (int j = 0; j < remoteBlock[i].n; j++)
        {
            printf("freq=%.1f,burstTime=%.1f,blkW=%.2f,dist=%.0f\n", remoteBlock[i].freq[j], remoteBlock[i].burstTime[j], remoteBlock[i].width[j],remoteBlock[i].dist[j]);
        }
    }
    if(nBlocks>0)
        printf("*******************************************\n");
}

//遥控检测主函数
void upLoadSniffer(
    struct blockGroup *remoteBlock, int *nBlocks, float ***specMat, int nRows, int nCols, float cenFreq, float gain, float dt, float dF, struct DroneLib *droneLib, int nUAV, int *ctrSpan)
{   
    struct RmtBlock   pulse[MaxRmtBlock];
    int               nPulse;
    struct blockGroup chCtrPulse[MaxDrone];
    int               nChCtrPulse = 0;
    *nBlocks = 0;
    
    float ***filtedAmp = createMat(NCh, nRows, nCols);

    for (int i = 0; i < NCh; i++)
    {
        conv2(*(specMat + i), nRows, nCols, *(filtedAmp + i), HpMask, HpMaskH, HpMaskW, ctrSpan);
    }

    for (int i = 0; i < NCh; i++)
    {
        nPulse = rmtBlockDetect(pulse, *(filtedAmp + i), specMat, nRows, nCols, dt, dF, i, cenFreq, ctrSpan);

        nChCtrPulse = checkRmtBlk(pulse, nPulse, chCtrPulse, droneLib, nUAV, NCh);

        mergeUavCtrPulse(remoteBlock, nBlocks, chCtrPulse, nChCtrPulse, droneLib, NCh);
    }

    calcUavCtrPulse(remoteBlock, *nBlocks, specMat, nRows, nCols, dt, dF, cenFreq, gain, droneLib);

    freeMat(filtedAmp, NCh, nRows);
}

//遥控检测主函数，二维卷积作为参数输入的版本
void upLoadSniffer_noFilt(
    struct blockGroup *remoteBlock, int *nBlocks, float ***downMat, float ***upMat, int nRows, int nCols, float cenFreq, float gain, float dt, float dF, struct DroneLib *droneLib, int nUAV, int *ctrSpan)
{   
    struct RmtBlock   pulse[MaxRmtBlock];
    int               nPulse;
    struct blockGroup chCtrPulse[MaxDrone];
    int               nChCtrPulse = 0;
    *nBlocks = 0;

    for (int i = 0; i < NCh; i++)
    {
        nPulse = rmtBlockDetect(pulse, *(upMat + i), downMat, nRows, nCols, dt, dF, i, cenFreq, ctrSpan);

        nChCtrPulse = checkRmtBlk(pulse, nPulse, chCtrPulse, droneLib, nUAV, NCh);

        mergeUavCtrPulse(remoteBlock, nBlocks, chCtrPulse, nChCtrPulse, droneLib, NCh);
    }

    calcUavCtrPulse(remoteBlock, *nBlocks, downMat, nRows, nCols, dt, dF, cenFreq, gain, droneLib);

}

