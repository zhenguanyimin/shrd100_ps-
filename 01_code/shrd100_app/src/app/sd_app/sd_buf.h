
#ifndef APP_SD_APP_WRITE_H_
#define APP_SD_APP_WRITE_H_

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "stdint.h"
#include "stdio.h"
#include "xsdps.h"		/* SD device driver */
#include "ff.h"
#include "xtime_l.h"

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/
// the struct headInfo size must <= 512 byte
int32_t LogStoreToBuf(char *buf, uint8_t len);
int32_t DebugStoreToBuf(char *buf, uint8_t len);
int32_t DataStoreToBuf(char *buf, uint16_t len);

void SdBufInit(void);
void CheckInfoBufSaveToSd( FIL *file );
void CheckDataBufSaveToSd( FIL *file );
void CheckDebugBufSaveToSd( FIL *file );

#endif /* APP_SD_APP_WRITE_H_ */
