/*
 * service_setGotoBootload.h
 *
 *  Created on: 2023年2月16日
 *      Author: A22745
 */

#ifndef SRC_SRV_LINK_SRV_SERVERS_SERVICE_SETGOTOBOOTLOAD_H_
#define SRC_SRV_LINK_SRV_SERVERS_SERVICE_SETGOTOBOOTLOAD_H_

#include "../../../cfg/flash_mmap.h"
#include "service_manager.h"
#include "xplatform_info.h"
#include "xil_io.h"

typedef struct image_verification_parameter
{
    uint32_t image_length; 	// Image length, byte
    uint32_t image_crc; 	// < crc check of message body
    uint8_t isUpgrade;		// 1; upgrade, 0:false
} image_verification_parameter_t;

#endif /* SRC_SRV_LINK_SRV_SERVERS_SERVICE_SETGOTOBOOTLOAD_H_ */
