/*
 * bat_port.c
 *
 *  Created on: Jan 13, 2021
 *      Author: quangnd
 */

#include "bat_port.h"

static void bat_port_update_soc_temp(Bat_port* p_port);
static BP_data bp[BAT_PORT_NUM];

Bat_port* bat_port_construct(uint8_t id){
	Bat_port* p_port = (Bat_port*)malloc(sizeof(Bat_port));
	p_port->id = id;
	sw_init(&p_port->node_id);
	p_port->bp = bp_construct(p_port->id);
	return p_port;
}

void bat_port_init(Bat_port* p_port, uint8_t id){
	p_port->id = id;
	sw_init(&p_port->node_id);
	p_port->bp = &bp[id];
	p_port->bp->pos = id;
	bp_reset_data(p_port->bp);

}

void bat_port_reset(Bat_port* p_port){
	sw_off(&p_port->node_id);
	bp_reset_data(p_port->bp);
}

void bat_port_set_working_port(Bat_port* p_port){
	p_port->bp->sw_state = BMS_ON_ENTIRE;
	bp_reset_inactive_counter(p_port->bp);
}

uint32_t bat_port_get_drop_vol(Bat_port* p_port){
	if(p_port->bp->cur < 0) return 0;
	p_port->drop_vol = p_port->bp->cur*BP_RESISTANCE_mOhm/1000;
	return p_port->drop_vol;
}

void bat_port_update_distance_available(Bat_port* p_port){
	//bat_port_update_soc_temp(p_port);
	p_port->distance = DISTANCE_SOC_CAL_FACTOR*p_port->bp->soc*1000;
}

static void bat_port_update_soc_temp(Bat_port* p_port){
	if(p_port->soc_temp == 0){
		p_port->soc_temp = (p_port->bp->vol - BP_LOWER_WORKING_VOL_LEVEL_mV)
				*100*1000/BP_WORKING_VOL_LEVEL_RANGE;
	}
	p_port->soc_temp -= p_port->bp->cur*SOC_TEMP_CAL_FACTOR;
}
