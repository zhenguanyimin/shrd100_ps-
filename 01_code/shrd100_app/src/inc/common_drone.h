#ifndef COMMON_DRONE_H
#define COMMON_DRONE_H

#include <stdio.h>

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

#endif /* COMMON_DRONE_H */
