/******************************************************************************
* Copyright (C) 2004 - 2023 AUTEL, Inc.  All rights reserved.
* 
 ******************************************************************************/

/*****************************************************************************/
/**
 *
 * @file xusb_cdc_config.h
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

#ifndef XUSB_CDC_CONFIG_H
#define XUSB_CDC_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/


/************************** Constant Definitions *****************************/


/*---------------------------------------------------------------------*/
/*  USB DEVICE CDC FIFO SIZE define                                    */
/*---------------------------------------------------------------------*/
#define USB_DEV_SEND_FIFO_SIZE (1024*1024)
#define USB_DEV_RECV_FIFO_SIZE (64*1024)

/** the cdc device number, 1~2 */
//#define CONFIG_USB_DEC_CDC_NUM (2)
#define CONFIG_USB_DEC_CDC_NUM (1)

#if ((CONFIG_USB_DEC_CDC_NUM > 2) || (CONFIG_USB_DEC_CDC_NUM <= 0))
#error "CONFIG_USB_DEC_CDC_NUM is not 1~2"
#endif


/**************************** Type Definitions ******************************/


/************************** Function Prototypes ******************************/


#ifdef __cplusplus
}
#endif

#endif /* XUSB_CDC_CONFIG_H */

