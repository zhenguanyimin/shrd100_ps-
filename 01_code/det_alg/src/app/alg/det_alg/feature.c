#include <stdio.h>
#include "feature.h"
#include "preProcess.h"

/*
 * 打印特征库参数
 */
void printUavlib(struct DroneLib *UAVtypes, int nUAV)
{
    int i = 0, j = 0;
    for (i = 0; i < nUAV; i++)
    {
        printf("id=%d\n", i);
        printf("name=%s\n", UAVtypes[i].name);
        printf("downOrUp=%d\n", UAVtypes[i].downOrUp);
        printf("freqPoints=");
        for (j = 0; j < UAVtypes[i].nfreq; j++)
            printf("%.1f,", UAVtypes[i].freqPoints[j]);
        printf("\n");
        printf("nfreqp=%d\n", UAVtypes[i].nfreq);
        printf("isFixedFreq=%d\n", UAVtypes[i].isFixedFreq);
        printf("hoppType=%d\n", UAVtypes[i].hoppType);
        printf("pulseW=");
        for (j = 0; j < UAVtypes[i].nPulseW; j++)
            printf("%.2f,", UAVtypes[i].pulseW[j]);
        printf("\n");
        printf("isFixedPulseW=%d\n", UAVtypes[i].isFixedPulseW);
        printf("pulseT=");
        for (j = 0; j < UAVtypes[i].nPulseT; j++)
            printf("%.2f,", UAVtypes[i].pulseT[j]);
        printf("\n");
        printf("nPulseT=%d\n", UAVtypes[i].nPulseT);
        printf("pulseBW=");
        for (j = 0; j < UAVtypes[i].nPulseBW; j++)
            printf("%.1f,", UAVtypes[i].pulseBW[j]);
        printf("\n");
        printf("pulseWErr=");
        for (j = 0; j < UAVtypes[i].nPulseW; j++)
            printf("%.2f,", UAVtypes[i].pulseWErr[j]);
        printf("\n");
        printf("meetPulseW=");
        for (j = 0; j < UAVtypes[i].nPulseW; j++)
            printf("%d,", UAVtypes[i].meetPulseW[j]);
        printf("\n");
        printf("pulseTErr=%.1f\n", UAVtypes[i].pulseTErr);
        printf("hoppCnt=%d\n", UAVtypes[i].hoppCnt);
        printf("freqErr=%.1f\n", UAVtypes[i].freqErr);
        printf("method=%d\n", UAVtypes[i].method);
        printf("SNR=%.1f\n", UAVtypes[i].SNR);
        printf("duty=%.3f\n\n", UAVtypes[i].duty);
    }
}

