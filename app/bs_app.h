

#ifndef APP_BS_APP_H_
#define APP_BS_APP_H_

#include "can_master.h"
#include "switch.h"
#include "io_control.h"
#include "board.h"
#include "app_config.h"
#include "bat_port.h"
#include "adc_sensor.h"
#include "delay.h"
#include "app_update.h"
#include "io_control.h"
#include "app_co_init.h"
#include "app_co_comm_od.h"
#include "app_co_manu_od.h"
#include "app_co_od_simply_config.h"
#include "app_co_storage_init.h"
#include "CO_SDO.h"

typedef enum BS_STATE{
	PMU_ST_SETUP = 0,
	PMU_ST_IDLE,
	PMU_ST_ACTIVE,
	PMU_ST_FAIL,
	PMU_ST_SLEEP,
	PMU_ST_BOOT
} BS_STATE;

typedef enum {
	BP_DISCHARGE = 0,
	BP_CHARGE = 1
} BP_MODE;

typedef enum PMU_SW_TYPE{
	BS_NO_SWITCH,
	BS_PRE_SWICTH,
	BS_MAIN_SWITCH,
	BS_FINISH_SWITCH,
	BS_MERGE_SWITCH,
	BS_RECOVERY_SWITCH,
	BS_SOFTSTART_SWITCH
} BS_SW_TYPE;
typedef enum {
	MC_ANTI_INACTIVATED 		= 1,
	MC_ANTI_ACTIVATED_IDLE 		= 2,
	MC_ANTI_ACTIVATED_RUNNING	= 3
} MC_ANTI_THEFT;

typedef struct {
	uint32_t 	speed;
	uint32_t 	throttle_command;
	uint32_t 	estimate_torque;
	uint8_t 	estimate_DC_cur;
	uint8_t 	allow_DC_cur;
	uint8_t 	errol;
	uint8_t 	board_temp;
	uint8_t 	motor_temp;
	uint8_t 	MC_status;
	uint32_t 	Volt_DC;
	uint8_t 	acceleration;
	uint32_t 	charge_capacitor_time;
	uint32_t	inactive;
	MC_ANTI_THEFT anti_st;

}MC;


typedef struct BS_app_t pmu_app;
struct BS_app_t{
	CAN_master	base;
	BS_STATE	state;
	BS_SW_TYPE 	sw_type;
	bool		is_softstart;
	uint32_t 	sw_type_timeout;
	BP_MODE		mode;
	Bat_port* 	ports;
	Bat_port* 	impl_port;
	Bat_port* 	working_port[BAT_PORT_NUM];
	Bat_port*	next_working_port;
	Bat_port*	act_working_port;
	uint8_t 	working_port_num;
	void		(*check_working_port_num)(pmu_app*);
	uint8_t 	working_port_discharge_num;
	uint8_t		bat_port_num;
	uint8_t		distance_available;
	int32_t		cur_max_bp; // Cur max from BPs
	uint32_t 	switch_bp_time;
	uint32_t 	merger_bp_time;
	uint8_t		is_read_sn_request;
	uint8_t		is_read_sn;
	uint8_t 	read_sn_request;
	uint32_t 	softstart_off_delay_ms;
	bool		is_unbalance_mode;
	bool		is_ready_stream_distance;
	bool		is_bp_disconnect;
	bool		is_new_bp_data;
	bool 		is_new_app_update;
	App_update*	app_update;
	bool		is_wakeup;
	uint64_t 	sw_timeout;
	bool 		sw_is_change;
	char 		vehicle_sn[32];
	bool		active_sleep_mode;
	bool		is_power_sys_block;
	bool		is_request_upgrade;
	uint16_t	pmu_error;
	uint32_t	bootState_timeout;
	IOC 		ioc;

};

extern pmu_app selex_bs_app;
extern uint32_t io_timestamp ;
extern volatile uint64_t sys_timestamp;
extern const uint32_t sys_tick_ms;
extern uint64_t update_distance_timestamp;
extern volatile int32_t mc_cur_limit;
void bs_app_update_key_state						(pmu_app* p_app);
void bs_app_update_bp_switch_state					(pmu_app* p_app);
void bs_app_update_working_port						(pmu_app* p_app);
void bs_app_update_switch_type_charge				(pmu_app* );
void bs_app_update_switch_type_discharge			(pmu_app* );
void bs_app_reboot_power_sys						(pmu_app* );
void bs_app_update_next_working_port_discharge		(pmu_app*);
void bs_app_update_next_working_port_charge			(pmu_app*);
void bs_app_update_distance_available				(pmu_app* p_app, const uint32_t timestamp);
void bs_app_stream_bp_sn							(pmu_app*);
void bs_app_update_first_working_port				(pmu_app*);
void pmu_app_check_connected_port_state				(pmu_app*);
void pmu_app_verify_BP_valid						(pmu_app*);
void bs_app_reset_to_idle_state						(pmu_app*);
void bs_app_update_charger_state					(pmu_app*);
void pmu_app_update_bp_oparetion_mode				(pmu_app* );
void pmu_app_update_sleep_mode						(pmu_app* );
void pmu_app_abp_manage								(pmu_app* );
void build_package_data_uart						(const pmu_app* const p_app,uint8_t* s);
void BP_SN_uart_sent								(void);
void hardware_init									(void);
void app_init										(pmu_app*);
void buck_converter_manage							(pmu_app* p_app);
void get_sn_device_from_hmi							(pmu_app* p_app);
void MC_paramaters_reset							(MC* mc);
void turn_on_softstart_power_sys					(pmu_app*, const uint64_t);
void hmi_command_update								(pmu_app*);
void pmu_switch_update_timeout						(pmu_app* p_app, uint64_t timestamp);
void pmu_app_check_switch_sleep_mode				(pmu_app* p_app, uint64_t timestamp);

void pmu_app_update_ev_anti_theft_mode				(pmu_app* p_app);
bool string_cmp										(char const str1[], char const str2[], uint32_t lenght);
void bs_app_remove_working_port						(pmu_app* , uint8_t );
void ev_protect_init								(pmu_app*);

void pmu_app_update_ev_protect_mode					(pmu_app* p_app,uint64_t timestamp);
void sys_reset										(pmu_app* p_app);


static inline void bs_app_set_state(pmu_app* p_app, BS_STATE state){
	p_app->state = state;
}

static inline BS_STATE bs_app_get_state(pmu_app* p_app){
	return p_app->state;
}

static inline void bs_app_set_sw_process_state(pmu_app* p_app, BS_SW_TYPE type){
	if (p_app->sw_type == type) return;
	p_app->sw_type = type;
	p_app->sw_is_change = true;
}

static inline void bs_app_set_mode(pmu_app* p_app, BP_MODE mode){
	p_app->mode = mode;
}

typedef void (*sw_bp_act)(pmu_app*, const uint64_t);
static inline void bs_app_process_bp_sw(sw_bp_act sw_act){
	sw_act(&selex_bs_app, sys_timestamp);
}
static inline bool pmu_sw_is_timeout(pmu_app* p_app, uint64_t sys_timestamp){
	return (sys_timestamp >= p_app->sw_timeout ) && (p_app->sw_timeout != 0);
}

static inline void set_charge_capcitor_time(MC* p_mc){
	p_mc->charge_capacitor_time = 100;
}

#endif /* APP_BS_APP_H_ */
