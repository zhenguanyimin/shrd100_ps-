/*
 * UDS_Server.c
 *
 * Created on: 2022-9-20
 *      Author: A19199
 */

#include "FreeRTOS.h"
#include "task.h"

#include "link_srv.h"
#include "../../drv/axiuart/sys_intr.h"
#include "xuartlite.h"
#include "FreeRTOSConfig.h"

#include "../../inc/common.h"

/**
 *
 */
void Alink_Server_Process(alink_msg_t *request_msg, alink_msg_t *response_msg)
{
    uint16_t length;
    SERVICE_ENTRY *service_entity;

    length = request_msg->msg_head.len_lo + (request_msg->msg_head.len_hi << 8);
    if (length == 0)
    {   // empty
        //        return;
    }

    service_entity = ServiceManager_GetEntry(request_msg->msg_head.msgid);

    if ((service_entity != NULL) && (service_entity->entry != NULL))
    {
        length = service_entity->entry(request_msg, response_msg);
    }

    if ((request_msg->msg_head.ans == 1) && (length))
    {
        response_msg->msg_head.magic = request_msg->msg_head.magic;
        response_msg->msg_head.len_lo = length & 0xFF;
        response_msg->msg_head.len_hi = (length & 0xFF00) >> 8;
        response_msg->msg_head.destid = request_msg->msg_head.sourceid;
        response_msg->msg_head.sourceid = request_msg->msg_head.destid;
        response_msg->msg_head.msgid = request_msg->msg_head.msgid;
        response_msg->msg_head.ans = 0;
    }
    else
    {
        response_msg->msg_head.len_hi = 0;
        response_msg->msg_head.len_lo = 0;
    }

    return;
}

int32_t WatchAdp_Init()
{
    int32_t ret = RET_OK;

    ret = Axi_UartLite_Init(WATCH_UARTLITE_NUM, (XUartLite_Handler)AlinkMsg_UartLiteRecvHandler);
    if (ret != RET_OK)
    {
        ret = RET_SYSTEM_ERR;
        return ret;
    }
    UartLite_RevData(WATCH_UARTLITE_NUM, Get_RevBuffPoint(), 1024);

    return ret;
}
