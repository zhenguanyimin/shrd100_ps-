/******************************************************************************
* Copyright (C) 2004 - 2023 AUTEL, Inc.  All rights reserved.
* 
 ******************************************************************************/

/*****************************************************************************/
/**
 *
 * @file xusb_freerots_class_storage.c
 *
 * This file contains the implementation of the Mass Storage specific class
 * code for the example.
 *
 * <pre>
 * MODIFICATION HISTORY:
 *
 * Ver   Who  Date     Changes
 * ----- ---- -------- -------------------------------------------------------
 *
 * </pre>
 *
 *****************************************************************************/

/***************************** Include Files *********************************/
#include "xparameters.h"
#include "FreeRTOS.h"
#include "task.h"
#include "xusb_ch9_cdc_acm.h"
#include "xusb_class_cdc_acm.h"
#include "../../srv/log/log.h"

/************************** Constant Definitions *****************************/

/***************** Macros (Inline Functions) Definitions *********************/

/**************************** Type Definitions *******************************/

/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/
#define LINE_CODEING_SIZE (7)
static uint8_t line_coding[LINE_CODEING_SIZE] = "\x00\xC2\x01\x00\x00\x00\x08";


/*****************************************************************************/
/**
* This function is class handler for Mass storage and is called when
* Setup packet received is for Class request(not Standard Device request)
*
* @param	InstancePtr is pointer to Usb_DevData instance.
* @param	SetupData is pointer to SetupPacket received.
*
* @return	None
*
* @note		None.
*
******************************************************************************/
void ClassReq(struct Usb_DevData *InstancePtr, SetupPacket *SetupData)
{

	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(SetupData   != NULL);

	//xil_printf("ClassReq in\r\n");

	switch (SetupData->bRequest) {
		case CDC_SEND_ENCAPSULATED_COMMAND:
			EpBufferSend(InstancePtr->PrivateData, 0, NULL, 0);
			break;

		case CDC_GET_ENCAPSULATED_RESPONSE:
			
			break;
		
		case CDC_SET_COMM_FEATURE:
			EpBufferSend(InstancePtr->PrivateData, 0, NULL, 0);
			break;
		
		case CDC_GET_COMM_FEATURE:
			EpBufferSend(InstancePtr->PrivateData, 0, NULL, 0);
			break;
		
		case CDC_CLEAR_COMM_FEATURE:
			EpBufferSend(InstancePtr->PrivateData, 0, NULL, 0);
			break;

	  /*******************************************************************************/
	  /* Line Coding Structure                                                       */
	  /*-----------------------------------------------------------------------------*/
	  /* Offset | Field       | Size | Value  | Description                          */
	  /* 0      | dwDTERate   |   4  | Number |Data terminal rate, in bits per second*/
	  /* 4      | bCharFormat |   1  | Number | Stop bits                            */
	  /*                                        0 - 1 Stop bit                       */
	  /*                                        1 - 1.5 Stop bits                    */
	  /*                                        2 - 2 Stop bits                      */
	  /* 5      | bParityType |  1   | Number | Parity                               */
	  /*                                        0 - None                             */
	  /*                                        1 - Odd                              */
	  /*                                        2 - Even                             */
	  /*                                        3 - Mark                             */
	  /*                                        4 - Space                            */
	  /* 6      | bDataBits  |   1   | Number Data bits (5, 6, 7, 8 or 16).          */
	  /*******************************************************************************/
		case CDC_SET_LINE_CODING:
			if (LINE_CODEING_SIZE == SetupData->wLength)
			{
				//xil_printf("set line coding\r\n");
				EpBufferRecv(InstancePtr->PrivateData, 0, (u8 *)line_coding, SetupData->wLength);
				EpBufferSend(InstancePtr->PrivateData, 0, NULL, 0);
			}
			break;
		
		case CDC_GET_LINE_CODING:
			//xil_printf("get line coding\r\n");
			EpBufferSend(InstancePtr->PrivateData, 0, line_coding, LINE_CODEING_SIZE);
			break;
		
		case CDC_SET_CONTROL_LINE_STATE:
			//xil_printf("set line state\r\n");
			EpBufferSend(InstancePtr->PrivateData, 0, NULL, 0);
			break;
		
		case CDC_SEND_BREAK:
			EpBufferSend(InstancePtr->PrivateData, 0, NULL, 0);
			break;

		default:
//			LOG_DEBUG("Unsupported command:%d\r\n", SetupData->bRequest);
			/* Unsupported command. Stall the end point */
			EpSetStall(InstancePtr->PrivateData, 0, USB_EP_DIR_OUT);
			break;
	}
}


