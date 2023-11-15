

#ifndef BT_UART_H_
#define BT_UART_H_

#include "xscugic.h"
#include "../uartlite/uartlite_drv.h"

void wifi_Setup_Intr_System(XScuGic *GicInstancePtr );
uint32_t wifi_uart_buff_empty();
uint32_t get_wifi_uart_data( uint32_t uPos, uint8_t *buff , uint32_t len );
uint32_t output_wifi_uart_data( uint8_t * buff, uint32_t uSize );
void init_fc41d_uart();
void WifiSendDataFunc(char *data, uint32_t len);

#endif
