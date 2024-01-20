/*
 * bp_data.c
 *
 *  Created on: Aug 25, 2021
 *      Author: KhanhDinh
 */

#include "bp_data.h"

#define  WHEEL_CIRUMFERENCE   float(1.5)
#define  WHEEL_CIRUMFERENCE   float(1.5)


BP_data* bp_construct(uint8_t id){
	BP_data* p_bp = (BP_data*)malloc(sizeof(BP_data));
	while(p_bp == NULL);
	p_bp->pos = id;
	bp_reset_data(p_bp);
	return p_bp;
}

void bp_reset_data(BP_data* p_bp){
	co_slave_set_con_state((CO_Slave*)p_bp, CO_SLAVE_CON_ST_DISCONNECT);
	p_bp->vol = 0;
	p_bp->cur = 0;
	p_bp->cur_limit = 0;
	p_bp->soc = 0;
	p_bp->soh = 0;
	p_bp->cycle = 0;
	p_bp->state = BP_ST_INIT;
	p_bp->status = BP_STT_OK;
	p_bp->sw_state = BMS_OFF_ENTIRE;
	p_bp->pre_sw_state = BMS_OFF_ENTIRE;
	for(uint8_t i = 0; i < 16; i++) p_bp->cell_vol[i] = 0;
	for(uint8_t i = 0; i < 8; i++) p_bp->temp[i] = 0;
	for(uint8_t i = 0; i < 32; i++) p_bp->base.vehicle_sn[i] = '0';
	bp_reset_inactive_counter(p_bp);
	bp_reset_recovery_counter(p_bp);
	p_bp->power_discharge = 0;
	p_bp->valid_st = NO_CHECK;
	p_bp->cnt_remove_bp_invalid = 0;

}
/**
 * check check_unbalance_BP: Vcellmax-Vcellmin >200mV -- return true
 */
uint8_t check_unbalance_BP (uint16_t vol_data[] ){
    if ( (get_value_max(vol_data,16) - get_value_min(vol_data,16))  >= 2 ) return 1;
    else return 0;

}
uint32_t get_value_min(uint8_t data[], uint32_t size_data){
	uint8_t min_data = data[0];
	for ( uint32_t i= 1; i < size_data; i++){
		if ( data[i] < min_data ) min_data =  data[i];
	}
	return min_data;
}
uint32_t get_value_max(uint8_t data[], uint32_t size_data){
	uint8_t max_data = data[0];
	for ( uint32_t i= 1; i < size_data; i++){
		if ( data[i] > max_data ) max_data =  data[i];
	}
	return max_data;
}
