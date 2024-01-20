/*
 * canopen_init.c
 *
 *  Created on: Oct 21, 2020
 *      Author: quangnd
 */

#include "app_co.h"
#include "app_config.h"
#define READ_SN_ATTEMP_NUM			1
static uint8_t read_sn_attemp_cnt = 0;
static uint16_t sdo_client_reset_cnt = 0;
static CO_Slave*	bp_slaves[BAT_PORT_NUM];

static void can_master_slave_select_impl(const CAN_master*, const uint32_t);
static void can_master_slave_deselect_impl(const CAN_master*, const uint32_t);
static void on_slave_assign_success_handle(CAN_master* , const uint64_t);
static void on_slave_assign_fail_handle(CAN_master*, const uint64_t);
static void rpdo_process_handle( CAN_master* );
static void can_receive_handle(CAN_Hw* p_hw);
static void can_master_update_pdo_handle(CAN_master*, uint64_t);
static void on_slave_reassign_success_handle(CAN_master*, const uint64_t);


void app_co_master_init(pmu_app* p_app){
	can_master_init((CAN_master*)p_app, bp_slaves, p_app->bat_port_num, &can_port);
	p_app->base.slave_start_node_id = BP_DEFAULT_NODE_ID_NUM;
	p_app->base.sdo_server.rx_address = CO_CAN_ID_RSDO;
	p_app->base.sdo_server.tx_address = CO_CAN_ID_TSDO;

	p_app->base.slave_select = can_master_slave_select_impl;
	p_app->base.slave_deselect = can_master_slave_deselect_impl;
	p_app->base.on_slave_assign_success = on_slave_assign_success_handle;
	p_app->base.on_slave_assign_fail = on_slave_assign_fail_handle;
	p_app->base.pdo_process = rpdo_process_handle;
	p_app->base.update_pdo_handle = can_master_update_pdo_handle;
	p_app->base.on_slave_reassign_success = on_slave_reassign_success_handle;
	can_set_receive_handle(&can_port, can_receive_handle);
	p_app->base.BUS_CAN_state = BUSCAN_ST_IDLE;
	for(uint8_t i = 0; i < p_app->base.slave_num; i++){
		bp_slaves[i] = (CO_Slave*) (p_app->ports[i].bp);
		bp_slaves[i]->node_id = BP_DEFAULT_NODE_ID_NUM;
		bp_slaves[i]->inactive_time_ms = 0;
		bp_slaves[i]->sdo_server_address = CO_CAN_ID_TSDO + bp_slaves[i]->node_id;
		for(uint8_t j = 0; j < 32; j++) bp_slaves[i]->sn[j] = 0;
		co_slave_set_con_state(bp_slaves[i], CO_SLAVE_CON_ST_DISCONNECT);
	}
	for(uint8_t i = 0; i < p_app->bat_port_num; i++){
		bp_set_con_state(p_app->ports[i].bp, CO_SLAVE_CON_ST_AUTHORIZING);
	}
}

static void can_master_slave_select_impl(const CAN_master* p_cm, const uint32_t id){
	(void)p_cm;
	bat_port_disconnect(&selex_bs_app.ports[id]);
}

static void can_master_slave_deselect_impl(const CAN_master* p_cm, const uint32_t id){
	(void)p_cm;
	bat_port_connect(&selex_bs_app.ports[id]);
}

static void on_slave_assign_success_handle(CAN_master* p_cm, const uint64_t timestamp){

	p_cm->assign_timeout = 0;
	p_cm->assign_state = CM_ASSIGN_ST_DONE;

	if(selex_bs_app.state == PMU_ST_SETUP ){
		can_master_reconfig_node_id_num(p_cm);
		p_cm->co_service = CM_CO_REASSIGN;
		can_master_start_reassign_slave_id((CAN_master*)&selex_bs_app, timestamp);
	}
	else if((selex_bs_app.state == PMU_ST_IDLE)	|| (selex_bs_app.state == PMU_ST_ACTIVE)){
		co_slave_set_con_state(p_cm->assigning_slave, CO_SLAVE_CON_ST_CONNECTED);
		can_master_start_assign_next_slave(p_cm, timestamp);
	}
}

static void on_slave_assign_fail_handle(CAN_master* p_cm, const uint64_t timestamp){
	co_slave_set_con_state(p_cm->assigning_slave, CO_SLAVE_CON_ST_DISCONNECT);
	if( selex_bs_app.state == PMU_ST_SETUP ){
		read_sn_attemp_cnt++;
		if(read_sn_attemp_cnt <= READ_SN_ATTEMP_NUM){
			can_master_start_authorize_slave(p_cm, timestamp);
			return;
		}
		read_sn_attemp_cnt = 0;
		can_master_slave_select(p_cm, can_master_get_assign_slave_id(p_cm));

		p_cm->assigning_slave = can_master_get_slave(p_cm, CO_SLAVE_CON_ST_AUTHORIZING);
		if(p_cm->assigning_slave == NULL){
			bs_app_reset_to_idle_state(&selex_bs_app);
			//bs_app_set_state(&selex_bs_app, BS_ST_IDLE);
			p_cm->assign_state = CM_ASSIGN_ST_DONE;
			return;
		}
		can_master_start_authorize_slave(p_cm, timestamp);
	}
	else if ((selex_bs_app.state == PMU_ST_IDLE)	|| (selex_bs_app.state == PMU_ST_ACTIVE)){
		can_master_slave_select(p_cm, p_cm->assigning_slave->node_id - p_cm->slave_start_node_id);
		can_master_start_assign_next_slave(p_cm, timestamp);
		//can_master_start_pdo(p_cm, timestamp);
		p_cm->cnt++;
	}
}

static void can_master_update_pdo_handle(CAN_master* p_cm, uint64_t timestamp){
	(void)timestamp;
	if(selex_bs_app.sw_type != BS_NO_SWITCH) return;
	p_cm->pdo_sync_timestamp = 0;
}

static void on_slave_reassign_success_handle(CAN_master* p_cm, const uint64_t timestamp){
	(void)timestamp;
	co_slave_set_con_state(p_cm->assigning_slave, CO_SLAVE_CON_ST_CONNECTED);
	bs_app_update_first_working_port(&selex_bs_app);
	bs_app_set_state(&selex_bs_app, PMU_ST_ACTIVE);
}
/*automatically reset SDO Client status when timeout is over*/
void CO_SDO_Client_reset_timeout(CAN_master *p_cm) {
	if (CO_SDO_get_status(&p_cm->co_app.sdo_client) == CO_SDO_RT_abort) {
		if (sdo_client_reset_cnt++ > SDO_CLIENT_RESET_TIMEOUT_10ms) {
			CO_SDO_reset_status(&p_cm->co_app.sdo_client);
		}
	}
	else{
		sdo_client_reset_cnt = 0;
	}

}
/* */

void pdo_data_processing( pmu_app * p_app) {
	PDO_t pdo 		= p_app->base.PDO;
	PDO_t *p_pdo 	= &p_app->base.PDO;
	/* BP data processing*/
	uint32_t vol_buff;
	int32_t cur_buff;
	for (uint8_t pack = 0; pack < 3; pack++) {
		if(selex_bs_app.ports[pack].bp->base.con_state != CO_SLAVE_CON_ST_CONNECTED) continue;
		if (pdo.BP_PDO[pack].BP_TPDO1.new_msg == true) {
			uint8_t BP_TPDO1_data[8];
			CO_memcpy(BP_TPDO1_data, pdo.BP_PDO[pack].BP_TPDO1.data, 8);
			vol_buff = 10 * (uint32_t) CO_getUint16(BP_TPDO1_data);
			if (vol_buff > 30000 && vol_buff < 75000) {
				selex_bs_app.ports[pack].bp->vol = vol_buff;
			}
			cur_buff = (-10) * ((int16_t) CO_getUint16(BP_TPDO1_data + 2));
			if (cur_buff > -80000 && cur_buff < 80000) {
				selex_bs_app.ports[pack].bp->cur = cur_buff;
			}
			selex_bs_app.ports[pack].bp->soc 		= BP_TPDO1_data[4];
			selex_bs_app.ports[pack].bp->soc_x10 	= 10 * BP_TPDO1_data[4];
			selex_bs_app.ports[pack].bp->state 		= BP_TPDO1_data[5];
			selex_bs_app.ports[pack].bp->status 	= CO_getUint16(
					BP_TPDO1_data + 6);
			selex_bs_app.is_new_bp_data 			= true;
			p_pdo->BP_PDO[pack].BP_TPDO1.new_msg 	= false;

		}
		if (pdo.BP_PDO[pack].BP_TPDO2.new_msg == true) {
			for (uint8_t cell = 0; cell < 4; cell++) {
				selex_bs_app.ports[pack].bp->cell_vol[cell] = CO_getUint16(
						pdo.BP_PDO[pack].BP_TPDO2.data + 2 * cell);
			}
			p_pdo->BP_PDO[pack].BP_TPDO2.new_msg = false;

		}
		if (pdo.BP_PDO[pack].BP_TPDO3.new_msg == true) {
			for (uint8_t cell = 0; cell < 4; cell++) {
				selex_bs_app.ports[pack].bp->cell_vol[cell + 4] = CO_getUint16(
						pdo.BP_PDO[pack].BP_TPDO3.data + 2 * cell);
			}
			p_pdo->BP_PDO[pack].BP_TPDO3.new_msg = false;
		}
		if (pdo.BP_PDO[pack].BP_TPDO4.new_msg == true) {
			CO_memcpy(selex_bs_app.ports[pack].bp->temp,
					pdo.BP_PDO[pack].BP_TPDO4.data, 6);
			selex_bs_app.ports[pack].bp->fet_status =
					pdo.BP_PDO[pack].BP_TPDO4.data[6];
			uint8_t soc_buff = pdo.BP_PDO[pack].BP_TPDO4.data[7];
			if ((soc_buff > 0) && (soc_buff < 10)) {
				selex_bs_app.ports[pack].bp->soc_x10 += soc_buff;
			}
			p_pdo->BP_PDO[pack].BP_TPDO4.new_msg = false;
		}
		if (pdo.BP_PDO[pack].BP_RPDO1.new_msg == true) {
			for (uint8_t cell = 0; cell < 4; cell++) {
				selex_bs_app.ports[pack].bp->cell_vol[cell + 8] = CO_getUint16(
						pdo.BP_PDO[pack].BP_RPDO1.data + 2 * cell);
			}
			p_pdo->BP_PDO[pack].BP_RPDO1.new_msg = false;
		}
		if (pdo.BP_PDO[pack].BP_RPDO2.new_msg == true) {
			for (uint8_t cell = 0; cell < 4; cell++) {
				selex_bs_app.ports[pack].bp->cell_vol[cell + 12] = CO_getUint16(
						pdo.BP_PDO[pack].BP_RPDO2.data + 2 * cell);
			}
			p_pdo->BP_PDO[pack].BP_RPDO2.new_msg = false;
		}
	}
}
#define PDO_PROCESS 0
static void rpdo_process_handle( CAN_master*  p_cm){
	uint32_t cob_id = p_cm->p_hw->can_rx.StdId & 0xFFFFFF80;
	uint8_t node_id = (uint8_t) (p_cm->p_hw->can_rx.StdId & 0x7F);
	uint8_t bp_id = 0;
	PDO_t	*pdo = &p_cm->PDO;

	bp_id = node_id - p_cm->slave_start_node_id;
	if(bp_id >= p_cm->slave_num) return;
	bp_reset_inactive_counter(selex_bs_app.ports[bp_id].bp);
	if(selex_bs_app.ports[bp_id].bp->base.con_state == CO_SLAVE_CON_ST_CONNECTED){
		switch (cob_id) {
		case BP_VOL_CUR_TPDO_COBID:
			pdo->BP_PDO[bp_id].BP_TPDO1.new_msg = true;
			CO_memcpy(pdo->BP_PDO[bp_id].BP_TPDO1.data, p_cm->p_hw->rx_data, 8);
			break;
		case BP_1_4CELLS_VOL_TPDO_COBID:
			pdo->BP_PDO[bp_id].BP_TPDO2.new_msg = true;
			CO_memcpy(pdo->BP_PDO[bp_id].BP_TPDO2.data, p_cm->p_hw->rx_data, 8);
			break;
		case BP_5_8CELLS_VOL_TPDO_COBID:
			pdo->BP_PDO[bp_id].BP_TPDO3.new_msg = true;
			CO_memcpy(pdo->BP_PDO[bp_id].BP_TPDO3.data, p_cm->p_hw->rx_data, 8);
			break;
		case BP_TEMP_TPDO_COBID:
			pdo->BP_PDO[bp_id].BP_TPDO4.new_msg = true;
			CO_memcpy(pdo->BP_PDO[bp_id].BP_TPDO4.data, p_cm->p_hw->rx_data, 8);
			break;
		case BP_9_12CELLS_VOL_TPDO_COBID:
			pdo->BP_PDO[bp_id].BP_RPDO1.new_msg = true;
			CO_memcpy(pdo->BP_PDO[bp_id].BP_RPDO1.data, p_cm->p_hw->rx_data, 8);
			break;
		case BP_13_16CELLS_VOL_TPDO_COBID:
			pdo->BP_PDO[bp_id].BP_RPDO2.new_msg = true;
			CO_memcpy(pdo->BP_PDO[bp_id].BP_RPDO2.data, p_cm->p_hw->rx_data, 8);
			break;
		default:
			break;
		}
	}
}

static void can_receive_handle(CAN_Hw *p_hw) {

	app_co_can_receive_handle(p_hw->can_rx.StdId,p_hw->rx_data);
	uint32_t cob_id = p_hw->can_rx.StdId;
	switch(p_hw->can_rx.StdId & 0xFFFFFF80){
	case CO_CAN_ID_TPDO_1:
	case CO_CAN_ID_TPDO_2:
	case CO_CAN_ID_TPDO_3:
	case CO_CAN_ID_TPDO_4:
	case CO_CAN_ID_RPDO_1:
	case CO_CAN_ID_RPDO_2:
		selex_bs_app.base.pdo_process((CAN_master*)&selex_bs_app);
		return;
	default:
		break;
	}
	/* if assign request message */
	if (cob_id == selex_bs_app.base.node_id_scan_cobid) {
		if (selex_bs_app.base.assign_state == CM_ASSIGN_ST_WAIT_REQUEST) {
			selex_bs_app.base.p_hw->can_tx.StdId = selex_bs_app.base.node_id_scan_cobid;
			selex_bs_app.base.p_hw->can_tx.DLC = 0;
			can_send(selex_bs_app.base.p_hw, selex_bs_app.base.p_hw->tx_data);
			selex_bs_app.base.assign_state = CM_ASSIGN_ST_START;
		}
		else if (selex_bs_app.base.assign_state == CM_ASSIGN_ST_SLAVE_SELECT) {
			selex_bs_app.base.assign_state = CM_ASSIGN_ST_SLAVE_SELECT_CONFIRM;
		}
		else if (selex_bs_app.base.assign_state == CM_ASSIGN_ST_WAIT_CONFIRM) {
			/* slave confirm assign id success*/
			if (p_hw->rx_data[0] != selex_bs_app.base.assigning_slave->node_id) return;
			/* finish assign for current slave and move to next slave */
			can_master_start_authorize_slave((CAN_master*) &selex_bs_app, sys_timestamp);
		}
		return;
	}
}
/*
 * when is_request_upgrade = true then reset MCU to upgrade firmware
 * is_request_upgrade is set from SDO */
void sysreset_upgrade_firmware(void) {
	if(selex_bs_app.is_request_upgrade != 1 ) return;
	selex_bs_app.is_request_upgrade = 0;
	delay(100,BSP_DELAY_UNITS_MICROSECONDS);
	sys_reset(&selex_bs_app);
}

