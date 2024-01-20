/*
 * can_master.h
 *
 *  Created on: Apr 1, 2021
 *      Author: KhanhDinh
 */

#ifndef SERVICE_CAN_MASTER_CAN_MASTER_H_
#define SERVICE_CAN_MASTER_CAN_MASTER_H_

#include "stdlib.h"
#include "stdint.h"
#include "CO.h"
#include "can_hal.h"
#include "app_config.h"
#include "delay.h"


#define BMS_MAINSWITCH_SDO_TIMEOUT_mS					500
#define CONFIG_DRIVE_MODE_SDO_TIMEOUT_mS				500
#define CONFIG_PROTECT_MOTOR_SDO_TIMEOUT_mS				500

#define HMI_READ_BP_SN_COBID							0x90
#define HMI_TRANS_APP_UPDATE							0x91
#define DRIVE_MODE_ECO_SPORT							0x2025
#define DRIVE_MODE_LOCK_MOTOR							0x2026
#define BMS_MAINSWITCH_INDEX							0x200301

#define CAN_NODE_ID_ASSIGN_COBID						0x70
#define SYNC_BP_DATA_COBID								0x80
#define SLAVE_SERIAL_NUMBER_OBJECT_INDEX				0x200300
#define SLAVE_ID_NUMBER_OBJECT_INDEX					0x200302
#define VEHICLE_ID_NUMBER_OBJECT_INDEX					0x200401
#define MATING_STATE_NUMBER_OBJECT_INDEX				0x200400
#define MC_NODE_NUMBER_OBJECT_INDEX						0x100800

#define	SDO_MUX_HMI_FIND_VEHICLES						0x200200
#define	SDO_MUX_HMI_DISABLE_VEHICLES					0x200201
#define	SDO_MUX_HMI_HORN_VEHICLES						0x200202
#define	NEW_FIRMWARE_REQUEST							0x200101

#define SLAVE_RPDO1										0x200
#define SDO_RX_BUFFER_SIZE                 				(32UL)
typedef struct CO_SDO_SERVER_t CO_SDO_SERVER;

#define BP_VOL_CUR_TPDO_COBID                           CO_CAN_ID_TPDO_1
#define BP_1_4CELLS_VOL_TPDO_COBID                     	CO_CAN_ID_TPDO_2
#define BP_5_8CELLS_VOL_TPDO_COBID                    	CO_CAN_ID_TPDO_3
#define BP_TEMP_TPDO_COBID                              CO_CAN_ID_TPDO_4
#define BP_9_12CELLS_VOL_TPDO_COBID                     CO_CAN_ID_RPDO_1
#define BP_13_16CELLS_VOL_TPDO_COBID                    CO_CAN_ID_RPDO_2
typedef enum{
//     CO_CAN_ID_NMT_SERVICE       = 0x000,   /**< 0x000, Network management */
//     CO_CAN_ID_SYNC              = 0x080,   /**< 0x080, Synchronous message */
//     CO_CAN_ID_EMERGENCY         = 0x080,   /**< 0x080, Emergency messages (+nodeID) */
//     CO_CAN_ID_TIME_STAMP        = 0x100,   /**< 0x100, Time stamp message */
//     CO_CAN_ID_TPDO_1            = 0x180,   /**< 0x180, Default TPDO1 (+nodeID) */
//     CO_CAN_ID_RPDO_1            = 0x200,   /**< 0x200, Default RPDO1 (+nodeID) */
//     CO_CAN_ID_TPDO_2            = 0x280,   /**< 0x280, Default TPDO2 (+nodeID) */
//     CO_CAN_ID_RPDO_2            = 0x300,   /**< 0x300, Default RPDO2 (+nodeID) */
//     CO_CAN_ID_TPDO_3            = 0x380,   /**< 0x380, Default TPDO3 (+nodeID) */
//     CO_CAN_ID_RPDO_3            = 0x400,   /**< 0x400, Default RPDO3 (+nodeID) */
//     CO_CAN_ID_TPDO_4            = 0x480,   /**< 0x480, Default TPDO4 (+nodeID) */
//     CO_CAN_ID_RPDO_4            = 0x500,   /**< 0x500, Default RPDO5 (+nodeID) */
     CO_CAN_ID_TSDO              = 0x580,   /**< 0x580, SDO response from server (+nodeID) */
     CO_CAN_ID_RSDO              = 0x600,   /**< 0x600, SDO request from client (+nodeID) */
//     CO_CAN_ID_HEARTBEAT         = 0x700    /**< 0x700, Heartbeat message */
}CO_Default_ID;

typedef enum SDO_STATE_t{
	SDO_ST_IDLE 	= 0,
	SDO_ST_SENT		= 1,
	SDO_ST_SUCCESS	= 2,
	SDO_ST_FAIL		= 3
}SDO_STATE;
typedef enum {
	BUSCAN_ST_IDLE 	= 0,
	BUSCAN_ST_BUSY 	= 1,

}BUS_CAN_STATE;

struct CO_SDO_SERVER_t{

        uint32_t timeout;
        SDO_STATE state;
        uint32_t rx_index;
        uint32_t tx_index;
        uint32_t tx_address;
        uint32_t rx_address;
        uint32_t object_mux;
        uint8_t* rx_data_buff;
        uint8_t* tx_data_buff;
        CAN_RxHeaderTypeDef	can_rx_buff;
        CAN_RxHeaderTypeDef	can_tx_buff;
        uint8_t tx_msg_data[8];
        uint8_t rx_msg_data[8];
        uint32_t buff_offset;
        uint32_t object_data_len;
        uint8_t is_new_msg;
        uint8_t is_new_msg_boot_hmi;
};

typedef struct CO_ReadPDO_t CO_ReadPDO;
struct CO_ReadPDO_t{
	uint32_t cob_id;
    uint8_t is_new_msg;
};

typedef struct CAN_master_t CAN_master;

typedef void (*CAN_Master_Slave_Select)(const CAN_master*,const uint32_t);

typedef enum CM_ASSIGN_STATE{
    CM_ASSIGN_ST_WAIT_REQUEST,
	CM_ASSIGN_ST_START,
	CM_ASSIGN_ST_SLAVE_SELECT,
	CM_ASSIGN_ST_SLAVE_SELECT_CONFIRM,
	CM_ASSIGN_ST_WAIT_CONFIRM,
	CM_ASSIGN_ST_AUTHORIZING,
	CM_ASSIGN_ST_DONE,
	CM_ASSIGN_ST_FAIL,
	CM_ASSIGN_ST_REASSIGN,
	CM_ASSIGN_ST_PRE_AUTHORIZING,
	CM_ASSIGN_ST_AUTHORIZED

} CM_ASSIGN_STATE;

typedef enum CM_CO_SERVICE{
	CM_CO_MERGE_NEXT_WP = 0,
	CM_CO_TURN_OFF_WP,
	CM_CO_TURN_ON_WP,
	CM_CO_SOFTSTART_WP,
	CM_CO_TURN_ON_CHARGER_FET,
	CM_CO_TURN_OFF_ALL_FET,
	CM_CO_SOFTSTART_PS,
	CM_CO_REASSIGN,
	CM_CO_AUTHORIZING,
	CM_CO_AUTHORIZING_VSN_READ

} CM_CO_SERVICE;
typedef struct {
	bool new_msg;
	uint8_t data[8];
} MSG_buff;
typedef struct {
	MSG_buff BP_TPDO1;
	MSG_buff BP_TPDO2;
	MSG_buff BP_TPDO3;
	MSG_buff BP_TPDO4;
	MSG_buff BP_RPDO1;
	MSG_buff BP_RPDO2;
} BP_PDO;

typedef struct {
	MSG_buff MC_TPO1;
	MSG_buff MC_TPO2;
	MSG_buff MC_TPO3;
	BP_PDO BP_PDO[3];

} PDO_t;
struct CAN_master_t{
	uint8_t is_active;
	BUS_CAN_STATE BUS_CAN_state;
	uint32_t slave_num;
	uint32_t slave_start_node_id;
	uint32_t node_id_scan_cobid;
	CM_ASSIGN_STATE	assign_state;
	uint64_t assign_timeout;
	CO_Slave**		slaves;
	CO_Slave* assigning_slave;
	CO_Slave* authorizing_slave;
	uint16_t 		time_stamp;
	CO_SDO_SERVER 	sdo_server;
	CO_SDO_SERVER 	sdo_ser_boot;
	uint32_t pdo_sync_timestamp;
	CAN_Hw* p_hw;
	uint32_t cnt;
	uint8_t		hmi_find_vehicles;
	uint8_t		hmi_disable_vehicles;
	uint8_t		hmi_horn_ctrl;
	CAN_Master_Slave_Select slave_select;
	CAN_Master_Slave_Select slave_deselect;
	CO co_app;
	CM_CO_SERVICE co_service;
	PDO_t	PDO;
	bool 		is_new_seg_app_update;
	void (*on_slave_assign_success)(CAN_master* p_cm, const uint64_t timestamp);
	void (*on_slave_assign_fail)(CAN_master* p_cm, const uint64_t timestamp);
	void (*on_slave_reassign_success)(CAN_master* p_cm, const uint64_t timestamp);
	void (*pdo_process)( CAN_master*  p_cm);
	void (*update_pdo_handle)(CAN_master* p_cm, uint64_t timestamp);
};
void reset_buff_bp_pdo(BP_PDO *);
void can_master_init(CAN_master* p_cm,CO_Slave** slaves, const uint32_t slave_num,CAN_Hw* p_hw);
void can_master_process(CAN_master* p_cm,const uint64_t timestamp);
void can_master_start_assign_next_slave(CAN_master* p_cm, const uint64_t timestamp);
void pmu_co_process(CAN_master* p_cm,const uint64_t timestamp);
void can_master_update_id_assign_process(CAN_master* p_cm,const uint64_t timestamp);
void can_master_read_slave_sn(CAN_master* p_cm, uint8_t slave_id, const uint64_t timestamp);
void can_master_read_vehicle_sn(CAN_master* p_cm, uint8_t slave_id, const uint64_t timestamp);
void can_master_start_authorize_slave(CAN_master* p_cm, const uint64_t timestamp);
void co_sdo_read_object(CAN_master* p_cm,const uint32_t mux,const uint32_t node_id,uint8_t* rx_buff,const uint64_t timeout);
void co_sdo_write_object(CAN_master* p_cm,const uint32_t mux,const uint32_t node_id,
		uint8_t* tx_buff,const uint32_t len,const uint64_t timeout);
void can_master_start_assign_slave(CAN_master* p_cm, CO_Slave *slave, const uint64_t timestamp);
void can_master_disable_pdo(CAN_master* p_cm);
void can_master_start_pdo(CAN_master* p_cm, const uint64_t timestamp);
CO_Slave* can_master_get_slave(const CAN_master* const p_cm, CO_SLAVE_NET_STATE net_state);
void can_master_reset_polling_assign_slave(CAN_master* p_cm, const uint64_t timestamp);
uint8_t can_master_get_assign_slave_id(const CAN_master* const p_cm);
void co_send_sync(CAN_master *p_cm);
void can_master_start_reassign_slave_id(CAN_master* p_cm, const uint64_t timestamp);
void can_master_reconfig_node_id_num(CAN_master* p_cm);
void can_master_reset_node_id_num(CAN_master* p_cm);
void pmu_process_sdo(CAN_master *p_cm, const uint64_t timestamp) ;

static inline void can_master_slave_select(const CAN_master* p_cm, const uint32_t id){
	p_cm->slave_select(p_cm,id);
}

static inline void can_master_slave_deselect(const CAN_master* p_cm, const uint32_t id){
	p_cm->slave_deselect(p_cm,id);
}

static inline SDO_STATE sdo_server_get_state(const CO_SDO_SERVER* const p_svr){
	return p_svr->state;
}

static inline void sdo_server_set_state(CO_SDO_SERVER* p_svr, const SDO_STATE state){
	p_svr->state=state;
	if(state == SDO_ST_IDLE) p_svr->timeout = 0;
}

#endif /* SERVICE_CAN_MASTER_CAN_MASTER_H_ */
