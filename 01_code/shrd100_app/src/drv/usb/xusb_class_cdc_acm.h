/******************************************************************************
* Copyright (C) 2004 - 2023 AUTEL, Inc.  All rights reserved.
* 
 ******************************************************************************/

/*****************************************************************************/
/**
 *
 * @file xusb_class_acm.h
 *
 * This file contains definitions used in the CDC ACM class code.
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

#ifndef XUSB_CLASS_CDC_ACM_H
#define XUSB_CLASS_CDC_ACM_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/
#include "xil_types.h"
#include "xusb_ch9.h"
#include "task.h"
#include "semphr.h"

/************************** Constant Definitions *****************************/


/*---------------------------------------------------------------------*/
/*  CDC definitions                                                    */
/*---------------------------------------------------------------------*/
#define CDC_SEND_ENCAPSULATED_COMMAND               0x00U
#define CDC_GET_ENCAPSULATED_RESPONSE               0x01U
#define CDC_SET_COMM_FEATURE                        0x02U
#define CDC_GET_COMM_FEATURE                        0x03U
#define CDC_CLEAR_COMM_FEATURE                      0x04U
#define CDC_SET_LINE_CODING                         0x20U
#define CDC_GET_LINE_CODING                         0x21U
#define CDC_SET_CONTROL_LINE_STATE                  0x22U
#define CDC_SEND_BREAK                              0x23U


/**************************** Type Definitions ******************************/


/************************** Function Prototypes ******************************/
void ClassReq(struct Usb_DevData *InstancePtr, SetupPacket *SetupData);


#ifdef __cplusplus
}
#endif

#endif /* XUSB_CLASS_STORAGE_H */
