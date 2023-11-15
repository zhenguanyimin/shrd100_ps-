/*
 * pl_config.h
 *
 *  Created on: 2023年5月8日
 *      Author: A21001
 */

#ifndef SRC_APP_RF_CONFIG_PL_CONFIG_H_
#define SRC_APP_RF_CONFIG_PL_CONFIG_H_

void RF_StopPLAlg(void);
void RF_StartPLAlgDirectly(void);
void RF_StartPLAlgByTimer(void);

int32_t Rf_Pl_Config_Init(void);

#endif /* SRC_APP_RF_CONFIG_PL_CONFIG_H_ */
