#ifndef CALIBRATION_PARAMETER_CONFIG_H
#define CALIBRATION_PARAMETER_CONFIG_H

#include "xgpio.h"
#include <stdio.h>

/*!
 *  @brief   Structure for detected points
 *
 */
typedef struct dacVoltage_t
{
	uint32_t    Votage[8][2];             /* dac voltge, start and end*/
} dacVoltage;

typedef enum {
    dataSelectAID       		= 1U,
//	dataSelectBID,
//	dataCalibrationA0ID,
//	dataCalibrationA1ID,
//	dataCalibrationA2ID,
//	dataCalibrationA3ID,
//	dataCalibrationA4ID,
//	dataCalibrationA5ID,
//	dataCalibrationB0ID,
//	dataCalibrationB1ID,
//	dataCalibrationB2ID,
//	dataCalibrationB3ID,
//	dataCalibrationB4ID,
//	dataCalibrationB5ID,
//    dataMulyipleAID,
//	dataMulyipleBID,
	dataDacVolt1 = 2,
	dataDacVolt2 = 3,
	dataDacVolt3 = 4,
	dataDacVolt4,
	dataDacVolt5,
	dataDacVolt6,
	dataDacVolt7,
	dataDacVolt8,
	dataDacVolt9,
	dataDacVolt10,
	dataDacVolt11,
	dataDacVolt12,
	/* add new parameter */
	dataInvalidID
} strick_fre_reg_id_t;

uint32_t getSglFreRegData(strick_fre_reg_id_t reg_id);
uint32_t setSinglFreRegData(strick_fre_reg_id_t reg_id, uint32_t value);

#endif /* CALIBRATION_PARAMETER_CONFIG_H */
