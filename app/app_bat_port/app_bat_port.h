/*
 * app_bat_port.h
 *
 *  Created on: Jan 13, 2021
 *      Author: quangnd
 */

#ifndef APP_APP_BAT_PORT_APP_BAT_PORT_H_
#define APP_APP_BAT_PORT_APP_BAT_PORT_H_

#include "bs_app.h"

void app_bat_ports_init(pmu_app*);
void app_estimate_init(pmu_app* p_app);
void cal_max_cur_dc_bus(pmu_app* );
void cal_power(Bat_port* port, float delta_t );

#endif /* APP_APP_BAT_PORT_APP_BAT_PORT_H_ */
