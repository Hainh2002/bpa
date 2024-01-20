/*
 * bat_port.h
 *
 *  Created on: Jan 13, 2021
 *      Author: quangnd
 */

#ifndef COMPONENT_BAT_PORT_BAT_PORT_H_
#define COMPONENT_BAT_PORT_BAT_PORT_H_

#include "bp_data.h"
#include "app_config.h"
#include "can_master.h"
#include "switch.h"
typedef struct Bat_port_t Bat_port;

typedef enum BAT_PORT_STATE{
	BAT_PORT_ST_INACTIVE = 0,
	BAT_PORT_ST_DISCONNECT,
	BAT_PORT_ST_CONNECTED,
	BAT_PORT_ST_POWERING
} BAT_PORT_STATE;

struct Bat_port_t{
	BP_data*		bp;
	uint8_t			id;
	Switch			node_id;
	uint16_t 		drop_vol;
	uint32_t		soc_temp;
	uint8_t			distance;
	void			(*set_power_sw)(Bat_port*, BMS_SWICTH_STATE,CM_CO_SERVICE );

};

Bat_port* bat_port_construct(uint8_t id);
void bat_port_init(Bat_port*, uint8_t);
void bat_port_reset(Bat_port* p_port);
uint32_t bat_port_get_drop_vol(Bat_port* p_port);
void bat_port_update_distance_available(Bat_port* p_port);
void bat_port_set_working_port(Bat_port* p_port);

static inline void bat_port_set_power_sw(Bat_port* p_port, BMS_SWICTH_STATE sw_state, CM_CO_SERVICE co_service){
	p_port->set_power_sw(p_port, sw_state, co_service);
}

static inline void bat_port_connect(Bat_port* p_port){
	sw_on(&p_port->node_id);
}

static inline void bat_port_disconnect(Bat_port* p_port){
	sw_off(&p_port->node_id);
}


#endif /* COMPONENT_BAT_PORT_BAT_PORT_H_ */
