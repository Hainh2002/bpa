/*
 * app_co_od.c
 *
 *  Created on: Jun 17, 2022
 *      Author: Dakaka
 *
 *       Only edit in USER CODE 1,2  if you haven't read the documents or love simplicity!
 */

#include "app_co_od.h"
#include "CO.h"
#include "CO_OD.h"
#include "bs_app.h"
#include "CRC.h"

/*******************************************************************************
 * 		1.	Declare and define ALL sub-object in OD
 *******************************************************************************/

/*------------------------------------------------------------------------------------
 *  1.1 Declare and define all communication profile initializer sub-object
 *  		WARNING: Should not be adjust!									***/

CO_Sub_Object device_type[] = {
		{(void*)&od_comm_prof_init_data.x1000_device_type, ODA_SDO_R, 4, NULL}
};/* 0x1000 VAR Device type */

CO_Sub_Object error_reg[] = {
		{(void*)&CO_DEVICE.emcy.error_reg.std, ODA_SDO_R, 1, NULL}
};/* 0x1001 VAR Error register */

CO_Sub_Object status[] = {
		{(void*)&CO_DEVICE.status, ODA_SDO_R, 4, NULL}
}; /* 0x1002 VAR Manufacturer status register */

CO_Sub_Object error_field[]={
		{(void*)&CO_DEVICE.emcy.predefined_error_fields.error_no, ODA_SDO_R, 1, NULL},
		{(void*)&(CO_DEVICE.emcy.predefined_error_fields.error_code[0].w), ODA_SDO_R, 4, NULL},
		{(void*)&(CO_DEVICE.emcy.predefined_error_fields.error_code[1].w), ODA_SDO_R, 4, NULL},
		{(void*)&(CO_DEVICE.emcy.predefined_error_fields.error_code[2].w), ODA_SDO_R, 4, NULL},
		{(void*)&(CO_DEVICE.emcy.predefined_error_fields.error_code[3].w), ODA_SDO_R, 4, NULL},
		{(void*)&(CO_DEVICE.emcy.predefined_error_fields.error_code[4].w), ODA_SDO_R, 4, NULL},
		{(void*)&(CO_DEVICE.emcy.predefined_error_fields.error_code[5].w), ODA_SDO_R, 4, NULL},
		{(void*)&(CO_DEVICE.emcy.predefined_error_fields.error_code[6].w), ODA_SDO_R, 4, NULL},
		{(void*)&(CO_DEVICE.emcy.predefined_error_fields.error_code[7].w), ODA_SDO_R, 4, NULL}
}; /* 0x1003 ARRAY Error field */

CO_Sub_Object sync_cob_id[]={
		{(void*)&od_comm_prof_init_data.x1005_cob_id_sync_message, ODA_SDO_RW, 4, NULL}
}; /* 0x1005 VAR COB-ID Sync message */

CO_Sub_Object comm_cycle[]={
		{(void*)&od_comm_prof_init_data.x1006_comm_cycle_period_ms, ODA_SDO_RW, 2, NULL}
}; /* 0x1006 VAR Communication cycle period [ms] */

CO_Sub_Object sync_window_len[]={
		{(void*)&od_comm_prof_init_data.x1007_sync_window_length_ms, ODA_SDO_RW, 2, NULL}
}; /* 0x1007 VAR Synchronous window length */
CO_Sub_Object device_name[]={
		{(void*)&od_comm_prof_init_data.x1008_device_name, ODA_SDO_R, CO_CF_1008_DEVICE_NAME_SIZE, NULL}
}; /* 0x1008 VAR Manufacturer device name */

CO_Sub_Object hw_version[]={
		{(void*)&od_comm_prof_init_data.x1009_hardware_version, ODA_SDO_R, 4, NULL}
}; /* 0x1009 VAR Manufacturer hardware version */

CO_Sub_Object sw_version[]={
		{(void*)&od_comm_prof_init_data.x100A_software_version, ODA_SDO_R, 4, NULL}
}; /* 0x100A VAR Manufacturer software version */

CO_Sub_Object guard_time[]={
		{(void*)&od_comm_prof_init_data.x100C_guard_time_ms, ODA_SDO_RW, 2, NULL}
}; /* 0x100C VAR Guard time [ms] */

CO_Sub_Object life_time_factor[]={
		{(void*)&od_comm_prof_init_data.x100D_life_time_factor, ODA_SDO_RW, 1, NULL}
}; /* 0x100D VAR Life time factor */


CO_Sub_Object time_cob_id[]={
		{(void*)&od_comm_prof_init_data.x1012_cob_id_time_stamp_object, ODA_SDO_RW, 4, NULL}
}; /* 0x1012 VAR COB-ID TIME */

CO_Sub_Object high_res_timestamp[]={
		{(void*)&CO_DEVICE.high_resolution_timestamp, ODA_SDO_RW, 4, NULL}
}; /* 0x1013 VAR Guard time */

CO_Sub_Object emcy_cob_id[]={
		{(void*)&od_comm_prof_init_data.x1014_cob_id_emcy, ODA_SDO_RW, 4, NULL}
}; /* 0x1014 VAR COB-ID EMCY */

CO_Sub_Object consumer_heartbeat[]={
		{(void*)&od_comm_prof_init_data.x1016_consumer_heartbeat_time_sub0, ODA_SDO_R, 1, NULL},
		{(void*)&od_comm_prof_init_data.x1016_consumer_heartbeat_time[0], ODA_SDO_RW, 4, NULL},
		{(void*)&od_comm_prof_init_data.x1016_consumer_heartbeat_time[1], ODA_SDO_RW, 4, NULL},
		{(void*)&od_comm_prof_init_data.x1016_consumer_heartbeat_time[2], ODA_SDO_RW, 4, NULL},
		{(void*)&od_comm_prof_init_data.x1016_consumer_heartbeat_time[3], ODA_SDO_RW, 4, NULL},
		{(void*)&od_comm_prof_init_data.x1016_consumer_heartbeat_time[4], ODA_SDO_RW, 4, NULL}
}; /* 0x1016 Consumer heart-beat time */

CO_Sub_Object producer_heartbeat[]={
		{(void*)&od_comm_prof_init_data.x1017_producer_heartbeat_time, ODA_SDO_RW, 2, NULL}
}; /* 0x1017 Producer heart-beat time */

CO_Sub_Object identity[]={
		{(void*)&od_comm_prof_init_data.x1018_identity.vendor_id, ODA_SDO_RW, 4, NULL},
		{(void*)&od_comm_prof_init_data.x1018_identity.product_code, ODA_SDO_R, 4, NULL},
		{(void*)&od_comm_prof_init_data.x1018_identity.revision_number, ODA_SDO_R, 4, NULL},
		{(void*)&od_comm_prof_init_data.x1018_identity.serial_number, ODA_SDO_R, 4, NULL}
}; /* 0x1018 ARRAY Identity */

CO_Sub_Object sync_cter_over_flow_value[]={
		{(void*)&od_comm_prof_init_data.x1019_sync_cter_over_flow_value, ODA_SDO_RW, 2, NULL}
}; /* 0x1019 Synchronous counter overflow value [1-30000]*/

CO_Sub_Object sync_bit_pos_over_flow_value[]={
		{(void*)&od_comm_prof_init_data.x1030_sync_bit_pos_over_flow_value, ODA_SDO_RW, 1, NULL}
}; /* 0x1030 Synchronous bit position overflow value [0-31]*/

CO_Sub_Object sdo_server_comm_para[]={
		{(void*)&od_comm_prof_init_data.x1031_sdo_server_comm_para.node_id, ODA_SDO_RW, 1, NULL},
		{(void*)&od_comm_prof_init_data.x1031_sdo_server_comm_para.trans_type, ODA_SDO_RW, 2, NULL},
		{(void*)&od_comm_prof_init_data.x1031_sdo_server_comm_para.allow_timeout_ms, ODA_SDO_RW, 2, NULL}
}; /* 0x1031 SDO server communication parameter*/

CO_Sub_Object sdo_client_comm_para[]={
		{(void*)&od_comm_prof_init_data.x1032_sdo_client_comm_para.trans_type, ODA_SDO_RW, 2, NULL},
		{(void*)&od_comm_prof_init_data.x1032_sdo_client_comm_para.sync_mask_reg, ODA_SDO_RW, 4, NULL}
}; /* 0x1032 SDO client communication parameter*/



/*--------------------------------------------------------------------------------------------
 * 1.2 Declare and define all manufacturer-specific profile sub-object
 */

/******************************************************************************************************/

/* USER CODE 1 BEGIN */

/*CO_Sub_Object_Ext_Confirm_Func_t used to write a flag that requires firmware upgrade to flash */

uint8_t is_request_upgrade = 0;
static CO_Sub_Object_Ext_Confirm_Func_t sys_reset_upgrade_firmware(void) {
	if(is_request_upgrade != 1 ) return CO_EXT_CONFIRM_abort;
	is_request_upgrade = 0;
	selex_bs_app.is_request_upgrade = true;
	return CO_EXT_CONFIRM_success;

}
CO_Sub_Object_Ext_t new_firmware_ext = {
		.p_shadow_data = &is_request_upgrade,
		.confirm_func = sys_reset_upgrade_firmware
};

CO_Sub_Object new_firmware[] = {
		{(void*)NULL,  0, 1, NULL},
		{(void*)NULL,  ODA_SDO_RW, 1, &new_firmware_ext},

};
/******************************************************************************************************/


/******************************************************************************************************/
/*

 **data_state = 0 : Kết thúc quá trình upgrade
 * data_state = 1 : Upgrade PMU
 * data_state = 2 : Upgrade MC
 * data_state= 5: Upgrade BP1
 * data_state = 6 : Upgrade BP2
 * data_state = 7 : Upgrade BP3
 * data_state = 8 : Upgrade HMI
 *
 * */
uint8_t ota_command[5];
static CO_Sub_Object_Ext_Confirm_Func_t upgrade_fw_set_state_Confirm_Func(void){
	uint8_t data_state = ota_command[0];
	if(data_state == 0){
		bs_app_set_state(&selex_bs_app, PMU_ST_ACTIVE);
		CO_recover_sync_mode(&selex_bs_app.base.co_app,&od_comm_prof_init_data);
		return CO_EXT_CONFIRM_success;
	}
	/* upgrade PMU*/
	if(data_state == 1){
		selex_bs_app.is_request_upgrade = 1;
		bs_app_set_state(&selex_bs_app, PMU_ST_BOOT);
		selex_bs_app.bootState_timeout = 0;
		CO_disable_sync_mode(&selex_bs_app.base.co_app);
		return CO_EXT_CONFIRM_success;
	}
	/* upgrade BP*/
	if(		data_state == 5||
			data_state == 6||
			data_state == 7){

		bs_app_remove_working_port(&selex_bs_app, data_state - 4);
		selex_bs_app.bootState_timeout = 0;
		bs_app_set_state(&selex_bs_app, PMU_ST_BOOT);
		CO_disable_sync_mode(&selex_bs_app.base.co_app);
		return CO_EXT_CONFIRM_success;

	}

	return CO_EXT_CONFIRM_success;
}
CO_Sub_Object_Ext_t upgrade_fw_set_state_ext = {
		.p_shadow_data = ota_command,
		.confirm_func = upgrade_fw_set_state_Confirm_Func
};
CO_Sub_Object upgrade_fw_object[] = {
		{(void*)ota_command,  ODA_SDO_RW, 5 , &upgrade_fw_set_state_ext}
};

uint8_t fw_name_value[] = 	FRIMWARE_NAME;

CO_Sub_Object fw_name[]={
		{(void*)fw_name_value, ODA_SDO_R, sizeof(FRIMWARE_NAME), NULL}
};
/******************************************************************************************************/

/* USER CODE 1 END */

/*******************************************************************************************************
 * 			2. Declare and define OD
 *
 * 			Warning: Must sort the object so that the index value ascending from smallest to largest!
 *******************************************************************************************************/
static CO_Object smc_od_list[]=
{
		/*** Communication profile objects***/

		/** General communication object**/
		{.index = 0x1000, .sub_number = 1, .subs = device_type},		/* 0x1000 VAR Device type */
		{.index = 0x1001, .sub_number = 1, .subs = error_reg},			/* 0x1001 VAR Error register */
		{.index = 0x1002, .sub_number = 1, .subs = status},				/* 0x1002 VAR Manufacturer status register */
		{.index = 0x1003, .sub_number = 9, .subs = error_field}, 		/* 0x1003 ARRAY Error field */

		{.index = 0x1005, .sub_number = 1, .subs = sync_cob_id}, 		/* 0x1005 VAR COB-ID Sync message */
		{.index = 0x1006, .sub_number = 1, .subs = comm_cycle}, 		/* 0x1006 VAR Communication cycle period [ms] */
		{.index = 0x1007, .sub_number = 1, .subs = sync_window_len},	/* 0x1007 VAR Synchronous window length */
		{.index = 0x1008, .sub_number = 1, .subs = device_name}, 		/* 0x1008 VAR Manufacturer device name */
		{.index = 0x1009, .sub_number = 1, .subs = hw_version}, 		/* 0x1009 VAR Manufacturer hardware version */
		{.index = 0x100A, .sub_number = 1, .subs = sw_version}, 		/* 0x100A VAR Manufacturer software version */
		{.index = 0x100C, .sub_number = 1, .subs = guard_time}, 		/* 0x100C VAR Guard time [ms] */
		{.index = 0x100D, .sub_number = 1, .subs = life_time_factor},	/* 0x100D VAR Life time factor */
		{.index = 0x1012, .sub_number = 1, .subs = time_cob_id}, 		/* 0x100D VAR Time cob id */
		{.index = 0x1013, .sub_number = 1, .subs = high_res_timestamp},/* 0x1013 VAR Guard time */
		{.index = 0x1014, .sub_number = 1, .subs = emcy_cob_id}, 		/* VAR COB-ID emergency */

		{.index = 0x1016, .sub_number = 6, .subs = consumer_heartbeat},/* 0x1016 ARRAY Consumer heart-beat time */
		{.index = 0x1017, .sub_number = 1, .subs = producer_heartbeat},/* 0x1017 VAR Producer heart-beat time */
		{.index = 0x1018, .sub_number = 4, .subs = identity},			/* 0x1018 ARRAY Identity */
		{.index = 0x1019, .sub_number = 1, .subs = sync_cter_over_flow_value},	/* 0x1019 VAR Synchronous counter overflow value [0-30000]*/
		{.index = 0x1030, .sub_number = 1, .subs = sync_bit_pos_over_flow_value},	/* 0x1030 VAR Auxiliary synchronous counter overflow value [0-31]*/
		{.index = 0x1031, .sub_number = 3, .subs = sdo_server_comm_para},	/* 0x1031 ARRY SDO server communication parameter*/
		{.index = 0x1032, .sub_number = 2, .subs = sdo_client_comm_para},	/* 0x1032 ARRY SDO client communication parameter*/
		/** SDO parameter object [0x1200- 0x12FF] **/
		/** PDO parameter object [0x1400- 0x1BFF] **/

		/*** Manufacturer-specific profile object ***/

		/* USER CODE 2 BEGIN */
		{.index = SDO_HMI_OTA_INDEX, 		.sub_number = 1, 	.subs = upgrade_fw_object},
		{.index = 0x2300, 					.sub_number = 2, 	.subs = new_firmware},
		{.index = 0x5000, 					.sub_number = 1, 	.subs = fw_name},

		/* USER CODE 2 END */
};

static CO_OD smc_od =
{
		(sizeof(smc_od_list) / sizeof(smc_od_list[0])), smc_od_list
};

CO_OD *p_co_od = &smc_od;

