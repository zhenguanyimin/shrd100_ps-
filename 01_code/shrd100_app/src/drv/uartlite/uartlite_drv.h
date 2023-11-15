#include <stdint.h>
#ifndef  __UARTLITE_DRV_H
#define  __UARTLITE_DRV_H

/* Exported common structure --------------------------------------------------------*/
typedef struct
{
    volatile uint32_t RX_FIFO					;//0x00000000
    volatile uint32_t TX_FIFO                 ;//0X00000004
    volatile uint32_t STATUS_REG              ;//0X00000008
    volatile uint32_t CONTROL_REG             ;//0X0000000c
}   UART_TYPEDEF;

typedef enum {
	UART_DISABLE 	 =0x00,
	UART_ENABLE 		 =0x01,
} FUNCTION_STATE;


#define UARTLITE_IRPT_IMU	  XPAR_FABRIC_PL_UART5_INTERRUPT_INTR
#define UARTLITE_IRPT_WIFI	  XPAR_FABRIC_PL_UART0_INTERRUPT_INTR

#define     UARTLITE_IMU                ((UART_TYPEDEF 			*)XPAR_PL_UART5_BASEADDR)
#define     UARTLITE_WIFI                ((UART_TYPEDEF 			*)XPAR_PL_UART0_BASEADDR)
/************************** Constant Definitions ****************************/

/* UART Lite register offsets */

#define XUL_RX_FIFO_OFFSET			0	/* receive FIFO, read only */
#define XUL_TX_FIFO_OFFSET			4	/* transmit FIFO, write only */
#define XUL_STATUS_REG_OFFSET		8	/* status register, read only */
#define XUL_CONTROL_REG_OFFSET		12	/* control reg, write only */

/* Control Register bit positions */

#define XUL_CR_ENABLE_INTR			0x10	/* enable interrupt */
#define XUL_CR_FIFO_RX_RESET		0x02	/* reset receive FIFO */
#define XUL_CR_FIFO_TX_RESET		0x01	/* reset transmit FIFO */

/* Status Register bit positions */

#define XUL_SR_PARITY_ERROR			0x80
#define XUL_SR_FRAMING_ERROR		0x40
#define XUL_SR_OVERRUN_ERROR		0x20
#define XUL_SR_INTR_ENABLED			0x10	/* interrupt enabled */
#define XUL_SR_TX_FIFO_FULL			0x08	/* transmit FIFO full */
#define XUL_SR_TX_FIFO_EMPTY		0x04	/* transmit FIFO empty */
#define XUL_SR_RX_FIFO_FULL			0x02	/* receive FIFO full */
#define XUL_SR_RX_FIFO_VALID_DATA	0x01	/* data in receive FIFO */

/* The following constant specifies the size of the Transmit/Receive FIFOs.
 * The FIFO size is fixed to 16 in the Uartlite IP and the size is not
 * configurable. This constant is not used in the driver.
 */
#define XUL_FIFO_SIZE			16

/* Stop bits are fixed at 1. Baud, parity, and data bits are fixed on a
 * per instance basis
 */
#define XUL_STOP_BITS			1

/* Parity definitions
 */
#define XUL_PARITY_NONE			0
#define XUL_PARITY_ODD			1
#define XUL_PARITY_EVEN			2


/***********************************************************************
* Function
**********************************************************************/

extern void uart_interrupt_control(UART_TYPEDEF* UART_X, uint32_t INT_Type, FUNCTION_STATE INT_State);

extern void uart_tx_fifo_clear(UART_TYPEDEF* UART_X);

extern void uart_rx_fifo_clear(UART_TYPEDEF* UART_X);

extern uint32_t uart_tx_fifo_empty(UART_TYPEDEF* UART_X);

extern uint32_t uart_rx_fifo_no_empty(UART_TYPEDEF* UART_X);

extern uint32_t uart_error_status(UART_TYPEDEF* UART_X,int ErrorType);

extern uint32_t uart_send_byte(UART_TYPEDEF* UART_X,char UART_Byte, uint32_t time_out);

extern void uart_send_byte_noblock(UART_TYPEDEF* UART_X,char UART_Byte);

extern void uart_send_noblock(UART_TYPEDEF* UART_X, char* UART_Data, uint32_t DataLen);

extern void uart_send_block(UART_TYPEDEF* UART_X,char* UART_Data,uint32_t DataLen, uint32_t time_out);

extern char uart_receive_byte(UART_TYPEDEF* UART_X);

extern char uart_receive_byte_noblock(UART_TYPEDEF* UART_X);

extern char uart_receive_no_block(UART_TYPEDEF* UART_X,char* UART_Data,uint32_t DataLen);

extern uint32_t uart_receive_block(UART_TYPEDEF* UART_X,char* UART_Data,uint32_t DataLen,uint32_t Timeout);

extern void uart_send_hex(UART_TYPEDEF* UART_X, uint32_t HexData);


#endif


