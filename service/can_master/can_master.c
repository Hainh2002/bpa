/*
 * can_master.c
 *
 *  Created on: Apr 1, 2021
 *      Author: KhanhDinh
 */

#include "can_master.h"
#include "app_update.h"
#define ASSIGN_TIMEOUT_mS								200
#define READ_SN_SDO_TIMEOUT_mS							200
#define REASSIGN_ID_SDO_TIMEOUT_mS						200
#define PDO_SYNC_TIME_INTERVAL_mS						1000

#define SDO_CS_INIT_READ                0
#define SDO_CS_SEGMENT_READ             1
#define SDO_CS_FINISH_READ              2

#define SDO_CS_INIT_WRITE               4
#define SDO_CS_SEGMENT_WRITE            5
#define SDO_CS_FINISH_WRITE             6
#define SDO_CS_ABORT                    7
#define CNT_50ms                    	5

uint16_t cnt_reset_polling = 0;
uint32_t test_check_sum = 0;
uint32_t test_add = 0;
uint32_t f_rx = 0;
static uint64_t delay_nodeID = 0;

static void can_master_process_sdo(CAN_master *p_cm, const uint32_t timestamp);
//void co_od_get_object_data_buff(CAN_master *p_cm,const uint32_t mux,uint8_t **buff);

CAN_master* can_master_construct(void) {
	CAN_master *p_cm = (CAN_master*) malloc(sizeof(CAN_master));
	while (p_cm == NULL)
		;
	return p_cm;
}

void can_master_init(CAN_master *p_cm, CO_Slave **slaves,
		const uint32_t slave_num, CAN_Hw *p_hw) {
	p_cm->node_id_scan_cobid = CAN_NODE_ID_ASSIGN_COBID;
	p_cm->slaves = slaves;
	p_cm->p_hw = p_hw;
	p_cm->sdo_server.state = SDO_ST_IDLE;
	p_cm->sdo_server.is_new_msg = 0;
	p_cm->sdo_server.is_new_msg_boot_hmi = 0;
	p_cm->slave_num = slave_num;
	p_cm->assign_state = CM_ASSIGN_ST_DONE;
	p_cm->cnt = 0;

}
void reset_buff_bp_pdo(BP_PDO * pdo){
	pdo->BP_TPDO1.new_msg = false;
	pdo->BP_TPDO2.new_msg = false;
	pdo->BP_TPDO3.new_msg = false;
	pdo->BP_TPDO4.new_msg = false;
	pdo->BP_RPDO1.new_msg = false;
	pdo->BP_RPDO2.new_msg = false;
}
void co_send_sync(CAN_master *p_cm) {

	can_send_func(p_cm->p_hw, SYNC_BP_DATA_COBID, 0, p_cm->p_hw->tx_data);
}

/* Process CO flag */
void can_master_process(CAN_master *p_cm, const uint64_t timestamp) {
	if ((p_cm->sdo_server.timeout <= timestamp)
			&& (p_cm->sdo_server.timeout != 0)) {
		p_cm->sdo_server.state = SDO_ST_FAIL;
		p_cm->sdo_server.timeout = 0;
	}

	if (p_cm->sdo_server.is_new_msg == 1
			&& p_cm->BUS_CAN_state == BUSCAN_ST_IDLE) {
		can_master_process_sdo(p_cm, timestamp);
		//can_hw_enable_irq();
		return;
	}
}
void pmu_co_process(CAN_master *p_cm, const uint64_t timestamp) {
	pmu_process_sdo(p_cm, timestamp);
}
void can_master_disable_pdo(CAN_master *p_cm) {
	//p_cm->pdo_sync_timestamp = 0;
	for (uint8_t id = 0; id < p_cm->slave_num; id++) {
		p_cm->slaves[id]->inactive_time_ms = 0;
	}
}

void can_master_start_pdo(CAN_master *p_cm, const uint64_t timestamp) {
	(void) timestamp;
	co_send_sync(p_cm);
	for (uint32_t i = 0; i < p_cm->slave_num; i++) {
		if (p_cm->slaves[i]->con_state == CO_SLAVE_CON_ST_CONNECTED) {
			co_send_sync(p_cm);
			return;
		}
	}
}


void can_master_start_assign_slave(CAN_master *p_cm, CO_Slave *slave,
		const uint64_t timestamp) {
	co_slave_set_con_state(slave, CO_SLAVE_CON_ST_ASSIGNING);
	//can_master_disable_pdo(p_cm);
	p_cm->assigning_slave = slave;
	for (uint8_t i = 0; i < 32; i++) {
		p_cm->assigning_slave->sn[i] = 0;
	}
	p_cm->assign_timeout = timestamp + ASSIGN_TIMEOUT_mS;
	p_cm->assign_state = CM_ASSIGN_ST_WAIT_REQUEST;
	can_master_slave_deselect(p_cm,
			p_cm->assigning_slave->node_id - p_cm->slave_start_node_id);
}

void can_master_start_authorize_slave(CAN_master *p_cm,
		const uint64_t timestamp) {
	p_cm->assign_state = CM_ASSIGN_ST_PRE_AUTHORIZING;
	p_cm->assign_timeout = timestamp + 5*ASSIGN_TIMEOUT_mS;
}

void can_master_read_slave_sn(CAN_master *p_cm, uint8_t cab_id,
		const uint64_t timestamp) {
	(void)timestamp;
	co_sdo_read_object(p_cm, SLAVE_SERIAL_NUMBER_OBJECT_INDEX,
			p_cm->slaves[cab_id]->node_id, p_cm->slaves[cab_id]->sn,
			1000);
}
void can_master_read_mating_state(CAN_master *p_cm, uint8_t cab_id,
		const uint64_t timestamp) {
	(void)timestamp;
	co_sdo_read_object(p_cm, MATING_STATE_NUMBER_OBJECT_INDEX,
			p_cm->slaves[cab_id]->node_id, &p_cm->slaves[cab_id]->mating_st,
			100);
}
void can_master_read_vehicle_sn(CAN_master *p_cm, uint8_t cab_id,
		const uint64_t timestamp) {
	(void)timestamp;
	co_sdo_read_object(p_cm, VEHICLE_ID_NUMBER_OBJECT_INDEX,
			p_cm->slaves[cab_id]->node_id, p_cm->slaves[cab_id]->vehicle_sn,
			100);
}
void co_sdo_read_object(CAN_master *p_cm, const uint32_t mux,
		const uint32_t node_id, uint8_t *rx_buff, const uint64_t timeout) {
#if 0
	p_cm->sdo_server.timeout = timeout;
	p_cm->sdo_server.tx_address = CO_CAN_ID_TSDO + node_id;
	p_cm->sdo_server.rx_address = CO_CAN_ID_RSDO + node_id;
	p_cm->sdo_server.object_mux = mux;
	p_cm->sdo_server.buff_offset = 0;
	p_cm->sdo_server.rx_data_buff = rx_buff;
	p_cm->p_hw->can_tx.StdId = p_cm->sdo_server.tx_address;
	p_cm->p_hw->can_tx.DLC = 4;
	p_cm->p_hw->tx_data[0] = SDO_CS_INIT_READ;
	p_cm->p_hw->tx_data[1] = (uint8_t) ((p_cm->sdo_server.object_mux & 0x00ff0000) >> 16);
	p_cm->p_hw->tx_data[2] = (uint8_t) ((p_cm->sdo_server.object_mux & 0x0000ff00) >> 8);
	p_cm->p_hw->tx_data[3] = (uint8_t) ((p_cm->sdo_server.object_mux & 0x000000ff));
	can_send(p_cm->p_hw, p_cm->p_hw->tx_data);
	p_cm->sdo_server.state = SDO_ST_SENT;
#endif
	CO_Sub_Object tx_obj = { .p_data = rx_buff,	//<< Address variable receiving data
			.attr = ODA_SDO_RW,	//<< [skip] set ODA_SDO_RW
			.len = 32,			//<< data size
			.p_ext = NULL		//<< [skip] set NULL
	};
	CO_SDOclient_start_upload(&p_cm->co_app.sdo_client, node_id, (mux >> 8),
			(uint8_t) mux, &tx_obj, timeout);

}

void co_sdo_write_object(CAN_master *p_cm, const uint32_t mux,
		const uint32_t node_id, uint8_t *tx_buff, const uint32_t len,
		const uint64_t timeout) {
#if 0
	p_cm->sdo_server.timeout = timeout;
	p_cm->sdo_server.tx_address = CO_CAN_ID_TSDO + node_id;
	p_cm->sdo_server.rx_address = CO_CAN_ID_RSDO + node_id;
	p_cm->sdo_server.object_mux = mux;
	p_cm->sdo_server.buff_offset = 0;
	p_cm->sdo_server.object_data_len = len;
	p_cm->sdo_server.tx_data_buff = tx_buff;
	p_cm->p_hw->can_tx.StdId = p_cm->sdo_server.tx_address;
	p_cm->p_hw->can_tx.DLC = 4;
	p_cm->p_hw->tx_data[0] = SDO_CS_INIT_WRITE;
	p_cm->p_hw->tx_data[1] = (uint8_t) ((p_cm->sdo_server.object_mux & 0x00ff0000) >> 16);
	p_cm->p_hw->tx_data[2] = (uint8_t) ((p_cm->sdo_server.object_mux & 0x0000ff00) >> 8);
	p_cm->p_hw->tx_data[3] = (uint8_t) ((p_cm->sdo_server.object_mux & 0x000000ff));
	can_send(p_cm->p_hw, p_cm->p_hw->tx_data);
	p_cm->sdo_server.state = SDO_ST_SENT;
#endif
	CO_Sub_Object tx_obj = { .p_data = tx_buff,	//<< Address variable receiving data
			.attr = ODA_SDO_RW,	//<< [skip] set ODA_SDO_RW
			.len = len,			//<< data size
			.p_ext = NULL		//<< [skip] set NULL
	};
	CO_SDOclient_start_download(&p_cm->co_app.sdo_client, node_id, (mux >> 8),
			mux, &tx_obj, 1000);

}

void can_master_update_id_assign_process(CAN_master *p_cm,
		const uint64_t timestamp) {
	switch (p_cm->assign_state) {
	case CM_ASSIGN_ST_WAIT_REQUEST:
	case CM_ASSIGN_ST_SLAVE_SELECT:
	case CM_ASSIGN_ST_WAIT_CONFIRM:
		/* timeout*/
		if ((p_cm->assign_timeout <= timestamp)
				&& (p_cm->assign_timeout != 0)) {
			p_cm->assign_state = CM_ASSIGN_ST_FAIL;
		}
		break;
	case CM_ASSIGN_ST_START:
		p_cm->assign_state = CM_ASSIGN_ST_SLAVE_SELECT;
		can_master_slave_select(p_cm,
				p_cm->assigning_slave->node_id - p_cm->slave_start_node_id);
		break;
	case CM_ASSIGN_ST_SLAVE_SELECT_CONFIRM:
		p_cm->assign_state = CM_ASSIGN_ST_WAIT_CONFIRM;
		p_cm->p_hw->can_tx.StdId = p_cm->node_id_scan_cobid;
		p_cm->p_hw->can_tx.DLC = 1;
		p_cm->p_hw->tx_data[0] = p_cm->assigning_slave->node_id;
		can_send(p_cm->p_hw, p_cm->p_hw->tx_data);
		p_cm->assign_timeout = timestamp + ASSIGN_TIMEOUT_mS;
		break;
	case CM_ASSIGN_ST_PRE_AUTHORIZING:
		/* timeout*/
		if ((p_cm->assign_timeout <= timestamp)&& (p_cm->assign_timeout != 0)) {
			p_cm->assign_state = CM_ASSIGN_ST_FAIL;
			delay_nodeID = 0;
			if (CO_SDO_get_status(&p_cm->co_app.sdo_client)== CO_SDO_RT_abort) {
				CO_SDO_reset_status(&p_cm->co_app.sdo_client);
			}
		}
		can_master_slave_deselect(p_cm, can_master_get_assign_slave_id(p_cm));
		if(delay_nodeID ++ < CNT_50ms ){
			return;
		}
		delay_nodeID = 0;
		/* Delay between Set Node ID Pin High and Send read SN msg */

		if (CO_SDO_get_status(&p_cm->co_app.sdo_client) != CO_SDO_RT_idle)
			return;
		can_master_read_slave_sn(p_cm, can_master_get_assign_slave_id(p_cm),
				timestamp);
		p_cm->assign_state = CM_ASSIGN_ST_AUTHORIZING;
		p_cm->assign_timeout = timestamp + 10 * ASSIGN_TIMEOUT_mS;
		p_cm->co_service = CM_CO_AUTHORIZING;
		break;
	case CM_ASSIGN_ST_AUTHORIZING:
		/* timeout*/
		if ((p_cm->assign_timeout <= timestamp)&& (p_cm->assign_timeout != 0)) {
			p_cm->assign_state = CM_ASSIGN_ST_FAIL;
			if (CO_SDO_get_status(&p_cm->co_app.sdo_client)
					== CO_SDO_RT_abort) {

				CO_SDO_reset_status(&p_cm->co_app.sdo_client);

			}
		}
		if (CO_SDO_get_status(&p_cm->co_app.sdo_client) == CO_SDO_RT_abort
				&& p_cm->co_service == CM_CO_AUTHORIZING){

			CO_SDO_reset_status(&p_cm->co_app.sdo_client);
			p_cm->assign_state = CM_ASSIGN_ST_FAIL;

		}
		else if (CO_SDO_get_status(&p_cm->co_app.sdo_client)
				== CO_SDO_RT_success && p_cm->co_service == CM_CO_AUTHORIZING){
			CO_SDO_reset_status(&p_cm->co_app.sdo_client);
			can_master_read_vehicle_sn(p_cm, can_master_get_assign_slave_id(p_cm),
					timestamp);
			p_cm->assign_state = CM_ASSIGN_ST_AUTHORIZED;
			p_cm->assign_timeout = timestamp + 10 * ASSIGN_TIMEOUT_mS;
			p_cm->co_service = CM_CO_AUTHORIZING_VSN_READ;
		}
		break;
	case CM_ASSIGN_ST_AUTHORIZED:
		if ((p_cm->assign_timeout <= timestamp)&& (p_cm->assign_timeout != 0)) {
			p_cm->assign_state = CM_ASSIGN_ST_FAIL;
			if (CO_SDO_get_status(&p_cm->co_app.sdo_client)
					== CO_SDO_RT_abort) {

				CO_SDO_reset_status(&p_cm->co_app.sdo_client);
			}
		}
		if (CO_SDO_get_status(&p_cm->co_app.sdo_client) == CO_SDO_RT_abort
				&& p_cm->co_service == CM_CO_AUTHORIZING_VSN_READ){

			CO_SDO_reset_status(&p_cm->co_app.sdo_client);
			p_cm->assign_state = CM_ASSIGN_ST_FAIL;
			//p_cm->on_slave_assign_success(p_cm, timestamp);

		}
		else if (CO_SDO_get_status(&p_cm->co_app.sdo_client)
				== CO_SDO_RT_success && p_cm->co_service == CM_CO_AUTHORIZING_VSN_READ){
			CO_SDO_reset_status(&p_cm->co_app.sdo_client);
			p_cm->on_slave_assign_success(p_cm, timestamp);
		}
		break;
	case CM_ASSIGN_ST_REASSIGN:
		/* timeout*/
		if ((p_cm->assign_timeout <= timestamp)&& (p_cm->assign_timeout != 0)) {
			p_cm->assign_state = CM_ASSIGN_ST_FAIL;
			if (CO_SDO_get_status(&p_cm->co_app.sdo_client)
					== CO_SDO_RT_abort) {
				CO_SDO_reset_status(&p_cm->co_app.sdo_client);
			}
		}
		if (CO_SDO_get_status(&p_cm->co_app.sdo_client) == CO_SDO_RT_abort
				&& p_cm->co_service == CM_CO_REASSIGN) {

			CO_SDO_reset_status(&p_cm->co_app.sdo_client);
			can_master_reset_node_id_num(p_cm);
			p_cm->assign_state = CM_ASSIGN_ST_FAIL;
		}

		else if (CO_SDO_get_status(&p_cm->co_app.sdo_client)
				== CO_SDO_RT_success && p_cm->co_service == CM_CO_REASSIGN) {

			CO_SDO_reset_status(&p_cm->co_app.sdo_client);
			p_cm->assign_state = CM_ASSIGN_ST_DONE;
			p_cm->on_slave_reassign_success(p_cm, timestamp);

		}
		break;
	case CM_ASSIGN_ST_DONE:
		break;
	case CM_ASSIGN_ST_FAIL:
		p_cm->on_slave_assign_fail(p_cm, timestamp);
		break;
	}

}

static void can_master_process_sdo(CAN_master *p_cm, const uint32_t timestamp) {
	(void) timestamp;
	if ((p_cm->sdo_server.rx_data_buff == NULL)
			&& (p_cm->sdo_server.tx_data_buff == NULL))
		return;
	uint8_t cs = p_cm->sdo_server.rx_msg_data[0];
	uint32_t mux;
	uint32_t dlc = p_cm->sdo_server.can_rx_buff.DLC;
	uint32_t seg_len = 0;
	switch (cs) {
	case SDO_CS_INIT_READ:
		mux = (((uint32_t) (p_cm->sdo_server.rx_msg_data[1])) << 16)
		+ (((uint32_t) (p_cm->sdo_server.rx_msg_data[2])) << 8)
		+ (uint32_t) (p_cm->sdo_server.rx_msg_data[3]);
		p_cm->sdo_server.object_data_len =
				(uint32_t) (p_cm->sdo_server.rx_msg_data[7])
				+ (((uint32_t) (p_cm->sdo_server.rx_msg_data[6])) << 8)
				+ (((uint32_t) (p_cm->sdo_server.rx_msg_data[5])) << 16)
				+ (((uint32_t) (p_cm->sdo_server.rx_msg_data[4])) << 24);

		if (mux != p_cm->sdo_server.object_mux) {
			p_cm->sdo_server.state = SDO_ST_FAIL;
			return;
		}
		p_cm->sdo_server.buff_offset = 0;
		p_cm->p_hw->tx_data[0] = SDO_CS_SEGMENT_WRITE;
		if (p_cm->sdo_server.object_data_len < p_cm->sdo_server.buff_offset)
			return;
		seg_len = p_cm->sdo_server.object_data_len
				- p_cm->sdo_server.buff_offset;
		if (seg_len > 7) {
			seg_len = 7;
		}
		p_cm->p_hw->can_tx.DLC = seg_len + 1;
		p_cm->p_hw->tx_data[0] = SDO_CS_SEGMENT_WRITE;
		CO_memcpy(p_cm->p_hw->tx_data + 1,
				p_cm->sdo_server.tx_data_buff + p_cm->sdo_server.buff_offset,
				seg_len);
		p_cm->sdo_server.buff_offset += seg_len;
		can_send(p_cm->p_hw, p_cm->p_hw->tx_data);
		p_cm->sdo_server.is_new_msg = 0;
		break;
	case SDO_CS_SEGMENT_READ:
		p_cm->p_hw->tx_data[0] = SDO_CS_SEGMENT_WRITE;
		if (p_cm->sdo_server.object_data_len < p_cm->sdo_server.buff_offset)
			return;
		seg_len = p_cm->sdo_server.object_data_len
				- p_cm->sdo_server.buff_offset;
		if (seg_len > 7) {
			seg_len = 7;
		}
		p_cm->p_hw->can_tx.DLC = seg_len + 1;
		p_cm->p_hw->tx_data[0] = SDO_CS_SEGMENT_WRITE;
		CO_memcpy(p_cm->p_hw->tx_data + 1,
				p_cm->sdo_server.tx_data_buff + p_cm->sdo_server.buff_offset,
				seg_len);
		p_cm->sdo_server.buff_offset += seg_len;
		can_send(p_cm->p_hw, p_cm->p_hw->tx_data);
		p_cm->sdo_server.is_new_msg = 0;
		break;
	case SDO_CS_FINISH_READ:
		p_cm->sdo_server.state = SDO_ST_SUCCESS;
		p_cm->sdo_server.timeout = 0;
		p_cm->sdo_server.is_new_msg = 0;
		break;
	case SDO_CS_INIT_WRITE:
		mux = (((uint32_t) (p_cm->sdo_server.rx_msg_data[1])) << 16)
		+ (((uint32_t) (p_cm->sdo_server.rx_msg_data[2])) << 8)
		+ (uint32_t) (p_cm->sdo_server.rx_msg_data[3]);
		if (mux != p_cm->sdo_server.object_mux) {
			p_cm->sdo_server.state = SDO_ST_FAIL;
			return;
		}
		p_cm->sdo_server.buff_offset = 0;
		p_cm->sdo_server.object_data_len = CO_getUint32(
				p_cm->sdo_server.rx_msg_data + 4);
		p_cm->p_hw->tx_data[0] = SDO_CS_SEGMENT_READ;
		can_send(p_cm->p_hw, p_cm->p_hw->tx_data);
		p_cm->sdo_server.is_new_msg = 0;
		break;
	case SDO_CS_SEGMENT_WRITE:
		CO_memcpy(p_cm->sdo_server.rx_data_buff + p_cm->sdo_server.buff_offset,
				p_cm->sdo_server.rx_msg_data + 1, 7);
		p_cm->sdo_server.buff_offset += 7;
		p_cm->p_hw->tx_data[0] = SDO_CS_SEGMENT_READ;
		can_send(p_cm->p_hw, p_cm->p_hw->tx_data);
		p_cm->sdo_server.is_new_msg = 0;
		break;
	case SDO_CS_FINISH_WRITE:
		if (dlc == 0)
			return;
		CO_memcpy(p_cm->sdo_server.rx_data_buff + p_cm->sdo_server.buff_offset,
				p_cm->sdo_server.rx_msg_data + 1, dlc - 1);
		p_cm->sdo_server.buff_offset += dlc - 1;
		if (p_cm->sdo_server.buff_offset != p_cm->sdo_server.object_data_len) {
			p_cm->sdo_server.state = SDO_ST_FAIL;
		} else {
			p_cm->sdo_server.state = SDO_ST_SUCCESS;
			p_cm->sdo_server.timeout = 0;
		}
		p_cm->sdo_server.is_new_msg = 0;
		break;
	case SDO_CS_ABORT:
		p_cm->sdo_server.state = SDO_ST_FAIL;
		p_cm->sdo_server.is_new_msg = 0;
		break;
	}
}

void can_master_start_assign_next_slave(CAN_master *p_cm,
		const uint64_t timestamp) {
	p_cm->assigning_slave = can_master_get_slave(p_cm,
			CO_SLAVE_CON_ST_ASSIGNING);
	if (p_cm->assigning_slave == NULL) {
		p_cm->assign_state = CM_ASSIGN_ST_DONE;
		return;
	}
	for (int i = 0; i < 32; i++) {
		p_cm->assigning_slave->sn[i] = 0;
	}
	p_cm->assign_timeout = timestamp + ASSIGN_TIMEOUT_mS;
	p_cm->assign_state = CM_ASSIGN_ST_WAIT_REQUEST;
	can_master_slave_deselect(p_cm,
			p_cm->assigning_slave->node_id - p_cm->slave_start_node_id);
}
/* reset BP scan loop */
void can_master_reset_polling_assign_slave(CAN_master *p_cm,
		const uint64_t timestamp) {
	if ((p_cm->assign_state != CM_ASSIGN_ST_DONE))
		return;
	if (cnt_reset_polling == 0) {
		for (uint8_t i = 0; i < p_cm->slave_num; i++) {
			if (p_cm->slaves[i]->con_state == CO_SLAVE_CON_ST_CONNECTED)
				continue;
			if (p_cm->slaves[i]->recovery_time_ms > 0) {
				p_cm->slaves[i]->recovery_time_ms--;
				continue;
			}
			can_master_slave_select(p_cm, i);
			p_cm->slaves[i]->con_state = CO_SLAVE_CON_ST_ASSIGNING;
			cnt_reset_polling = 1;
		}
		return;
	} else if (cnt_reset_polling >= 1) {
		cnt_reset_polling++;
		if (cnt_reset_polling < 10)
			return;
		cnt_reset_polling = 0;
		can_master_start_assign_next_slave(p_cm, timestamp);
	}

}

CO_Slave* can_master_get_slave(const CAN_master *const p_cm,
		CO_SLAVE_NET_STATE net_state) {
	for (uint32_t i = 0; i < p_cm->slave_num; i++) {
		if (p_cm->slaves[i]->con_state == net_state) {
			return p_cm->slaves[i];
		}
	}
	return NULL;
}

uint8_t can_master_get_assign_slave_id(const CAN_master *const p_cm) {
	for (uint8_t i = 0; i < p_cm->slave_num; i++) {
		if (p_cm->slaves[i] == p_cm->assigning_slave) {
			return i;
		}
	}
	return 0;
}

void can_master_reconfig_node_id_num(CAN_master *p_cm) {
	p_cm->slave_start_node_id = BP_START_NODE_ID_NUM;
	for (uint8_t i = 0; i < p_cm->slave_num; i++) {
		p_cm->slaves[i]->node_id = i + p_cm->slave_start_node_id;
	}
}

void can_master_reset_node_id_num(CAN_master *p_cm) {
	p_cm->slave_start_node_id = BP_DEFAULT_NODE_ID_NUM;
	for (uint8_t i = 0; i < p_cm->slave_num; i++) {
		p_cm->slaves[i]->node_id = p_cm->slave_start_node_id;
	}
}

void can_master_start_reassign_slave_id(CAN_master *p_cm,
		const uint64_t timestamp) {
	p_cm->assign_state = CM_ASSIGN_ST_REASSIGN;
	p_cm->assign_timeout = timestamp + 10 * ASSIGN_TIMEOUT_mS;
	if (sdo_server_get_state(&p_cm->sdo_server) != SDO_ST_IDLE)
		return;
	co_sdo_write_object(p_cm, SLAVE_ID_NUMBER_OBJECT_INDEX,
			BP_DEFAULT_NODE_ID_NUM, (uint8_t*) &p_cm->assigning_slave->node_id, 1,
			timestamp + REASSIGN_ID_SDO_TIMEOUT_mS);
}
