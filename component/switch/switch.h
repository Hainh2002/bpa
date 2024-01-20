/*
 * switch.h
 *
 *  Created on: Aug 21, 2020
 *      Author: quangnd
 */

#ifndef COMPONENT_SWITCH_SWITCH_H_
#define COMPONENT_SWITCH_SWITCH_H_

#include "stdbool.h"
#include "stdlib.h"
#include "stdint.h"

#define BLINK_INTERVAL_mS			500UL

typedef enum SW_STATE{
	SW_ST_OFF =0,
	SW_ST_ON =1,
}SW_STATE;

typedef struct Switch_t Switch;

struct Switch_t{
	SW_STATE 	state;
	SW_STATE	(*get_state)(Switch*);
	void 		(*sw_on)(Switch* p_sw);
	void 		(*sw_off)(Switch* p_sw);
	void		(*sw_toggle)(Switch* p_sw);
	bool		is_changed;
	uint32_t	command;
	uint32_t	blink_timestamp;
};

Switch* sw_construct(void);
void 	sw_init(Switch* p_sw);

void sw_update_input_state(Switch*);
void sw_update_output_state(Switch* p_sw);

static inline void sw_on(Switch* p_sw){
	if(p_sw->sw_on == NULL) return;
	p_sw->sw_on(p_sw);
}

static inline void sw_off(Switch* p_sw){
	if(p_sw->sw_off == NULL) return;
	p_sw->sw_off(p_sw);
}

static inline void sw_toggle(Switch* p_sw){
	if(p_sw->sw_toggle == NULL) return;
	p_sw->sw_toggle(p_sw);
}

static inline SW_STATE sw_get_state(Switch* p_sw){
	return p_sw->state;
}

#endif /* COMPONENT_SWITCH_SWITCH_H_ */
