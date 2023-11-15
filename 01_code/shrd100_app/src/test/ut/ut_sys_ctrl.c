
#include "ut_sys_ctrl.h"

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* FreeRTOS+CLI includes. */
#include "../../srv/cli/cli_if.h"
#include "../../srv/log/log.h"

static BaseType_t log_lvl_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);

static const CLI_Command_Definition_t log_lvl_command =
    {
        "log_lvl",
        "\r\nlog_lvl (level):\r\n Set or get the log level\r\n",
        log_lvl_command_handler,
        -1};

static BaseType_t log_lvl_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
    BaseType_t len1;
    uint32_t lvl;
    const char *lvl_str = NULL;
    const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

    if (len1 > 0)
    {
        lvl = strtoul(param1, NULL, 0);
        log_set_lvl((log_level_t)lvl);
        snprintf(pcWriteBuffer, xWriteBufferLen, "\r\nOK\r\n");
    }
    else
    {
        lvl = (uint32_t)log_get_lvl_num();
        lvl_str = log_get_lvl_str();
        snprintf(pcWriteBuffer, xWriteBufferLen, "\r\n%lu: %s\r\n", lvl, lvl_str);
    }

    return pdFALSE;
}

void ut_sys_ctrl_init(void)
{
    FreeRTOS_CLIRegisterCommand(&log_lvl_command);

    return;
}
