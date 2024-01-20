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

void sw_update_key_input_state(Switch* p_sw){
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

void sw_update_motor_lock_output_state(Switch* p_sw){
	if(!p_sw->is_changed) return;
	if(p_sw->state == SW_ST_OFF) sw_off(p_sw);
	else sw_on(p_sw);
}

void sw_update_blink_state(Switch* p_sw, uint32_t timestamp){
	if(p_sw->state == SW_ST_ON){
		if(p_sw->blink_timestamp > timestamp) return;
		p_sw->is_changed = false;
		sw_toggle(p_sw);
		p_sw->blink_timestamp = timestamp + BLINK_INTERVAL_mS;
		return;
	}
	if(p_sw->is_changed == false) return;
	p_sw->is_changed = false;
	sw_off(p_sw);
	p_sw->blink_timestamp = 0;
}
void sw_update_motor_lock_on_warning(Switch* p_sw){
	sw_on(p_sw);
}
void sw_update_motor_lock_off_warning(Switch* p_sw,uint32_t timestamp){
	if(p_sw->blink_timestamp > timestamp) return ;
	sw_toggle(p_sw);
	p_sw->blink_timestamp = timestamp + 500;
}
void motor_lock_end_of_warning(Switch* p_sw){
	sw_off(p_sw);
}
