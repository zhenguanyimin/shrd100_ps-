#ifndef EXTRACT_PROCESS_MODULE_H_
#define EXTRACT_PROCESS_MODULE_H_

#include "droneID_system_param.h"

/*==================================================================================================
*                                      DEFINES AND MACROS
==================================================================================================*/
/* < xcorr > */
#define STD_FILTER    					(0.023932486908359f)
#define SIZE_WINDOW                     (1024u)
#define INVERSE_WINDOW_SIZE             (0.0009765625f)

#define THRESHOLD_XCORR                 (0.7f)
#define THRESHOLD_POD_ABS               (1600)
#define THRESHOLD_POD_SQUARE            (THRESHOLD_POD_ABS * THRESHOLD_POD_ABS)


/*=================================================================================================
*                                STRUCTURES AND OTHER TYPEDEFS
=================================================================================================*/


/*==================================================================================================
*                                GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/


/*==================================================================================================
*                                    FUNCTION PROTOTYPES
==================================================================================================*/
//uint32_t ExtractBurstProcessing(Cplxint32_t *adcBuff, uint32_t *idxBuff, int32_t *scoreBuff);
uint32_t ExtractBurstProcessing(Cplxint32_t *adcBuff, uint32_t *idxBuff);
Cplxf_t CalcMean(Cplxf_t *winBuff, uint32_t length);
//Cplxf_t InnerProduct(Cplxf_t *x, Cplxf_t *y, uint32_t length);
Cplxint32_t InnerProduct(Cplxint32_t *x, Cplxint32_t *y, uint32_t length);
float CalcStd(Cplxf_t *x, Cplxf_t mean, uint32_t length, Cplxf_t *x0);


#endif
