/*
 * app_co_comm_od.c
 *
 *  Created on: Jul 15, 2022
 *      Author: Dakaka
 *
 *      Don't edit if you haven't read the documents or love simplicity!
 */

#include "app_co_comm_od.h"
/*******************************************************************************************
 * Define and assign default value to some communication profile initializer data variable
 * 				WARNING:
 *				- Edit carefully !!!
 * *******************************************************************************************/


/* Bit for build synchronous mask register */
#define CO_BIT_0	0x00000001
#define CO_BIT_1	0x00000002
#define CO_BIT_2	0x00000004
#define CO_BIT_3	0x00000008
#define CO_BIT_4	0x00000010
#define CO_BIT_5	0x00000020
#define CO_BIT_6	0x00000040
#define CO_BIT_7	0x00000080
#define CO_BIT_8	0x00000100
#define CO_BIT_9	0x00000200
#define CO_BIT_10	0x00000400
#define CO_BIT_11	0x00000800
#define CO_BIT_12	0x00001000
#define CO_BIT_13	0x00002000
#define CO_BIT_14	0x00004000
#define CO_BIT_15	0x00008000
#define CO_BIT_16	0x00010000
#define CO_BIT_17	0x00020000
#define CO_BIT_18	0x00040000
#define CO_BIT_19	0x00080000
#define CO_BIT_20	0x00100000
#define CO_BIT_21	0x00200000
#define CO_BIT_22	0x00400000
#define CO_BIT_23	0x00800000
#define CO_BIT_24	0x01000000
#define CO_BIT_25	0x02000000
#define CO_BIT_26	0x04000000
#define CO_BIT_27	0x08000000
#define CO_BIT_28	0x10000000
#define CO_BIT_29	0x20000000
#define CO_BIT_30	0x40000000
#define CO_BIT_31	0x80000000

uint8_t device_name_init[] = 	CO_CF_1008_DEVICE_NAME	;

OD_Communication_Profile_Init_Data_t od_comm_prof_init_data =
{
    .x1000_device_type = 0x00000000,
    .x1005_cob_id_sync_message =
    {
    		.node_id = 0,
    		.func_code = 0x80 >> 7,
			.reverse1 = 0,
			.bit_frame = CO_BIT_CAN_FRAME_11BIT,
			.bit_30 = CO_CF_100500_SYNC_GEN,		//<< EDITABLE:
													// 1: CANopen device does not generate SYNC message
													// 0: CANopen device generate SYNC message
			.valid = CO_CF_100500_SYNC_ENABLE		//<< EDITABLE:

    },

    .x1006_comm_cycle_period_ms = CO_CF_1006_COMM_CYCLE_PERIOD_MS, 		//<< EDITABLE: Adjust macro
    .x1007_sync_window_length_ms = CO_CF_1007_SYNC_WINDO_LENGTH_MS,		//<< EDITABLE: Adjust macro

	.x1008_device_name = device_name_init,

	.x1009_hardware_version = HW_VERSION,			//<< EDITABLE
	.x100A_software_version = SW_VERSION,			//<< EDITABLE

	.x100C_guard_time_ms = 0,						//<< EDITABLE, not supported yet!
	.x100D_life_time_factor = 0,					//<< EDITABLE, not supported yet!

	.x1012_cob_id_time_stamp_object = 0x00000000,
    .x1014_cob_id_emcy =					//<< Unsupported function
    {
    		.node_id = NODE_ID_DEFAULT,
			.func_code = 0x80 >> 7,
			.reverse1 = 0,					//<<
			.bit_frame = CO_BIT_CAN_FRAME_11BIT,
			.bit_30 = 0,
			.valid = CO_EMCY_NOT_VALID		//<< EDITABLE: not supported yet!
											// 0: EMCY exists / is valid
											// 1: EMCY does not exist / is not valid
    },
    .x1015_inhibit_time_emcy = 0x0000,
    .x1016_consumer_heartbeat_time_sub0 = 0x08,
    .x1016_consumer_heartbeat_time = {0x00000000},
    .x1017_producer_heartbeat_time = 0x0000,
    .x1018_identity = {
        .vendor_id = VENDER_ID,				//<< EDITABLE: Adjust macro
        .product_code = PRODUCT_CODE,		//<< EDITABLE: Adjust macro
        .revision_number = REVISION_NUMBER,	//<< EDITABLE: Adjust macro
        .serial_number = SERIAL_NUMBER		//<< EDITABLE: Adjust macro
    },
    .x1019_sync_cter_over_flow_value = CO_CF_1019_SYNC_CTER_OVER_FLOW_VALUE,	//<< EDITABLE: Adjust macro
	.x1030_sync_bit_pos_over_flow_value = CO_CF_1030_SYNC_BIT_POS_OVER_FLOW_VALUE,//<< EDITABLE: Adjust macro

	.x1031_sdo_server_comm_para = {
		.node_id = NODE_ID_DEFAULT,
		.trans_type = CO_SDO_SERVER_TRANSM_TYPE_SYNC,				//<< EDITABLE:
		.allow_timeout_ms = CO_CF_103101_SDO_SERV_TIMEOUT_MS,		//<< EDITABLE:
	},
	.x1032_sdo_client_comm_para = {
		.sync_mask_reg = CO_CF_103201_SDO_CLI_SYNC_MASK_REGISTER,	//<< EDITABLE:
		.trans_type = CO_SDO_CLIENT_TRANSM_TYPE_SYNC_CYCLIC(CO_CF_103200_SDO_CLI_SYNC_TRANS_TYPE), //<< EDITABLE:
	},

//    .x1280_SDOClientParameter = {
//        .COB_IDClientToServerTx.cob_id = 0x80000000,	//not supported yet!
//        .COB_IDServerToClientRx.cob_id = 0x80000000,	//not supported yet!
//        .node_IDOfTheSDOServer = 0x01					//not supported yet!
//    },
    /* RPDO1 Communication Parameter*/
	.x140x_rpdo_comm_para[0] = {
		.id = {
				.node_id = 0,
				.func_code = 0,
				.reverse1 = 0,
				.bit_frame = CO_BIT_CAN_FRAME_11BIT,
				.bit_30 = 0,
				.valid = CO_PDO_NOT_VALID
		},
		.transmission_type = 0,
		.max_inhibit_type_ms = 0,
		.compatibility_entry = 0,
		.max_event_time_ms = 0,
		.sync_start_value = 0
	},
    /* RPDO2 Communication Parameter*/
	.x140x_rpdo_comm_para[1] = {
		.id = {
				.node_id = 0,
				.func_code = 0,
				.reverse1 = 0,
				.bit_frame = CO_BIT_CAN_FRAME_11BIT,
				.bit_30 = 0,
				.valid = CO_PDO_NOT_VALID
		},
		.transmission_type = 0,
		.max_inhibit_type_ms = 0,
		.compatibility_entry = 0,
		.max_event_time_ms = 0,
		.sync_start_value = 0
	},
    /* RPDO3 Communication Parameter*/
	.x140x_rpdo_comm_para[2] = {
		.id = {
				.node_id = 0,
				.func_code = 0,
				.reverse1 = 0,
				.bit_frame = CO_BIT_CAN_FRAME_11BIT,
				.bit_30 = 0,
				.valid = CO_PDO_NOT_VALID
		},
		.transmission_type = 0,
		.max_inhibit_type_ms = 0,
		.compatibility_entry = 0,
		.max_event_time_ms = 0,
		.sync_start_value = 0
	},
    /* RPDO4 Communication Parameter*/
	.x140x_rpdo_comm_para[3] = {
		.id = {
				.node_id = 0,
				.func_code = 0,
				.reverse1 = 0,
				.bit_frame = CO_BIT_CAN_FRAME_11BIT,
				.bit_30 = 0,
				.valid = CO_PDO_NOT_VALID
		},
		.transmission_type = 0,
		.max_inhibit_type_ms = 0,
		.compatibility_entry = 0,
		.max_event_time_ms = 0,
		.sync_start_value = 0
	},
    /* RPDO1 Mapping Parameter*/
	.x160x_rpdo_map_para[0] = {
		.object_number = 0x00,
		.app_object[0].w	= 0x00000000,
		.app_object[1].w	= 0x00000000,
		.app_object[2].w	= 0x00000000,
		.app_object[3].w	= 0x00000000,
		.app_object[4].w	= 0x00000000,
		.app_object[5].w	= 0x00000000,
		.app_object[6].w	= 0x00000000,
		.app_object[7].w	= 0x00000000
	},
    /* RPDO2 Mapping Parameter*/
	.x160x_rpdo_map_para[1] = {
		.object_number = 0x00,
		.app_object[0].w	= 0x00000000,
		.app_object[1].w	= 0x00000000,
		.app_object[2].w	= 0x00000000,
		.app_object[3].w	= 0x00000000,
		.app_object[4].w	= 0x00000000,
		.app_object[5].w	= 0x00000000,
		.app_object[6].w	= 0x00000000,
		.app_object[7].w	= 0x00000000
	},
    /* RPDO3 Mapping Parameter*/
	.x160x_rpdo_map_para[2] = {
		.object_number = 0x00,
		.app_object[0].w	= 0x00000000,
		.app_object[1].w	= 0x00000000,
		.app_object[2].w	= 0x00000000,
		.app_object[3].w	= 0x00000000,
		.app_object[4].w	= 0x00000000,
		.app_object[5].w	= 0x00000000,
		.app_object[6].w	= 0x00000000,
		.app_object[7].w	= 0x00000000
	},
    /* RPDO4 Mapping Parameter*/
	.x160x_rpdo_map_para[3] = {
		.object_number = 0x00,
		.app_object[0].w	= 0x00000000,
		.app_object[1].w	= 0x00000000,
		.app_object[2].w	= 0x00000000,
		.app_object[3].w	= 0x00000000,
		.app_object[4].w	= 0x00000000,
		.app_object[5].w	= 0x00000000,
		.app_object[6].w	= 0x00000000,
		.app_object[7].w	= 0x00000000
	},
    /* TPDO1 Communication Parameter*/
	.x180x_tpdo_comm_para[0] = {
		.id = {
			.node_id = NODE_ID_DEFAULT ,
			.func_code =  0x00000180 >>7,
			.reverse1 = 0,
			.bit_frame = CO_BIT_CAN_FRAME_11BIT,
			.bit_30 = CO_BIT_RTR_NOT_ALLOW,
			.valid = CO_CF_180000_TPDO1_ENABLE			//<< EDITABLE:
		},
		.transmission_type = CO_TPDO_TRANSM_TYPE_SYNC_CYCLIC(CO_CF_180001_TPDO1_TRANS_TYPE),				//<< EDITABLE:
												// type: x = 1-30000 (cyclic every x-st SYNC )
												// or 0xFFF (event-driven)
												// or 0 (acyclic)
		.max_inhibit_type_ms = 1000,			//<< EDITABLE: adjusted when setting TPDO in event mode
		.max_event_time_ms = 10000,				//<< EDITABLE: adjusted when setting TPDO in event mode
		.sync_start_value = 0,					//<< ignored
		.sync_mask_reg = CO_CF_180005_TPDO1_SYNC_MASK_REGISTER		//<< EDITABLE: Adjust macro
	},
    /* TPDO2 Communication Parameter*/
	.x180x_tpdo_comm_para[1] = {				//<< EDITABLE:
		.id = {
			.node_id = NODE_ID_DEFAULT,
			.func_code = 0x00000280 >> 7,
			.reverse1 = 0,
			.bit_frame = CO_BIT_CAN_FRAME_11BIT,
			.bit_30 = CO_BIT_RTR_NOT_ALLOW,
			.valid = CO_CF_180100_TPDO2_ENABLE	//<< EDITABLE:
		},
		.transmission_type = CO_TPDO_TRANSM_TYPE_SYNC_CYCLIC(CO_CF_180101_TPDO2_TRANS_TYPE),				//<< EDITABLE:
												// type: x = 1-30000 (cyclic every x-st SYNC )
												// or 0xFFF (event-driven)
												// or 0 (acyclic)
		.max_inhibit_type_ms = 1000,			//<< EDITABLE: adjusted when setting TPDO in event mode
		.max_event_time_ms = 10000,				//<< EDITABLE: adjusted when setting TPDO in event mode
		.sync_start_value = 0,					//<< ignored
		.sync_mask_reg = CO_CF_180105_TPDO2_SYNC_MASK_REGISTER		//<< EDITABLE: Adjust macro
	},
    /* TPDO3 Communication Parameter*/
	.x180x_tpdo_comm_para[2] = {				//<< EDITABLE:
		.id = {
			.node_id = NODE_ID_DEFAULT ,
			.func_code =  0x00000380 >>7,
			.reverse1 = 0,
			.bit_frame = CO_BIT_CAN_FRAME_11BIT,
			.bit_30 = CO_BIT_RTR_NOT_ALLOW,
			.valid = CO_CF_180200_TPDO3_ENABLE				//<< EDITABLE:
		},
		.transmission_type = CO_TPDO_TRANSM_TYPE_SYNC_CYCLIC(CO_CF_180201_TPDO3_TRANS_TYPE),				//<< EDITABLE:
												// type: x = 1-30000 (cyclic every x-st SYNC )
												// or 0xFFF (event-driven)
												// or 0 (acyclic)
		.max_inhibit_type_ms = 1000,			//<< EDITABLE: adjusted when setting TPDO in event mode
		.max_event_time_ms = 10000,				//<< EDITABLE: adjusted when setting TPDO in event mode
		.sync_start_value = 0,					//<< ignored
		.sync_mask_reg = CO_CF_180205_TPDO3_SYNC_MASK_REGISTER	//<< EDITABLE: Adjust macro
	},
    /* TPDO4 Communication Parameter*/
	.x180x_tpdo_comm_para[3] = {				//<< EDITABLE:
		.id = {
			.node_id = NODE_ID_DEFAULT ,
			.func_code =  0x00000480 >>7,
			.reverse1 = 0,
			.bit_frame = CO_BIT_CAN_FRAME_11BIT,
			.bit_30 = CO_BIT_RTR_NOT_ALLOW,
			.valid = CO_CF_180300_TPDO4_ENABLE				//<< EDITABLE:
		},
		.transmission_type = CO_TPDO_TRANSM_TYPE_SYNC_CYCLIC(CO_CF_180301_TPDO4_TRANS_TYPE),				//<< EDITABLE:
												// type: x = 1-30000 (cyclic every x-st SYNC )
												// or 0xFFF (event-driven)
												// or 0 (acyclic)
		.max_inhibit_type_ms = 1000,			//<< EDITABLE: adjusted when setting TPDO in event mode
		.max_event_time_ms = 10000,				//<< EDITABLE: adjusted when setting TPDO in event mode
		.sync_start_value = 0,					//<< ignored
		.sync_mask_reg = CO_CF_180305_TPDO4_SYNC_MASK_REGISTER	//<< EDITABLE: Adjust macro
	},
    /* TPDO1 Mapping Parameter */
	.x1A0x_tpdo_map_para[0] = {			//<< EDITABLE:
		.object_number	= 0x00,					//<< number of mapped application objects in PDO
		.app_object[0].w	= 0x00000000,			//<< application object 1st
		.app_object[1].w	= 0x00000000,			//<< application object 2st
		.app_object[2].w	= 0x00000000,			//<< application object 3st
		.app_object[3].w	= 0x00000000,			//<< application object 4st
		.app_object[4].w	= 0x00000000,			//<< application object 5st
		.app_object[5].w	= 0x00000000,			//<< application object 6st
		.app_object[6].w	= 0x00000000,			//<< application object 7st
		.app_object[7].w	= 0x00000000			//<< application object 8st
	},

    /* TPDO2 Mapping Parameter */
	.x1A0x_tpdo_map_para[1] = {			//<< EDITABLE:
		.object_number	= 0x00,					//<< number of mapped application objects in PDO
		.app_object[0].w	= 0x00000000,			//<< application object 1st
		.app_object[1].w	= 0x00000000,			//<< application object 2st
		.app_object[2].w	= 0x00000000,			//<< application object 3st
		.app_object[3].w	= 0x00000000,			//<< application object 4st
		.app_object[4].w	= 0x00000000,			//<< application object 5st
		.app_object[5].w	= 0x00000000,			//<< application object 6st
		.app_object[6].w	= 0x00000000,			//<< application object 7st
		.app_object[7].w	= 0x00000000			//<< application object 8st
	},
    /* TPDO3 Mapping Parameter */
	.x1A0x_tpdo_map_para[2] = {			//<< EDITABLE:
		.object_number	= 0x00,					//<< number of mapped application objects in PDO
		.app_object[0].w	= 0x00000000,			//<< application object 1st
		.app_object[1].w	= 0x00000000,			//<< application object 2st
		.app_object[2].w	= 0x00000000,			//<< application object 3st
		.app_object[3].w	= 0x00000000,			//<< application object 4st
		.app_object[4].w	= 0x00000000,			//<< application object 5st
		.app_object[5].w	= 0x00000000,			//<< application object 6st
		.app_object[6].w	= 0x00000000,			//<< application object 7st
		.app_object[7].w	= 0x00000000			//<< application object 8st
	},
    /* TPDO4 Mapping Parameter */
	.x1A0x_tpdo_map_para[3] = {			//<< EDITABLE:
		.object_number	= 0x00,					//<< number of mapped application objects in PDO
		.app_object[0].w	= 0x00000000,			//<< application object 1st
		.app_object[1].w	= 0x00000000,			//<< application object 2st
		.app_object[2].w	= 0x00000000,			//<< application object 3st
		.app_object[3].w	= 0x00000000,			//<< application object 4st
		.app_object[4].w	= 0x00000000,			//<< application object 5st
		.app_object[5].w	= 0x00000000,			//<< application object 6st
		.app_object[6].w	= 0x00000000,			//<< application object 7st
		.app_object[7].w	= 0x00000000			//<< application object 8st
	},
};

