
#include "xil_types.h"
#include "uartlite_drv.h"

/***********************************************************************
* @brief         UART interrupt enable or disable
*
* @param[in]     UART_X:
*
* @param[in]     INT_Type:
*
* @param[in]     INT_State: ENABLE or DISABLE
*
* @return        None
***********************************************************************/
void uart_interrupt_control(UART_TYPEDEF* UART_X, uint32_t INT_Type, FUNCTION_STATE INT_State)
{
	if(UART_ENABLE == INT_State)
	{
		UART_X->CONTROL_REG |= INT_Type ;
	}
	else
	{
		UART_X->CONTROL_REG &= INT_Type ;
	}
}
/***********************************************************************
* @brief         UART TX FIFO clear
*
* @param[in]     UART_X:
*
* @return        None
***********************************************************************/
void uart_tx_fifo_clear(UART_TYPEDEF* UART_X)
{
	UART_X->CONTROL_REG |= XUL_CR_FIFO_TX_RESET ;
}

/***********************************************************************
* @brief         UART RX FIFO clear
*
* @param[in]     UART_X:
*
* @return        None
***********************************************************************/
void uart_rx_fifo_clear(UART_TYPEDEF* UART_X)
{
	UART_X->CONTROL_REG |= XUL_CR_FIFO_RX_RESET ;
}

/***********************************************************************
* @brief         UART TX FIFO full state
*
* @param[in]     UART_X:
*
* @return        1: TX FIFO full
* 				 0: TX FIFO not full
***********************************************************************/
uint32_t uart_tx_fifo_full(UART_TYPEDEF* UART_X)
{
	if( UART_X->STATUS_REG & XUL_SR_TX_FIFO_FULL )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/***********************************************************************
* @brief         UART TX logic empty state
*
* @param[in]     UART_X:
*
* @return        0: Both the TX FIFO and TX shift register are empty
* 				 1: NOT both the TX FIFO and TX shift register are empty
***********************************************************************/
uint32_t uart_tx_logic_empty(UART_TYPEDEF* UART_X)
{
	if( UART_X->STATUS_REG & XUL_SR_TX_FIFO_EMPTY )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}

}

/***********************************************************************
* @brief         UART RX FIFO not full state
*
* @param[in]     UART_X:
*
* @return        1: RX FIFO full
* 				 0: RX FIFO not full
***********************************************************************/
uint32_t uart_rx_fifo_full(UART_TYPEDEF* UART_X)
{
	if( UART_X->STATUS_REG & XUL_SR_RX_FIFO_FULL )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/***********************************************************************
* @brief         UART RX FIFO not empty state
*
* @param[in]     UART_X:
*
* @return        0: RX FIFO empty
* 				 1: RX FIFO not empty
***********************************************************************/
uint32_t uart_rx_fifo_no_empty(UART_TYPEDEF* UART_X)
{
	if( UART_X->STATUS_REG & XUL_SR_RX_FIFO_VALID_DATA )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
uint32_t uart_rx_fifo_empty(UART_TYPEDEF* UART_X)
{
	if( UART_X->STATUS_REG & XUL_SR_RX_FIFO_VALID_DATA )
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}
/***********************************************************************
* @brief         UART get error status
*
* @param[in]     UART_X: UART0_REGDEF-UART7_REGDEF
*
* @param[in]     ErrorType:
* 				 #define XUL_SR_PARITY_ERROR		0x80
*  				 #define XUL_SR_FRAMING_ERROR		0x40
*  				 #define XUL_SR_OVERRUN_ERROR		0x20
*
* @return        1: error
* 				 0: no error
***********************************************************************/
uint32_t uart_error_status(UART_TYPEDEF* UART_X,int ErrorType)
{
	if( UART_X->STATUS_REG & ErrorType )
	{
		return TRUE;
	}

	return FALSE;
}

/***********************************************************************
* @brief         UART send byte
*
* @param[in]     UART_X:
*
* @return        None
***********************************************************************/
uint32_t uart_send_byte(UART_TYPEDEF* UART_X,char UART_Byte, uint32_t time_out)
{
	while( TRUE == uart_tx_fifo_full(UART_X))
	{
		time_out--;

		if (time_out <=0 )
		{
			return FALSE;
		}
	}

	UART_X->TX_FIFO = UART_Byte ;

	return TRUE;
}

/***********************************************************************
* @brief         UART send byte
*
* @param[in]     UART_X: UART0_REGDEF-UART7_REGDEF
*
* @return        None
***********************************************************************/
void uart_send_byte_noblock(UART_TYPEDEF* UART_X,char UART_Byte)
{
	UART_X->TX_FIFO = UART_Byte ;
}

/***********************************************************************
* @brief         UART send noblock
*
* @param[in]     UART_X: UART0_REGDEF-UART7_REGDEF
*
* @return        None
***********************************************************************/
void uart_send_noblock(UART_TYPEDEF* UART_X, char* UART_Data, uint32_t DataLen)
{
	uint32_t index;

	for(index = 0;index < DataLen;index++)
	{
		uart_send_byte_noblock(UART_X,*UART_Data);
		UART_Data++;
	}
}

/***********************************************************************
* @brief         UART send byte
*
* @param[in]     UART_X: UART0_REGDEF-UART7_REGDEF
*
* @param[in]     UART_Data: pointer to send data
*
* @param[in]     DataLen: send data length
*
* @return        None
***********************************************************************/
void uart_send_block(UART_TYPEDEF* UART_X,char* UART_Data,uint32_t DataLen, uint32_t time_out)
{
	uint32_t index;

	for(index = 0;index < DataLen;index++)
	{
		uart_send_byte(UART_X,*UART_Data, time_out);
		UART_Data++;
	}
}

/***********************************************************************
* @brief         UART receive byte
*
* @param[in]     UART_X: UART0_REGDEF-UART7_REGDEF
*
* @return        receive byte
***********************************************************************/
char uart_receive_byte(UART_TYPEDEF* UART_X)
{
	int32_t TimeoutValue = 10000;

	while( TRUE == uart_rx_fifo_empty(UART_X))
	{
		TimeoutValue--;

		if (TimeoutValue <=0 )
		{
			return FALSE;
		}
	}

	return (char)( UART_X->RX_FIFO & 0xFF );

	return TRUE;
}

/***********************************************************************
* @brief         UART receive byte no block
*
* @param[in]     UART_X: UART0_REGDEF-UART7_REGDEF
*
* @return        receive byte
***********************************************************************/
char uart_receive_byte_noblock(UART_TYPEDEF* UART_X)
{
	return UART_X->RX_FIFO;
}

/***********************************************************************
* @brief         UART receive data
*
* @param[in]     UART_X: UART0_REGDEF-UART7_REGDEF
*
* @param[in]     UART_Data: pointer to receive data
*
* @param[in]     DataLen: receive data length
*
* @return        None
***********************************************************************/
char uart_receive_no_block(UART_TYPEDEF* UART_X,char* UART_Data,uint32_t DataLen)
{
	uint32_t index;

	if(NULL == UART_Data)
	{
		return FALSE;
	}

	for(index = 0;index < DataLen;index++)
	{
		*UART_Data = (char)( UART_X->RX_FIFO & 0xFF );
		UART_Data++;
	}

	return TRUE;
}

/***********************************************************************
* @brief         UART receive data with time out
*
* @param[in]     UART_X:
*
* @param[in]     UART_Data: pointer to receive data
*
* @param[in]     DataLen: receive data length
*
* @param[in]     Timeout: receive data timeout
*
* @return        real receive data count
***********************************************************************/
uint32_t uart_receive_block(UART_TYPEDEF* UART_X,char* UART_Data,uint32_t DataLen,uint32_t Timeout)
{
	uint32_t index;
	uint32_t TimeoutValue = Timeout;
	if (NULL == UART_Data)
	{
		return 0;
	}
	for (index = 0;index < DataLen;index++)
	{
		while (1 == uart_rx_fifo_empty(UART_X))
		{
			TimeoutValue--;

			if (TimeoutValue <=0 )
			{
				return index;
			}
		}

		TimeoutValue = Timeout;

		*UART_Data = (char)( UART_X->RX_FIFO & 0xFF );

		UART_Data++;
	}

	return index;
}

/***********************************************************************
* @brief         UART send int data by HEX format
*
* @param[in]     UART_X: UART0_REGDEF-UART7_REGDEF
*
* @param[in]     HexData: int data to be send
*
* @return        None
***********************************************************************/
void uart_send_hex(UART_TYPEDEF* UART_X, uint32_t HexData)
{
    int i = 0;

    unsigned char ByteData;

    uart_send_byte(UART_X, '0',10000);
    uart_send_byte(UART_X, 'x',10000);

    for(i = 7; i >= 0; i--)
    {
    	ByteData = (unsigned char)((HexData >> (i * 4)) & 0x0F);

        if (ByteData >= 0x0A)
        {
        	uart_send_byte(UART_X, (ByteData + ('A' - 0x0A)),10000);
        }
        else
        {
        	uart_send_byte(UART_X, ByteData + '0',10000);
        }
    }
}
