
#ifndef LOG_H
#define LOG_H
#include "../../inc/common.h"

typedef enum log_level
{
    LL_ANY = 0,
    LL_VERB = 1,
    LL_INFO = 2,
    LL_DEBUG = 3,
    LL_ERROR = 4,
	LL_DATA = 5,
    LL_PRINTF = 6,
    LL_NONE
} log_level_t;

typedef enum log_uart_chl
{
    LOG_PS_UART0 = 0,
    LOG_Pl_UART1 = 1,
    LOG_Pl_UART2 = 2,
	LOG_PS_TCP = 3,
	LOG_PS_USB = 4,
} log_uart_chl_t;

void log_set_lvl(log_level_t lvl);
log_level_t log_get_lvl_num(void);
const char *log_get_lvl_str(void);
void log_record(log_level_t lvl, char *fmt, ...);
void log_dump(uint32_t base_addr, uint8_t *buf, uint32_t len);
uint8_t GetSwitchOutputChl( );
void SetSwitchOutputChl(uint8_t value);

#define LOG_PRINTF(format, args...) log_record(LL_PRINTF, format, ##args)
#ifdef MODULE_NAME
#define LOG_ERROR(format, args...) log_record(LL_ERROR, "%s: "             \
                                                        "[LOG-E] " format, \
                                              MODULE_NAME, ##args)
#define LOG_DEBUG(format, args...) log_record(LL_DEBUG, "%s: "             \
                                                        "[LOG-D] " format, \
                                              MODULE_NAME, ##args)
#define LOG_INFO(format, args...) log_record(LL_INFO, "%s: "             \
                                                      "[LOG-I] " format, \
                                             MODULE_NAME, ##args)
#define LOG_VERBOSE(format, args...) log_record(LL_VERB, "%s: "             \
                                                         "[LOG-V] " format, \
                                                MODULE_NAME, ##args)
#else
#define LOG_ERROR(format, args...) log_record(LL_ERROR, "[LOG-E] " format, ##args)
#define LOG_DEBUG(format, args...) log_record(LL_DEBUG, "[LOG-D] " format, ##args)
#define LOG_INFO(format, args...) log_record(LL_INFO, "[LOG-I] " format, ##args)
#define LOG_VERBOSE(format, args...) log_record(LL_VERB, "[LOG-V] " format, ##args)
#define LOG_TEST(format, args...) log_record(LL_DEBUG, "[LOG-T] " format, ##args)
#endif

#define LOG_TRACE_POINT LOG_DEBUG("[%s:%d]\r\n", __FUNCTION__, __LINE__)

#endif /* LOG_H */
