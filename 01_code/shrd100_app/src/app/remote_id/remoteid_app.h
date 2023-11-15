#ifndef REMOTEID_APP_H
#define REMOTEID_APP_H

#include "../../hal/hal_remoteid/hal_remoteid.h"




void UartPsMsgProcTask_Post(RemoteID_DataInfo_t* bufaddr);
void RemoteIdHandle_Task();
int remoteid_init();


#endif



