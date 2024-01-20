/*
 * switch.c
 *
 *  Created on: Aug 21, 2020
 *      Author: quangnd
 */

#include "switch.h"

Switch* sw_construct(void){
	Switch* p_sw = (Switch*)malloc(sizeof(Switch));
	while(p_sw == NULL);
	p_sw->state = SW_ST_OFF;
	p_sw->is_changed = false;
	p_sw->blink_timestamp = 0;
	return p_sw;
}
void parking_init(Switch* p_sw){
	p_sw->state = SW_ST_ON;
	sw_off(p_sw);
	p_sw->is_changed = false;
	p_sw->blink_timestamp = 0;
}

void sw_init(Switch* p_sw){
	p_sw->state = SW_ST_OFF;
	sw_off(p_sw);
	p_sw->is_changed = false;
	p_sw->blink_timestamp = 0;
}

void sw_update_input_state(Switch* p_sw){
	SW_STATE old_state = p_sw->state;
	SW_STATE new_state = p_sw->get_state(p_sw);

	if(old_state != new_state){
		p_sw->state =new_state;
		p_sw->is_changed = true;
		p_sw->blink_timestamp =0;
	}
}

void sw_update_output_state(Switch* p_sw){
	if(!p_sw->is_changed) return;

	p_sw->is_changed = false;
	if(p_sw->state == SW_ST_OFF) sw_off(p_sw);
	else sw_on(p_sw);
}

