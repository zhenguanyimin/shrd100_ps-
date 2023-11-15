#ifndef _FEATURE_H__
#define _FEATURE_H__

#define maxPulseWNumInLib 5  //

//特征库，部分字段仅对图传有效，部分字段仅对遥控有效。建议后续分拆为两个特征库
struct DroneLib
{
    char  name[50];					//名称
	int   psbID[10];				//可能的机型序号
	int   nPsbID;					//机型序号可能性数量
    char  downOrUp;					//图传或遥控，下行down是图传，上行up是遥控
    float freqPoints[60];			//频点。对应图传的信道，遥控的跳频点，MHz
    int   nfreq;					//频点个数
    char  isFixedFreq;				//freqPoints中是频段范围还是频点，若是0表示对应频段下上限，若是1则对应频点
    int   hoppType;					//跳频类型，该参数仅对遥控有效。0严格按照频表在相应时间相应频点发出信号，1按照周期跳频且大部分频率不变，2只需要周期跳频即可
    float pulseW[maxPulseWNumInLib];//脉冲宽度特征，ms。
    int   nPulseW;					//宽度种类数量
    char  isFixedPulseW;			//脉宽是否固定，0不固定，1固定
    float pulseT[5];				//周期，ms
    int   nPulseT;					//周期种类
    float pulseBW[5];				//带宽大小，MHz
    int   nPulseBW;					//带宽种类
    float pulseWErr[maxPulseWNumInLib];//脉冲宽度误差容限，ms
    int   meetPulseW[maxPulseWNumInLib];//满足脉冲宽度的最低次数
    float pulseTErr;				//脉冲时间的周期误差，ms
    int   hoppCnt;					//满足规律跳频的最低次数
    float freqErr;					//频率误差容限,MHz
    int   method;					//使用方法，0对应图传，1对应遥控
    float SNR;						//最低信噪比门限，dB
    float duty;						//信道占空比
};

#endif

