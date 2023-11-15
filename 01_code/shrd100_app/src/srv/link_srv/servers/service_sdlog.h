#ifndef SERVICE_SETHITFREQ_H
#define SERVICE_SETHITFREQ_H

#include <stdint.h>
#include "../../alink/alink_msg_proc.h"
#include "service_manager.h"

uint16_t Service_SetHitFreq_fun(alink_msg_t *request_msg, alink_msg_t *response_msg);

#endif
