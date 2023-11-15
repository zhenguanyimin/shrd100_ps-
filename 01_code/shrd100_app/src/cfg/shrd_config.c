#include "shrd_config.h"
#include "version.h"

typedef struct aeag_cfg_mng
{
	sPL_SignalHandlingCfg plSignalHandling;
} aeag_cfg_mng_t;

static aeag_cfg_mng_t g_aeagCfgMng;
static sPL_SignalHandlingCfg plSignalHandling = {
	.uLocalOscFreq = 1200,
};
char l_sn_code[DEVICE_SN_CODE_LEN] 		= "DRONEID101B112023497001";
char l_company[25] 		= "SKYFEND";
char l_device_name[25] 	= "DRONE ID";
uint16_t l_device_type 	= 0x7;
char l_device_version[10] = "v101";

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

const char *get_embed_alink_version_string(void)
{
	return EMBED_ALINK_VERSION_STR;
}

char set_device_sn_code( char *psncode)
{
	memcpy( l_sn_code , psncode , DEVICE_SN_CODE_LEN );
}
char *get_device_sn_code(void)
{
	return l_sn_code;
}


