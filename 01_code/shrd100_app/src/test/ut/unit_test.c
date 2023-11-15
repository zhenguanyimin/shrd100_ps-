#include "unit_test.h"
#include "ut_dataPath_test.h"
#include "ut_sys_ctrl.h"
#include "ut_protocol.h"
#include "ut_reg_set.h"
#include "ut_alg_dbg.h"
#include "ut_led_ctrl.h"
#include "ut_dev_info.h"

void uint_test_init(void)
{
	ut_sys_ctrl_init();

	ut_protocol_init();

	ut_set_reg_init();

	ut_datapath_init();

	ut_led_ctrl_init();

	ut_alg_dbg_init();

	ut_dev_info_init();

	return;
}
