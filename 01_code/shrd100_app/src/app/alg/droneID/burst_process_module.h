#ifndef BURST_PROCESS_MODULE_H_
#define BURST_PROCESS_MODULE_H_

#include "droneID_system_param.h"

/*==================================================================================================
*                                      DEFINES AND MACROS
==================================================================================================*/


/*=================================================================================================
*                                STRUCTURES AND OTHER TYPEDEFS
=================================================================================================*/


/*==================================================================================================
*                                GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/


/*==================================================================================================
*                                    FUNCTION PROTOTYPES
==================================================================================================*/
int32_t PadBurstProcessing(uint64_t *inputData, uint32_t dataLength, DroneID_Signal_t *droneID);
void ParseBurstData(uint64_t *inputData, Cplxf_t *padBurst, Cplxf_t *padBurst2, uint32_t dataLength);
void CompFreqOffset(Cplxf_t *padBurst, uint32_t burstLength);
void LowPassFiltering(Cplxf_t *padBurst);
int32_t FindSTO(Cplxf_t *padBurst);
int32_t FindStoCpOpt1(Cplxf_t *padBurst, Cplxf_t *cpCorrScores);
void CompCoarseCFO(Cplxf_t *padBurst, int32_t sto, uint32_t length);
void CompCoarseCfoMutiCp(Cplxf_t *padBurst, Cplxf_t *cpCorrScores, int32_t sto, uint32_t length);
void ScDataRecovery(Cplxf_t *padBurst, int32_t sto, Cplxf_t *scBuff);
void ChEqualizer(Cplxf_t *scBuff, Cplxf_t *dataScBuff);
void EstimateChannel(Cplxf_t *rxData, Cplxf_t *chEst);
void ChEqualizerMvAvg(Cplxf_t *scBuff, Cplxf_t *dataScBuff);
void EstimateChannelMvAvg(Cplxf_t *rxData, Cplxf_t *chEst, int32_t zcIdx, uint32_t N_wind);
void CalcChannelZcSnr(Cplxf_t *rxData, int32_t zcIdx, float *zcSnr);
void DeMapping(Cplxf_t *dataScBuff, int8_t *deMapOut);
void SoftDeMapping(Cplxf_t *dataScBuff, float *deMapOut);
void DeScrambler(int32_t *scrSeq, int32_t *lfsr1, int32_t *lfsr2, int8_t *deMapOut);
void SoftDeScrambler(int32_t *scrSeq, int32_t *lfsr1, int32_t *lfsr2, float *softDeMapOut, int8_t *deMapOut);
void DeFrame(uint8_t *decodeOut, DroneID_Signal_t *droneID, int32_t num);
void GetField(uint8_t *src, uint8_t *dest, int8_t byteSize, int8_t isReorder);
bool DroneIdCrc16Check(uint8_t *decodeOut, DroneID_Signal_t *droneID);
bool DroneIdCrc24aCheck(uint8_t *decodeOut);
bool DroneIdCrc24aTableCheck(uint8_t *decodeOut);
void MimoMaxRatioComb(Cplxf_t *scBuff, Cplxf_t *scBuff2, Cplxf_t *dataScBuff);
void MimoMaxRatioCombV1(Cplxf_t *scBuff, Cplxf_t *scBuff2, Cplxf_t *dataScBuff);
void CalcEvm(Cplxf_t *dataScBuff, float *evm);
void DSPF_sp_PL_fftSPxSP_cn(float* g_x_sp, float* g_y_sp);
void EstimateChannelLsDft(Cplxf_t *rxData, Cplxf_t *chEst, uint32_t zcIdx, uint32_t storePathLen);
void EstimatePhaseDiff(Cplxf_t *scBuff, Cplxf_t *chEst4);
void EstimatePhaseDiffCompAllSym(Cplxf_t *scBuff, Cplxf_t *chEst4, Cplxf_t *chEst6, Cplxf_t *chEst5);
void CalcFreqDomainSnr(Cplxf_t *rxData, Cplxf_t *chEst5, int32_t rxIdx, float *pPn, float *pSnr);


#endif

