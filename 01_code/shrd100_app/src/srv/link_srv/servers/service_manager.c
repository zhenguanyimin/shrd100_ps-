/*
 * UDS_Service.c
 *
 * Created on: 2021-7-10
 *      Author: A19199
 */

#include "../servers/service_manager.h"

/**
 *
 * @param service_id
 * @return
 */
SERVICE_ENTRY *ServiceManager_GetEntry(uint8_t service_msgid)
{
	SERVICE_ENTRY* service_find;
	SERVICE_ENTRY* service_end;

	service_find = (SERVICE_ENTRY*)SERVICE_ENTRY_START;
	service_end = (SERVICE_ENTRY*)SERVICE_ENTRY_END;

	for( ; service_find < service_end; service_find ++ )
	{
		if((service_msgid == service_find->id) && (0x5555AAAA == service_find->index))
		{
			return service_find;
		}
	}

//	SERVICE_ENTRY *service_find;
//    uint8_t sectionindex = 0;
//    for (; sectionindex < (sizeof(ServiceManagerBuf) / sizeof(ServiceManagerBuf[0])); sectionindex++)
//    {
//        service_find = (SERVICE_ENTRY *)ServiceManagerBuf[sectionindex];
//        if (service_msgid == service_find->id)
//        {
//            return service_find;
//        }
//    }

    return (SERVICE_ENTRY *)0;
}
