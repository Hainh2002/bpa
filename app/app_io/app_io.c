/*
 * app_io.c
 *
 *  Created on: Jan 20, 2024
 *      Author: DELL 5425
 */


#include "app_io.h"

static void		mc_power_sw_on(Switch*);
static void		mc_power_sw_off(Switch*);

void app_io_init(pmu_app* p_app){
	p_app->ioc.mc_power.sw_on = mc_power_sw_on;
	p_app->ioc.mc_power.sw_off = mc_power_sw_off;
}

static void		mc_power_sw_on(Switch* p_sw){
	(void)p_sw;
//	HAL_OUTPUT_SW_SET_HIGH(MC_POWER);
}
static void		mc_power_sw_off(Switch* p_sw){
	(void)p_sw;
//	HAL_OUTPUT_SW_SET_LOW(MC_POWER);
}
