
#ifndef APP_INIT_H
#define APP_INIT_H

#include "../srv/protocol/protocol_if.h"

extern protocol_object_list_detected_t gDetectList[1];
extern protocol_object_list_tracked_t gTrackList[1];
extern protocol_beam_scheduling_t gBeamInfo[1];
extern protocol_cfg_param_t gConfigParmInfo[1];
extern protocol_radar_platfrom_info_t gPlatformInfo[1];
extern protocol_radar_status_t gRadarStatus[1];

void radar_alg_task(void *p);
void app_init(void);

#endif /* APP_INIT_H */
