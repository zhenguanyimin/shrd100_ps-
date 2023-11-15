
#include "app_init.h"
#include <stdio.h>
#include "xgpiops.h"
#include "sleep.h"
#include "xparameters.h"
#include "xil_cache.h"
#include "xil_printf.h"

#include "FreeRTOS.h"
#include "../cfg/shrd_config.h"

extern void rpc_salver_demo(void);

void init_thread(void *p)
{
	xil_printf(" Firmware compile time:%s %s\n", __DATE__, __TIME__);
	xil_printf("version:%s\r\n", EMBED_SOFTWARE_PS_VERSION_STR);

	rpc_salver_demo();

	vTaskDelete(NULL);
	return;
}

void app_init(void)
{
	// Xil_DCacheDisable();

	/* any thread using lwIP should be created using sys_thread_new */
//	(void)xTaskCreate(init_thread, "INIT_THRD", 1024, 0, TASK_PRI_APP_INIT, NULL);
	(void)xTaskCreate(init_thread, "INIT_THRD", 8*1024, 0, TASK_PRI_APP_INIT, NULL);

	vTaskStartScheduler();

	return;
}
