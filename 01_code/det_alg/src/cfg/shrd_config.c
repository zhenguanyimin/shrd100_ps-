#include "shrd_config.h"

typedef struct aeag_cfg_mng
{
	sPL_SignalHandlingCfg plSignalHandling;
} aeag_cfg_mng_t;

static aeag_cfg_mng_t g_aeagCfgMng;
static sPL_SignalHandlingCfg plSignalHandling = {
	.uLocalOscFreq = 1200,
};

uint32_t aeagCfg_GetPlSignalHandlingLocalOscFreq(void)
{
	uint32_t ret_val = 0;

	ret_val = g_aeagCfgMng.plSignalHandling.uLocalOscFreq;

	return ret_val;
}
sPL_SignalHandlingCfg *aeagCfg_GetPlSignalHandlingCfg(void)
{
	return &plSignalHandling;
}
