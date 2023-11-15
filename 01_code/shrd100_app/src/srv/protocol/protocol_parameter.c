
#include "protocol_parameter.h"

STATIC cfg_param_cb_fp g_cb_fp = NULL;

ret_code_t protocol_send_beam_scheduling(protocol_beam_scheduling_t *beam_scheduling)
{
	ret_code_t ret = RET_OK;

	if (beam_scheduling == NULL)
	{
		ret = RET_INVALID_PARAM;
		goto out;
	}

	ret = protocol_udp_send_data_blocking(beam_scheduling, sizeof(*beam_scheduling));

out:
	LOG_DEBUG("[%s:%d] ret=%d\r\n", __FUNCTION__, __LINE__, ret);
	return ret;
}

ret_code_t protocol_cfg_param_reg_cb(cfg_param_cb_fp cb_fp)
{
	ret_code_t ret = RET_OK;

	g_cb_fp = cb_fp;

	return ret;
}

ret_code_t protocol_cfg_param_process_data(void *data, uint32_t len)
{
	ret_code_t ret = RET_OK;
	protocol_cfg_param_t *cfgMsg = NULL;
	protocol_cfg_param_t cfgParam = {0};

	if ((data == NULL) || (len != sizeof(*cfgMsg)))
	{
		ret = RET_INVALID_PARAM;
		goto out;
	}

	if (g_cb_fp != NULL)
	{
		cfgMsg = (protocol_cfg_param_t *)data;
		// get the data needed from the message
		cfgParam.trSwitchCtrl = cfgMsg->trSwitchCtrl;
		cfgParam.workMode = cfgMsg->workMode;
		cfgParam.workWaveCode = cfgMsg->workWaveCode;
		cfgParam.workFrqCode = cfgMsg->workFrqCode;
		cfgParam.prfPeriod = cfgMsg->prfPeriod;
		cfgParam.accuNum = cfgMsg->accuNum;

		cfgParam.noiseCoef = ntohs(cfgMsg->noiseCoef) >> 7;
		cfgParam.clutterCoef = ntohs(cfgMsg->clutterCoef) >> 7;
		cfgParam.cfarCoef = ntohs(cfgMsg->cfarCoef) >> 7;
		cfgParam.focusRangeMin = ntohs(cfgMsg->focusRangeMin);
		cfgParam.focusRangeMax = ntohs(cfgMsg->focusRangeMax);
		cfgParam.clutterCurveNum = ntohs(cfgMsg->clutterCurveNum);
		cfgParam.lobeCompCoef = ntohs(cfgMsg->lobeCompCoef) >> 7;

		cfgParam.cohesionVelThre = cfgMsg->cohesionVelThre;
		cfgParam.cohesionRgnThre = cfgMsg->cohesionRgnThre;
		cfgParam.clutterMapSwitch = cfgMsg->clutterMapSwitch;
		cfgParam.clutterMapUpdateCoef = cfgMsg->clutterMapUpdateCoef;
		cfgParam.aziCalcSlope = cfgMsg->aziCalcSlope;
		cfgParam.aziCalcPhase = cfgMsg->aziCalcPhase;
		cfgParam.eleCalcSlope = cfgMsg->eleCalcSlope;
		cfgParam.eleCalcPhase = cfgMsg->eleCalcPhase;
		cfgParam.aziScanCenter = cfgMsg->aziScanCenter;
		cfgParam.aziScanScope = cfgMsg->aziScanScope;
		cfgParam.eleScanCenter = cfgMsg->eleScanCenter;
		cfgParam.eleScanScope = cfgMsg->eleScanScope;

		cfgParam.coherentDetectSwitch = ntohs(cfgMsg->coherentDetectSwitch);

		g_cb_fp(&cfgParam);
	}

out:
	return ret;
}
