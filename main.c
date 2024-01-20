#include "bs_app.h"
#include "app_bat_port.h"
#include "app_co.h"
#include "delay.h"

pmu_app 			selex_bs_app;
const uint32_t 		sys_tick_ms = 10;
volatile uint64_t 	sys_timestamp = 0;

static void manage_bp_timer_irq_handle(void);
static void co_sync_process_timer_irq_handle(void);

int main(void){

	__disable_irq();
	bs_app_set_state(&selex_bs_app, PMU_ST_SETUP);
	hardware_init();
	app_init(&selex_bs_app);
	timer0_set_irq_handler(&io_scan_timer, manage_bp_timer_irq_handle);
	timer3_set_irq_handler(&io_scan_timer, co_sync_process_timer_irq_handle);

	/* Wait for power stability */
	delay(2000,BSP_DELAY_UNITS_MILLISECONDS);
	__enable_irq();

	/* Start Scan BP from Port[0] */
	selex_bs_app.base.assigning_slave = (CO_Slave*)selex_bs_app.ports[0].bp;
	can_master_start_authorize_slave((CAN_master*)&selex_bs_app, sys_timestamp);

	__enable_fault_irq();

	while(1){
		sysreset_upgrade_firmware();
		delay(2000,BSP_DELAY_UNITS_MILLISECONDS);
	}
}

void manage_bp_timer_irq_handle(void){
	sys_timestamp += sys_tick_ms;
	pdo_data_processing(&selex_bs_app);
	CO_SDO_Client_reset_timeout((CAN_master*)&selex_bs_app);

	ioc_process(&selex_bs_app.ioc, sys_timestamp, (uint8_t)selex_bs_app.mode);

	switch(selex_bs_app.state){
	case PMU_ST_SETUP:
		can_master_update_id_assign_process		((CAN_master*)&selex_bs_app, sys_timestamp);
		break;
	case PMU_ST_IDLE:
		can_master_update_id_assign_process		((CAN_master*)&selex_bs_app, sys_timestamp);
		can_master_reset_polling_assign_slave	((CAN_master*)&selex_bs_app, sys_timestamp);
		turn_on_softstart_power_sys				(&selex_bs_app,sys_timestamp);
		break;
	case PMU_ST_ACTIVE:
		pmu_app_check_connected_port_state		(&selex_bs_app);
		pmu_app_update_bp_oparetion_mode		(&selex_bs_app);
		can_master_update_id_assign_process		((CAN_master*)&selex_bs_app, sys_timestamp);
		can_master_reset_polling_assign_slave	((CAN_master*)&selex_bs_app, sys_timestamp);
		break;
	case PMU_ST_SLEEP:
		break;
	case PMU_ST_BOOT:
		selex_bs_app.bootState_timeout += sys_tick_ms;
		if(selex_bs_app.bootState_timeout > BOOT_STATE_TIMEOUT){
			bs_app_set_state(&selex_bs_app, PMU_ST_ACTIVE);
		}
		break;
	case PMU_ST_FAIL:
		break;
	default:
		break;
	}
}
static bool sync_was = false;
static bool sync_was_last;
static void co_sync_process_timer_irq_handle(void){
	CO_process(&selex_bs_app.base.co_app,1);
	sync_was_last = sync_was;
	sync_was = CO_SYNC_process(&selex_bs_app.base.co_app.sync, 1, 1);
	CO_process_tpdo(&selex_bs_app.base.co_app, 1, sync_was_last);
}
