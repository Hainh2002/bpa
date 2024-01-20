/*
 * bs_app.c
 *
 *  Created on: Jan 13, 2021
 *      Author: quangnd
 */

#include "bs_app.h"
#include "app_bat_port.h"
#include "app_co.h"

/* Use for check_current_condition when process BP_Switch */
static uint32_t	check_current_switch_time = 0;
static uint32_t	wait_for_stable_charging = 0;

bool is_current_enable_switch;
static inline bool 	bs_app_check_current_enable_switch(pmu_app* );
static inline void bs_app_check_enable_Charge_mode(pmu_app* p_app);
static inline void bs_app_check_enable_Discharge_mode(pmu_app* p_app);
static void sdo_cmd_mc_wrrite_anti_theft(uint8_t cmd);

static inline int32_t bs_app_cal_cur_bp_sum(pmu_app* p_app);
static inline int32_t bs_app_calculate_current_DCbus(pmu_app* p_app);

static void pmu_app_write_active_anthi_theft			(pmu_app* p_app);
static void pmu_app_write_inactive_anthi_theft			(pmu_app* p_app);
static void pmu_app_run_warning_anti_theft				(pmu_app* p_app,uint64_t timestamp);
static void pmu_app_stop_warning_anti_theft				(pmu_app* p_app,uint64_t timestamp);

uint32_t 		te =0;
uint32_t 		cnt_out_charger_mode =0;
uint32_t 		cnt_in_charger_mode =0;
uint32_t 		cnt_in_idle_st =0;
uint32_t 		cnt_sw_st_fault =0;

/* Use for process BP_Switch */
/* BP Switch Parameters */
#define BP_SW_VOL_DIFF_LOWER_DISCHARGE_mV						2000
#define BP_SW_VOL_DIFF_UPPER_DISCHARGE_mV						9000
#define BP_SW_VOL_DIFF_CHARGE_mV								300
#define CUR_ENABLE_BP_SW_THRESHOLD_mA							5000
#define CUR_H_LOAD_ENABLE_BP_SW_THRESHOLD_mA					18000
#define CUR_ENABLE_BP_SW_TIME_mS								5000
#define PRE_TO_MAIN_SW_INTERVAL_mS								1000
#define MAIN_TO_FINISH_INTERVAL_mS								1000
#define CUR_CUTOFF_THRESHOLD_mA									-500
#define CUR_ON_THRESHOLD_mA										700
#define CUR_BP_SUM_IN_CHARGING_MODE								-10 //mA
#define CUR_BP_SUM_OUT_CHARGING_MODE							-200 //mA

static uint16_t sw_delay_cnt = 0; 				/* Delay between 2 Switch_State */
/* BP Merge Parameter */
#define CNT_100														100
#define UPPER_MERGE_VOL_DIFF_DISCHARGE_mV							2000
#define LOWER_MERGE_VOL_DIFF_DISCHARGE_mV							(-UPPER_MERGE_VOL_DIFF_DISCHARGE_mV)
#define UPPER_MERGE_VOL_DIFF_DISCHARGE_H_LOAD_mV							100
#define LOWER_MERGE_VOL_DIFF_DISCHARGE_H_LOAD_mV							(-UPPER_MERGE_VOL_DIFF_DISCHARGE_mV)
#define UPPER_MERGE_VOL_DIFF_CHARGE_mV								300
#define LOWER_MERGE_VOL_DIFF_CHARGE_mV								(-UPPER_MERGE_VOL_DIFF_CHARGE_mV)
static inline 	bool is_pre_sw_state_completed(void);
static void 	turn_off_charge_fet_working_port(pmu_app*, const uint64_t);
static void 	turn_on_entire_fet_next_working_port(pmu_app*, const uint64_t);
static void 	turn_off_entire_fet_working_port(pmu_app*, const uint64_t);
static void 	merge_next_working_port(pmu_app* , const uint64_t);
static void 	turn_on_charge_fet_working_port(pmu_app* , const uint64_t);
static void 	softstart_fet_working_port(pmu_app* , const uint64_t);
static uint8_t 	bs_app_get_working_port_id(pmu_app* , const uint8_t );
static Bat_port* 	bs_app_get_act_working_port(pmu_app* );
static void 	bs_app_shift_working_port(pmu_app* );
static uint8_t  bs_app_get_number_of_bp_in_vehicle(pmu_app*);
static void 	bs_app_package_bp_sn(pmu_app* );
static bool 	bs_app_verify_power_sys_valid(pmu_app* );
static void 	bs_app_buffer_bp_state_valid(pmu_app* );
static void 	update_device_infor(pmu_app* );
int32_t curr;

/* Use for send Serial Number of BPs to HMI */
static uint8_t id = 0;
static uint8_t sn_part = 0;

/* ---------------------------------------------------------------------------------- */
void hardware_init(void){
	board_init();
}
void app_init(pmu_app* p_app){
	app_bat_ports_init(p_app);
	app_co_master_init(p_app);
	app_co_init();

}

void pmu_app_update_bp_oparetion_mode(pmu_app* p_app){
	p_app->mode = BP_DISCHARGE;
	bs_app_update_bp_switch_state(p_app);
}

bool string_cmp(char const str1[], char const str2[], uint32_t lenght){
	for(uint32_t i = 0; i < lenght; i++){
		if(str1[i] != str2[i]) return  0;
	}
	return 1;
}

void pmu_app_check_connected_port_state(pmu_app* p_app){
	/* Check inactive time of BP*/
	for(uint8_t i = 0; i < p_app->bat_port_num; i++){
		if(p_app->ports[i].bp->base.con_state != CO_SLAVE_CON_ST_CONNECTED) continue;
		/* Inc inative_counter_time 10-div of every ports, reset whenever received sync_msg from BP */
		p_app->ports[i].bp->base.inactive_time_ms += sys_tick_ms;

		if(p_app->ports[i].bp->base.inactive_time_ms < BP_INIT_INACTIVE_TIMEOUT_mS) continue;
		if((p_app->ports[i].bp->base.inactive_time_ms < BP_INACTIVE_TIMEOUT_mS)&&
				((p_app->ports[i].bp->state == BP_ST_DISCHARGING)||
						(p_app->ports[i].bp->state == BP_ST_STANDBY)) ) continue;

		if(p_app->ports[i].bp->state == BP_ST_DISCHARGING){
			if(p_app->working_port[1] == NULL && p_app->working_port[2] == NULL ){
				sys_reset(p_app);
				bs_app_remove_working_port(p_app, i);
				bs_app_reset_to_idle_state(p_app);
				return;
			}
			bs_app_remove_working_port(p_app, i);
			continue;
		}
		else if(&p_app->ports[i] == p_app->next_working_port) p_app->next_working_port = NULL;
		bs_app_remove_working_port(p_app, i);
		p_app->ports[i].bp->base.recovery_time_ms = 1;
	}

	for(uint8_t i = 0; i < p_app->bat_port_num; i++){
		if(p_app->ports[i].bp->state == BP_ST_DISCHARGING
				&& p_app->mode != BP_CHARGE
				&& p_app->ports[i].bp->valid_st == INVALID ){

			bs_app_remove_working_port(p_app, i);
		}
	}
	/* Verify port operation
	 *
	 * If there are no active ports to discharge, then set mode to IDLE */

	if((p_app->ports[0].bp->sw_state == BMS_ON_ENTIRE)
			||(p_app->ports[1].bp->sw_state == BMS_ON_ENTIRE)
			||(p_app->ports[2].bp->sw_state == BMS_ON_ENTIRE)
			||(p_app->ports[0].bp->sw_state == BMS_OFF_CHARGE)
			||(p_app->ports[1].bp->sw_state == BMS_OFF_CHARGE)
			||(p_app->ports[2].bp->sw_state == BMS_OFF_CHARGE)
			||(p_app->ports[0].bp->state == BP_ST_SYSTEM_BOOST_UP)
			||(p_app->ports[1].bp->state == BP_ST_SYSTEM_BOOST_UP)
			||(p_app->ports[2].bp->state == BP_ST_SYSTEM_BOOST_UP)
			||(p_app->ports[0].bp->state == BP_ST_SOFTSTART)
			||(p_app->ports[1].bp->state == BP_ST_SOFTSTART)
			||(p_app->ports[2].bp->state == BP_ST_SOFTSTART)
			||(p_app->is_power_sys_block == true)){

		cnt_in_idle_st = 0;
	}
	else{
		cnt_in_idle_st ++;
		if( cnt_in_idle_st > 1500){
			sys_reset(p_app);
			bs_app_reset_to_idle_state(p_app);
		}
	}
	/* Check when BP is protected then go back to standby, set sw_state = BMS_ON_ENTIRE */
	for( uint8_t i= 0; i < 3 ; i++){
		if((	p_app->ports[i].bp->state == BP_ST_DISCHARGING)
				|| p_app->ports[i].bp->state == BP_ST_CHARGING){

			p_app->ports[i].bp->sw_state_check_cnt = 0 ;
		}
		else if((p_app->ports[i].bp->state == BP_ST_STANDBY)
				&&(p_app->ports[i].bp->sw_state == BMS_ON_ENTIRE)){
			if( p_app->ports[i].bp->sw_state_check_cnt ++ > 600 ){
				p_app->ports[i].bp->sw_state = BMS_OFF_ENTIRE;
				p_app->ports[i].bp->sw_state_check_cnt = 0;
				p_app->working_port_num--;
			}
		}
		if(p_app->ports[i].bp->state == BP_ST_FAULT){
			bs_app_remove_working_port(p_app, i);
		}
	}
}

void bs_app_reset_to_idle_state(pmu_app* p_app){
	p_app->state = PMU_ST_IDLE;
	app_bat_ports_init(p_app);
	app_co_master_init(p_app);
	can_master_reconfig_node_id_num(&p_app->base);
}
void bs_app_set_sleep_state(pmu_app* p_app){
	p_app->state = PMU_ST_SLEEP;
	app_bat_ports_init(p_app);
	app_estimate_init(p_app);
	app_co_init1(p_app);
	can_master_reconfig_node_id_num(&p_app->base);
}


void bs_app_update_distance_available(pmu_app* p_app, const uint32_t timestamp){
	if(update_distance_timestamp >= timestamp) return;

	uint8_t total_distance = 0;
	for(uint8_t i = 0; i < p_app->bat_port_num; i++){
		if(p_app->ports[i].bp->base.con_state != CO_SLAVE_CON_ST_CONNECTED) continue;
		bat_port_update_distance_available(&p_app->ports[i]);
		total_distance += p_app->ports[i].distance;
	}
	p_app->distance_available = total_distance;
	update_distance_timestamp = timestamp + UPDATE_DISTANCE_TIME_mS;
	p_app->is_ready_stream_distance = true;
}

/* Send BP Serial Number to HMI, per SN is divided 2 parts: 8-bytes upper and 8-bytes lower
 	   /Request read BP SN 	- Address: 0x90
   	   /8 bytes lower 		- Address: 0x200 + node_id;
   	   /8 bytes upper 		- Address: 0x300 + node_id;
 * Send BP SN part per 10ms (1tick) */
void bs_app_stream_bp_sn(pmu_app* p_app){
	bs_app_package_bp_sn(p_app);
}

static void bs_app_package_bp_sn(pmu_app* p_app){
	if(sn_part == 0){
		sn_part++;
		CO_memcpy(p_app->base.p_hw->tx_data_pdo, p_app->ports[id].bp->base.sn, 8);

	}
	else{
		sn_part--;
		CO_memcpy(p_app->base.p_hw->tx_data, p_app->ports[id].bp->base.sn + 8, 8);

		id++;
		if(id == p_app->bat_port_num){
			p_app->is_read_sn_request = false;
			id = 0;
		}
	}
	p_app->base.p_hw->can_tx.DLC = 8;
}

void bs_app_stream_error_code(pmu_app* p_app){
	//Package message
	can_send(p_app->base.p_hw, p_app->base.p_hw->tx_data);
	p_app->is_bp_disconnect = 0;
}

/* ---------------------------------------------------------------------------------- */

void bs_app_update_bp_switch_state(pmu_app* p_app){
#if 1
	if(p_app->sw_is_change == true){
		pmu_switch_update_timeout(p_app, sys_timestamp);
		p_app->sw_is_change = false;
	}
#endif
	switch(p_app->sw_type){
	case BS_NO_SWITCH:
		if ( p_app->mode == BP_DISCHARGE){
			bs_app_update_next_working_port_discharge(p_app);
			bs_app_update_switch_type_discharge(p_app);
			bs_app_reboot_power_sys(p_app);
		}
		else if ( p_app->mode == BP_CHARGE){
			bs_app_update_next_working_port_charge(p_app);
			bs_app_update_switch_type_charge(p_app);
			bs_app_reboot_power_sys(p_app);
		}
		break;
	case BS_PRE_SWICTH:
		if( pmu_sw_is_timeout(p_app, sys_timestamp) ){
			bs_app_set_sw_process_state(p_app, BS_NO_SWITCH);
			break;
		}
		bs_app_process_bp_sw(turn_off_charge_fet_working_port);
		break;
	case BS_MAIN_SWITCH:
		if( pmu_sw_is_timeout(p_app, sys_timestamp) ){
			bs_app_set_sw_process_state(p_app, BS_NO_SWITCH);
			break;
		}
		bs_app_process_bp_sw(turn_on_entire_fet_next_working_port);
		break;
	case BS_FINISH_SWITCH:
		if( pmu_sw_is_timeout(p_app, sys_timestamp) ){
			bs_app_set_sw_process_state(p_app, BS_NO_SWITCH);
			break;
		}
		bs_app_process_bp_sw(turn_off_entire_fet_working_port);
		break;
	case BS_MERGE_SWITCH:
		if( pmu_sw_is_timeout(p_app, sys_timestamp) ){
			bs_app_set_sw_process_state(p_app, BS_NO_SWITCH);
			break;
		}
		bs_app_process_bp_sw(merge_next_working_port);
		break;
	case BS_RECOVERY_SWITCH:
		if( pmu_sw_is_timeout(p_app, sys_timestamp) ){
			bs_app_set_sw_process_state(p_app, BS_NO_SWITCH);
			break;
		}
		bs_app_process_bp_sw(turn_on_charge_fet_working_port);
		break;
	case BS_SOFTSTART_SWITCH:
		if( pmu_sw_is_timeout(p_app, sys_timestamp) ){
			bs_app_set_sw_process_state(p_app, BS_NO_SWITCH);
			break;
		}
		bs_app_process_bp_sw(softstart_fet_working_port);
		break;
	}
}
void pmu_switch_update_timeout(pmu_app* p_app, uint64_t timestamp){
	uint64_t timeout;
	switch( p_app->sw_type ){
	case BS_NO_SWITCH:
		timeout = 0;
		break;
	case BS_PRE_SWICTH:
		timeout = 20000;
		break;

	case BS_MAIN_SWITCH:
		timeout = 20000;
		break;

	case BS_FINISH_SWITCH:
		timeout = 20000;
		break;

	case BS_MERGE_SWITCH:
		timeout = 20000;
		break;

	case BS_RECOVERY_SWITCH:
		timeout = 20000;
		break;
	case BS_SOFTSTART_SWITCH:
		timeout = 20000;
		break;
	default:
		timeout = 0;
		break;
	}
	p_app->sw_timeout = timestamp + timeout;
}

/* ON next_working_port, merge power with current working_port */
static void merge_next_working_port(pmu_app* p_app, const uint64_t timestamp){
	/* Wait for Sync_BP_data process stop */
	(void)timestamp;
	if(p_app->next_working_port == NULL){
		bs_app_set_sw_process_state(p_app, BS_NO_SWITCH);
		CO_SDO_reset_status(&p_app->base.co_app.sdo_client);
		return;
	}
	bat_port_set_power_sw(p_app->next_working_port, BMS_ON_ENTIRE, CM_CO_MERGE_NEXT_WP);
	/*Check SDO process is successful and correct service*/
	if(CO_SDO_get_status(&p_app->base.co_app.sdo_client) == CO_SDO_RT_success &&
			p_app->base.co_service == CM_CO_MERGE_NEXT_WP){

		bp_set_main_sw_index(p_app->next_working_port->bp, BMS_ON_ENTIRE);
		bs_app_update_working_port(p_app);
		p_app->working_port_num++;
		bs_app_set_sw_process_state(p_app, BS_NO_SWITCH);
		CO_SDO_reset_status(&p_app->base.co_app.sdo_client);
	}
	else if(CO_SDO_get_status(&p_app->base.co_app.sdo_client) == CO_SDO_RT_abort &&
			p_app->base.co_service == CM_CO_MERGE_NEXT_WP){
		bs_app_set_sw_process_state(p_app, BS_NO_SWITCH);
		CO_SDO_reset_status(&p_app->base.co_app.sdo_client);
	}
}

/* Turn OFF Charge Mosfet of current working_port */
static void turn_off_charge_fet_working_port(pmu_app* p_app, const uint64_t timestamp){
	(void)timestamp;
	for( uint8_t i = 0; i < 3; i++){
		if( p_app->working_port[i] == NULL) continue;
		if( p_app->working_port[i]->bp->sw_state == BMS_ON_ENTIRE){
			bat_port_set_power_sw(p_app->working_port[i], BMS_OFF_CHARGE,CM_CO_TURN_OFF_WP);
			break;
		}
	}
	/*Check SDO process is successful and correct service*/
	if(CO_SDO_get_status(&p_app->base.co_app.sdo_client) == CO_SDO_RT_success &&
			p_app->base.co_service == CM_CO_TURN_OFF_WP){

		for( uint8_t i = 0; i < 3; i++){
			if( p_app->working_port[i] == NULL) continue;
			if( p_app->working_port[i]->bp->sw_state == BMS_ON_ENTIRE &&
					(p_app->working_port[i] == p_app->impl_port)){
				bp_set_main_sw_index(p_app->working_port[i]->bp, BMS_OFF_CHARGE);
				CO_SDO_reset_status(&p_app->base.co_app.sdo_client);
			}
		}
		for( uint8_t i = 0; i < 3; i++){
			if( p_app->working_port[i] == NULL) continue;
			if( p_app->working_port[i]->bp->sw_state == BMS_ON_ENTIRE){
				CO_SDO_reset_status(&p_app->base.co_app.sdo_client);
				bat_port_set_power_sw(p_app->working_port[i], BMS_OFF_CHARGE, CM_CO_TURN_OFF_WP);
				return;
			}
		}

		bs_app_set_sw_process_state(p_app, BS_MAIN_SWITCH);
		wait_for_stable_charging = MAIN_TO_FINISH_INTERVAL_mS;
		sw_delay_cnt = PRE_TO_MAIN_SW_INTERVAL_mS;
		CO_SDO_reset_status(&p_app->base.co_app.sdo_client);
	}
	else if(CO_SDO_get_status(&p_app->base.co_app.sdo_client) == CO_SDO_RT_abort &&
			p_app->base.co_service == CM_CO_TURN_OFF_WP){
		bs_app_set_sw_process_state(p_app, BS_NO_SWITCH);
		CO_SDO_reset_status(&p_app->base.co_app.sdo_client);
	}

}

/* Turn ON entire Mosfet of next_working_port */
static void turn_on_entire_fet_next_working_port(pmu_app* p_app, uint64_t timestamp){
	(void)timestamp;

	if(!is_pre_sw_state_completed()) return;
	if(p_app->next_working_port == NULL){
		bs_app_set_sw_process_state(p_app, BS_RECOVERY_SWITCH);
		CO_SDO_reset_status(&p_app->base.co_app.sdo_client);
		return;
	}
	if(p_app->mode == BP_CHARGE){
		if(wait_for_stable_charging > 10) {
			wait_for_stable_charging -=  sys_tick_ms;
			return;
		}
	}
	bat_port_set_power_sw(p_app->next_working_port, BMS_ON_ENTIRE,CM_CO_TURN_ON_WP);
	/*Check SDO process is successful and correct service*/
	if(CO_SDO_get_status(&p_app->base.co_app.sdo_client) == CO_SDO_RT_success &&
			p_app->base.co_service == CM_CO_TURN_ON_WP){
		if( p_app->mode == BP_DISCHARGE){
			bp_set_main_sw_index(p_app->next_working_port->bp, BMS_ON_ENTIRE);
			bs_app_update_working_port(p_app);
			p_app->working_port_num++;
			bs_app_set_sw_process_state(p_app, BS_FINISH_SWITCH);
			sw_delay_cnt = MAIN_TO_FINISH_INTERVAL_mS;
		}
		else if( p_app->mode == BP_CHARGE){
			bp_set_main_sw_index(p_app->next_working_port->bp, BMS_ON_ENTIRE);
			p_app->working_port_num++;
			bs_app_update_working_port(p_app);// !!! location can test error
			bs_app_set_sw_process_state(p_app, BS_NO_SWITCH);

		}
		CO_SDO_reset_status(&p_app->base.co_app.sdo_client);
	}
	else if(CO_SDO_get_status(&p_app->base.co_app.sdo_client) == CO_SDO_RT_abort &&
			p_app->base.co_service == CM_CO_TURN_ON_WP){
		bp_set_main_sw_index(p_app->next_working_port->bp, BMS_OFF_ENTIRE);
		bs_app_set_sw_process_state(p_app, BS_RECOVERY_SWITCH);
		CO_SDO_reset_status(&p_app->base.co_app.sdo_client);
	}
}
static void softstart_fet_working_port(pmu_app* p_app, const uint64_t timestamp){
	(void)timestamp;
	if(!is_pre_sw_state_completed()) return;
	if(p_app->next_working_port == NULL){
		bs_app_set_sw_process_state(p_app, BS_NO_SWITCH);
		CO_SDO_reset_status(&p_app->base.co_app.sdo_client);
		return;
	}
	/* off buck converter in 2000ms*/

	bat_port_set_power_sw(p_app->next_working_port, BMS_ON_ENTIRE,CM_CO_SOFTSTART_WP);
	if(CO_SDO_get_status(&p_app->base.co_app.sdo_client) == CO_SDO_RT_success &&
			p_app->base.co_service == CM_CO_SOFTSTART_WP){
		bp_set_main_sw_index(p_app->next_working_port->bp, BMS_ON_ENTIRE);
		bs_app_update_working_port(p_app);
		p_app->working_port_num++;
		bs_app_set_sw_process_state(p_app, BS_NO_SWITCH);


		CO_SDO_reset_status(&p_app->base.co_app.sdo_client);
	}
	else if(CO_SDO_get_status(&p_app->base.co_app.sdo_client) == CO_SDO_RT_abort &&
			p_app->base.co_service == CM_CO_SOFTSTART_WP){
		bp_set_main_sw_index(p_app->next_working_port->bp, BMS_OFF_ENTIRE);
		bs_app_set_sw_process_state(p_app, BS_NO_SWITCH);
		CO_SDO_reset_status(&p_app->base.co_app.sdo_client);
	}

}

static void turn_on_charge_fet_working_port(pmu_app* p_app, const uint64_t timestamp){
	(void)timestamp;
	for( uint8_t i = 0; i < 3; i++){
		if( p_app->working_port[i] == NULL) continue;
		if( p_app->working_port[i]->bp->sw_state == BMS_OFF_CHARGE){
			bat_port_set_power_sw(p_app->working_port[i], BMS_ON_ENTIRE,CM_CO_TURN_ON_CHARGER_FET);
			break;
		}
	}
	if(CO_SDO_get_status(&p_app->base.co_app.sdo_client) == CO_SDO_RT_success&&
			p_app->base.co_service == CM_CO_TURN_ON_CHARGER_FET){
		for( uint8_t i = 0; i < 3; i++){
			if( p_app->working_port[i] == NULL) continue;
			if( p_app->working_port[i]->bp->sw_state == BMS_OFF_CHARGE &&
					(p_app->working_port[i] == p_app->impl_port)){
				bp_set_main_sw_index(p_app->working_port[i]->bp, BMS_ON_ENTIRE);
				CO_SDO_reset_status(&p_app->base.co_app.sdo_client);
			}
		}
		for( uint8_t i = 0; i < 3; i++){
			if( p_app->working_port[i] == NULL) continue;
			if( p_app->working_port[i]->bp->sw_state == BMS_OFF_CHARGE){
				CO_SDO_reset_status(&p_app->base.co_app.sdo_client);
				bat_port_set_power_sw(p_app->working_port[i], BMS_ON_ENTIRE,CM_CO_TURN_ON_CHARGER_FET);
				return;
			}
		}
		bs_app_set_sw_process_state(p_app, BS_NO_SWITCH);
		CO_SDO_reset_status(&p_app->base.co_app.sdo_client);
	}
	else if(CO_SDO_get_status(&p_app->base.co_app.sdo_client) == CO_SDO_RT_abort&&
			p_app->base.co_service == CM_CO_TURN_ON_CHARGER_FET){
		bs_app_set_sw_process_state(p_app, BS_NO_SWITCH);
		CO_SDO_reset_status(&p_app->base.co_app.sdo_client);
	}
}

/* Turn OFF entire Mosfet of current working_port, change working_port to next_working_port */
static void turn_off_entire_fet_working_port(pmu_app* p_app, uint64_t timestamp){
	(void)timestamp;

	if(!is_pre_sw_state_completed()) return;
	if( p_app->mode == BP_DISCHARGE) {
		for( uint8_t i = 0; i < 3; i++){
			if( p_app->working_port[i] == NULL) continue;
			if( p_app->working_port[i]->bp->sw_state == BMS_OFF_CHARGE){
				bat_port_set_power_sw(p_app->working_port[i], BMS_OFF_ENTIRE,CM_CO_TURN_OFF_ALL_FET);
				break;
			}
		}
	}
	else {
		for( uint8_t i = 0; i < 3; i++){
			if( p_app->working_port[i] == NULL) continue;
			if( p_app->working_port[i]->bp->sw_state == BMS_ON_ENTIRE){
				bat_port_set_power_sw(p_app->working_port[i], BMS_OFF_ENTIRE, CM_CO_TURN_OFF_ALL_FET);
				break;
			}
		}
	}
	if(CO_SDO_get_status(&p_app->base.co_app.sdo_client) == CO_SDO_RT_success&&
			p_app->base.co_service == CM_CO_TURN_OFF_ALL_FET){

		if( p_app->mode == BP_DISCHARGE) {
			for( uint8_t i = 0; i < 3; i++){
				if( p_app->working_port[i] == NULL) continue;
				if( p_app->working_port[i]->bp->sw_state == BMS_OFF_CHARGE &&
						p_app->working_port[i] == p_app->impl_port){
					bp_set_main_sw_index(p_app->working_port[i]->bp, BMS_OFF_ENTIRE);
					p_app->working_port[i] = NULL;
					bs_app_shift_working_port(p_app);
					p_app->working_port_num --;
					CO_SDO_reset_status(&p_app->base.co_app.sdo_client);
				}
			}
			for( uint8_t i = 0; i < 3; i++){
				if( p_app->working_port[i] == NULL) continue;
				if( p_app->working_port[i]->bp->sw_state == BMS_OFF_CHARGE){
					CO_SDO_reset_status(&p_app->base.co_app.sdo_client);
					bat_port_set_power_sw(p_app->working_port[i], BMS_OFF_ENTIRE, CM_CO_TURN_OFF_ALL_FET);
					return;
				}
			}
			bs_app_set_sw_process_state(p_app, BS_NO_SWITCH);
			CO_SDO_reset_status(&p_app->base.co_app.sdo_client);

		}
		else if( p_app->mode == BP_CHARGE){

			for( uint8_t i = 0; i < 3; i++){
				if( p_app->working_port[i] == NULL) continue;
				if( p_app->working_port[i]->bp->sw_state == BMS_ON_ENTIRE &&
						(p_app->working_port[i] == p_app->impl_port)){
					bp_set_main_sw_index(p_app->working_port[i]->bp, BMS_OFF_ENTIRE);
					p_app->working_port[i] = NULL;
					bs_app_shift_working_port(p_app);
					p_app->working_port_num --;
					CO_SDO_reset_status(&p_app->base.co_app.sdo_client);
				}
			}
			for( uint8_t i = 0; i < 3; i++){
				if( p_app->working_port[i] == NULL) continue;
				if( p_app->working_port[i]->bp->sw_state == BMS_ON_ENTIRE){
					CO_SDO_reset_status(&p_app->base.co_app.sdo_client);
					bat_port_set_power_sw(p_app->working_port[i], BMS_OFF_ENTIRE, CM_CO_TURN_OFF_ALL_FET);
					return;
				}
			}
			sw_delay_cnt = MAIN_TO_FINISH_INTERVAL_mS;
			wait_for_stable_charging = 10*MAIN_TO_FINISH_INTERVAL_mS;
			bs_app_set_sw_process_state(p_app, BS_MAIN_SWITCH);
			CO_SDO_reset_status(&p_app->base.co_app.sdo_client);
		}
		CO_SDO_reset_status(&p_app->base.co_app.sdo_client);
	}
	else if(CO_SDO_get_status(&p_app->base.co_app.sdo_client) == CO_SDO_RT_abort&&
			p_app->base.co_service == CM_CO_TURN_OFF_ALL_FET){
		bs_app_set_sw_process_state(p_app, BS_NO_SWITCH);
		CO_SDO_reset_status(&p_app->base.co_app.sdo_client);
	}
}
void turn_on_softstart_power_sys(pmu_app* p_app , const uint64_t timestamp){
	(void)timestamp;
	for( uint8_t i = 0; i < 3; i++){
		if(p_app->ports[i].bp->base.con_state == CO_SLAVE_CON_ST_CONNECTED &&
				p_app->ports[i].bp->state == BP_ST_STANDBY	){
			bat_port_set_power_sw(&p_app->ports[i], BMS_ON_ENTIRE, CM_CO_SOFTSTART_PS);
		}
	}
	if(CO_SDO_get_status(&p_app->base.co_app.sdo_client) == CO_SDO_RT_success &&
			p_app->base.co_service == CM_CO_SOFTSTART_PS){
		CO_SDO_reset_status(&p_app->base.co_app.sdo_client);
		for( uint8_t i = 0; i < 3; i++){
			if( &p_app->ports[i] == p_app->impl_port){
				CO_SDO_reset_status(&p_app->base.co_app.sdo_client);
				bp_set_main_sw_index(p_app->ports[i].bp, BMS_ON_ENTIRE);
				p_app->working_port[0] = &p_app->ports[i];
				p_app->working_port_num++;
				bs_app_set_state(p_app, PMU_ST_ACTIVE);
			}
		}
	}
	else if(CO_SDO_get_status(&p_app->base.co_app.sdo_client) == CO_SDO_RT_abort &&
			p_app->base.co_service == CM_CO_SOFTSTART_PS){
		CO_SDO_reset_status(&p_app->base.co_app.sdo_client);
	}
}
void bs_app_update_next_working_port_discharge(pmu_app* p_app){
	int32_t diff_vol_A;
	int32_t diff_vol_B;
	p_app->next_working_port = NULL;
	p_app->act_working_port = bs_app_get_act_working_port(p_app);
	for(uint8_t i = 0; i < p_app->bat_port_num; i++){
		if((p_app->ports[i].bp->base.con_state != CO_SLAVE_CON_ST_CONNECTED)
				|| (p_app->ports[i].bp->sw_state == BMS_ON_ENTIRE)
				|| (p_app->ports[i].bp->vol == 0)
				|| (p_app->ports[i].bp->state == BP_ST_FAULT)
				|| (p_app->ports[i].bp->valid_st == INVALID)) continue;

		if(p_app->next_working_port == NULL){
			/* Update first next_working_port */
			p_app->next_working_port = &p_app->ports[i];
			continue;
		}
		if(p_app->act_working_port == NULL){
			continue;
		}
		diff_vol_A = p_app->next_working_port->bp->vol - p_app->act_working_port->bp->vol ;
		diff_vol_B = p_app->ports[i].bp->vol - p_app->act_working_port->bp->vol;
		if(diff_vol_A > 0 && diff_vol_B > 0){
			if(diff_vol_A > diff_vol_B ){
				p_app->next_working_port = &p_app->ports[i];
			}
			continue;
		}
		else if( diff_vol_B > 0 ){
			p_app->next_working_port = &p_app->ports[i];
			continue;
		}
		else if(diff_vol_A < 0 && diff_vol_B < 0){
			if(diff_vol_A < diff_vol_B ){
				p_app->next_working_port = &p_app->ports[i];
			}
			continue;
		}
	}
}

void bs_app_reboot_power_sys(pmu_app* p_app){
	if((p_app->sw_type != BS_NO_SWITCH)
			|| (p_app->next_working_port == NULL)
	) return;

	if( p_app->ports[0].bp->sw_state == BMS_OFF_ENTIRE
			&& p_app->ports[1].bp->sw_state == BMS_OFF_ENTIRE
			&& p_app->ports[2].bp->sw_state == BMS_OFF_ENTIRE ){
		bs_app_set_sw_process_state(p_app, BS_SOFTSTART_SWITCH);
	}
}

uint32_t 		noise_filter_cnt =0;
uint32_t 		noise_filter_cnt_high_cur =0;
void bs_app_update_switch_type_discharge(pmu_app* p_app){

	if((p_app->next_working_port == NULL)
			|| (p_app->act_working_port == NULL)) return;

	int32_t diff_voltage = p_app->next_working_port->bp->vol
			- p_app->act_working_port->bp->vol;

	is_current_enable_switch = bs_app_check_current_enable_switch(p_app);
	//if(!is_current_enable_switch) {
	if((abs(diff_voltage) <= UPPER_MERGE_VOL_DIFF_DISCHARGE_H_LOAD_mV)){
		if(bs_app_calculate_current_DCbus(p_app) < CUR_H_LOAD_ENABLE_BP_SW_THRESHOLD_mA){
			if(noise_filter_cnt_high_cur++ > CNT_100){
				bs_app_set_sw_process_state(p_app, BS_MERGE_SWITCH);
				p_app->sw_type = BS_MERGE_SWITCH;
			}
		}
		else {
			noise_filter_cnt_high_cur = 0;
		}
	}
	else {
		noise_filter_cnt_high_cur = 0;
	}

	if(!is_current_enable_switch)  {
		return;
	}
	/* Check for Merge*/
	if((diff_voltage >= LOWER_MERGE_VOL_DIFF_DISCHARGE_mV)
			&& (diff_voltage <= UPPER_MERGE_VOL_DIFF_DISCHARGE_mV)){
		if(noise_filter_cnt ++ > CNT_100){
			bs_app_set_sw_process_state(p_app, BS_MERGE_SWITCH);
			noise_filter_cnt = 0;
		}
	}
	/* Check for Switch*/

	else if(diff_voltage > BP_SW_VOL_DIFF_LOWER_DISCHARGE_mV){
		if(noise_filter_cnt ++ > CNT_100){
			bs_app_set_sw_process_state(p_app, BS_PRE_SWICTH);
			noise_filter_cnt = 0;
		}
	}
	else noise_filter_cnt = 0;
	/* reboot the system if All of port are off*/

}
void bs_app_update_next_working_port_charge(pmu_app* p_app){


	int32_t diff_vol_A;
	int32_t diff_vol_B;
	p_app->next_working_port = NULL;
	for(uint8_t i = 0; i < p_app->bat_port_num; i++){
		if((p_app->ports[i].bp->base.con_state != CO_SLAVE_CON_ST_CONNECTED)
				|| (p_app->ports[i].bp->sw_state == BMS_ON_ENTIRE)
				|| (p_app->ports[i].bp->vol == 0)
				|| (p_app->ports[i].bp->state == BP_ST_FAULT)) continue;

		if(p_app->next_working_port == NULL){
			/* Update first next_working_port */
			p_app->next_working_port = &p_app->ports[i];
			continue;
		}

		diff_vol_A = p_app->next_working_port->bp->vol - p_app->working_port[0]->bp->vol ;
		diff_vol_B = p_app->ports[i].bp->vol - p_app->working_port[0]->bp->vol;
		if(diff_vol_A < 0 && diff_vol_B < 0){
			if(diff_vol_A > diff_vol_B ){
				p_app->next_working_port = &p_app->ports[i];
			}
			continue;
		}
		else if( diff_vol_B < 0 ){
			p_app->next_working_port = &p_app->ports[i];
			continue;
		}
		else if(diff_vol_A > 0 && diff_vol_B > 0){
			if(diff_vol_A > diff_vol_B ){
				p_app->next_working_port = &p_app->ports[i];
			}
			continue;
		}

	}
}

void bs_app_update_switch_type_charge(pmu_app* p_app){
	//need to consider the case where all working ports are NULL
	if((p_app->next_working_port == NULL)
			|| (p_app->working_port[0] == NULL)) return;

	int32_t diff_voltage = p_app->working_port[0]->bp->vol - p_app->next_working_port->bp->vol;

	if(abs(diff_voltage) <= UPPER_MERGE_VOL_DIFF_CHARGE_mV){
		bs_app_set_sw_process_state(p_app, BS_MERGE_SWITCH);
		p_app->sw_type = BS_MERGE_SWITCH;
	}
	else if(diff_voltage > BP_SW_VOL_DIFF_CHARGE_mV){
		bs_app_set_sw_process_state(p_app, BS_FINISH_SWITCH);
	}
}
/* Check current condition to enable BP Switch */
static inline bool bs_app_check_current_enable_switch(pmu_app* p_app){
	int32_t cur_t;
	if(p_app->working_port[0] != NULL	){
		cur_t = p_app->working_port[0]->bp->cur;
		if(abs(cur_t) > CUR_ENABLE_BP_SW_THRESHOLD_mA){
			check_current_switch_time = 0;
			return false;
		}
	}
	if(p_app->working_port[1] != NULL	){
		cur_t = p_app->working_port[1]->bp->cur;
		if(abs(cur_t) > CUR_ENABLE_BP_SW_THRESHOLD_mA){
			check_current_switch_time = 0;
			return false;
		}
	}

	if(check_current_switch_time < CUR_ENABLE_BP_SW_TIME_mS){
		check_current_switch_time += sys_tick_ms;
		return false;
	}
	return true;
}
static inline int32_t bs_app_calculate_current_DCbus(pmu_app* p_app){
	return 	p_app->ports[0].bp->cur +
			p_app->ports[1].bp->cur +
			p_app->ports[2].bp->cur;
}

/* Use for Scan BP process */
void bs_app_update_first_working_port(pmu_app* p_app){
	for(uint8_t i = 0; i < p_app->bat_port_num; i++){
		if((CO_Slave*)p_app->ports[i].bp != p_app->base.assigning_slave) continue;
		bat_port_set_working_port(&p_app->ports[i]);
		p_app->working_port[0] = &p_app->ports[i];
		p_app->working_port_num++;
		return;
	}
}

void bs_app_update_working_port(pmu_app* p_app){

	for(uint8_t i = 0; i < p_app->bat_port_num ; i++ ){
		if(p_app->working_port[i] == p_app->next_working_port){
			return;
		}
		if(p_app->working_port[i] == NULL ){
			p_app->working_port[i] = p_app->next_working_port;
			return;
		}
	}
}

/* Get working_port_id from bat_port_id */
static uint8_t bs_app_get_working_port_id(pmu_app* p_app, const uint8_t port_id){
	for(uint8_t i = 0; i < p_app->bat_port_num; i++){
		if(p_app->working_port[i] != &p_app->ports[port_id]) continue;
		return i;
	}
	return 0;
}
static Bat_port* bs_app_get_act_working_port(pmu_app* p_app ){
	for(uint8_t i = 0; i < p_app->bat_port_num; i++){
		if(p_app->working_port[i] == NULL) continue;
		return p_app->working_port[i];
	}
	return NULL;
}


/* Shift Working_port list to the left when a working_port disconnected */
void bs_app_remove_working_port(pmu_app* p_app, uint8_t i){
	bat_port_reset(&p_app->ports[i]);
	reset_buff_bp_pdo(&p_app->base.PDO.BP_PDO[i]);
	for(uint8_t j = 0; j < p_app->bat_port_num; j++){
		if(p_app->working_port[j] != &p_app->ports[i]) continue;
		p_app->working_port[j] = NULL;
		if(p_app->working_port[0] == NULL){
			p_app->working_port[0] = p_app->working_port[1];
			p_app->working_port[1] = p_app->working_port[2];
			p_app->working_port[2] = NULL;
		}
		if(p_app->working_port[1] == NULL){
			p_app->working_port[1] = p_app->working_port[2];
			p_app->working_port[2] = NULL;
		}
		p_app->working_port_num--;

		return ;
	}


}
/* Shift Working_port list  */
static void bs_app_shift_working_port(pmu_app* p_app){
	for( uint8_t i = 0; i < 2; i++){
		if( p_app->working_port[i] == NULL){
			p_app->working_port[i] = p_app->working_port[i + 1];
			p_app->working_port[i + 1] = NULL;
		}
	}
}
static uint8_t bs_app_get_number_of_bp_in_vehicle(pmu_app* p_app){
	uint8_t num = 0;
	for(uint8_t i = 0; i < p_app->bat_port_num; i++){
		if(p_app->ports[i].bp->state == BP_ST_INIT) continue;
		num++;
	}
	return num;
}

static inline bool is_pre_sw_state_completed(void){
	if(sw_delay_cnt == 0)
		return true;

	sw_delay_cnt -= sys_tick_ms;
	return false;
}

uint64_t blink_time = 0;

void sys_reset(pmu_app* p_app){
	(void)p_app;
	__NVIC_SystemReset();

}

