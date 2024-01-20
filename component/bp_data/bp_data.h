/*
 * bp_data.h
 *
 *  Created on: Aug 25, 2021
 *      Author: KhanhDinh
 */

#ifndef COMPONENT_BP_DATA_BP_DATA_H_
#define COMPONENT_BP_DATA_BP_DATA_H_

#include "stdint.h"
#include "stdlib.h"
//#include "string_util.h"
#include "CO.h"

typedef struct BP_data_t BP_data;


typedef enum BP_STATE{
	BP_ST_INIT=0,
	BP_ST_IDLE,
	BP_ST_SOFTSTART,
	BP_ST_DISCHARGING,
	BP_ST_CHARGING,
	BP_ST_FAULT,
	BP_ST_SHIPMODE,
	BP_ST_SYSTEM_BOOST_UP,
	BP_ST_ID_ASSIGN_START,
	BP_ST_ID_ASSIGN_WAIT_CONFIRM,
	BP_ST_ID_ASSIGN_CONFIRMED,
	BP_ST_ID_ASSIGN_WAIT_SLAVE_SELECT,
	BP_ST_START_AUTHENTICATE,
	BP_ST_AUTHENTICATING,
	BP_ST_STANDBY
} BP_STATE;

typedef enum BP_STATUS{
	BP_STT_OK		= 0,
	BP_STT_RSVD_0	= 1,
	BP_STT_CUV 		= 4,
	BP_STT_COV 		= 8,
	BP_STT_OCC 		= 16,
	BP_STT_OCD1 	= 32,
	BP_STT_OCD2		= 64,
	BP_STT_SCD 		= 128
}BP_STATUS;

typedef enum BMS_SWICTH_STATE{
	BMS_OFF_ENTIRE = 0,
	BMS_ON_ENTIRE = 3,
	BMS_OFF_CHARGE = 4,
	BMS_ON_SOFTSTART = 5,
} BMS_SWICTH_STATE;
typedef enum VERIFY_VALID_BP_STATE{
	NO_CHECK = 0,
	VALID,
	INVALID
} VERIFY_VALID_BP_STATE;
struct BP_data_t{
    CO_Slave 	base;
    BP_STATE	state;
    BMS_SWICTH_STATE 	sw_state;
    BMS_SWICTH_STATE 	pre_sw_state;
    uint16_t 	sw_state_check_cnt;
	uint8_t 	pos;
	uint8_t 	soc;
	uint16_t 	soc_x10;
	uint8_t 	soh;
	uint32_t 	vol;
	uint32_t 	cur_limit;
	int32_t 	cur;
	uint32_t 	cycle;
	uint16_t 	cell_vol[16];
	uint8_t 	temp[6];
	BP_STATUS 	status;
	uint8_t 	fet_status;
	uint16_t 	cnt_remove_bp_invalid;
	float 		power_discharge;// Wh
	VERIFY_VALID_BP_STATE			valid_st;


};

BP_data* bp_construct(uint8_t);
void bp_reset_data(BP_data* p_bp);
uint8_t check_unbalance_BP (uint16_t vol_data[] );
uint32_t get_value_min(uint8_t data[], uint32_t size_data);
uint32_t get_value_max(uint8_t data[], uint32_t size_data);

static inline void bp_set_main_sw_index(BP_data* p_bp, BMS_SWICTH_STATE sw_state){
	if(p_bp == NULL) return;
	p_bp->sw_state = sw_state;
}
static inline void bp_set_main_pre_sw_index(BP_data* p_bp, BMS_SWICTH_STATE sw_state){
	p_bp->pre_sw_state = sw_state;
}

static inline BMS_SWICTH_STATE bp_get_main_sw_index(BP_data* p_bp){
	return p_bp->sw_state;
}

static inline CO_SLAVE_NET_STATE bp_get_con_state(const BP_data* const p_bp){
        return p_bp->base.con_state;
}

static inline void bp_set_con_state(BP_data* p_bp,const CO_SLAVE_NET_STATE state){
        co_slave_set_con_state((CO_Slave*) p_bp, state);
}

static inline void bp_reset_inactive_counter(BP_data* p_bp){
        p_bp->base.inactive_time_ms = 0;
}
static inline void bp_reset_recovery_counter(BP_data* p_bp){
	p_bp->base.recovery_time_ms = 10;
}

static inline BP_STATE	bp_get_state(BP_data* p_bp){
	return p_bp->state;
}

static inline uint32_t bp_get_voltage(BP_data* p_bp){
	return p_bp->vol;
}

static inline void bp_set_sw_state(BP_data* p_bp, BMS_SWICTH_STATE sw_state){
	p_bp->sw_state = sw_state;
}

#endif /* COMPONENT_BP_DATA_BP_DATA_H_ */
