
#ifndef  __UARTPS_DRV_H
#define  __UARTPS_DRV_H

/* Scheduler includes. */
#include <stdint.h>
#include "xuartps.h"
#include "xscugic.h"
#include "xil_exception.h"
#include "portmacro.h"
//#include "rojdefs.h"

/* The UART interrupts of interest when receiving. */
#define UARTPS_INTERRUPT_MASK (XUARTPS_IXR_RXOVR | XUARTPS_IXR_RXFULL | XUARTPS_IXR_TOUT)

/* The UART interrupts of interest when transmitting. */
#define UARTPS_TRANSMIT_IINTERRUPT_MASK (XUARTPS_IXR_TXEMPTY)

void mcu_Setup_Intr_System();
int get_muc_dir_key_cnt();
#endif


