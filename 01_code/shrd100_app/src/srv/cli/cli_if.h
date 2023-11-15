
#ifndef CLI_IF_H
#define CLI_IF_H

#include "FreeRTOS_CLI.h"

typedef struct UsbCommand_msg
{
	uint8_t length; /**/
	uint8_t buf[200];
} sUsbCommand_msg_t;

extern void vRegisterSampleCLICommands(void);

extern void vUARTCommandConsoleStart(uint16_t usStackSize, UBaseType_t uxPriority);

extern void vBasicSocketsCommandInterpreterTask(void *pvParameters);
extern int32_t protocol_tcp_send_data(void *data, uint32_t len);
extern int32_t InitUsbCommandServer_Task();
#endif /* CLI_IF_H */
