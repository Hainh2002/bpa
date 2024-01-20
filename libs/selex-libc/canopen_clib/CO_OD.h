#ifndef CO_OD_H_
#define CO_OD_H_

#include "CO_Object.h"
#include "stdint.h"
#include "stdlib.h"
#include "CO_config.h"
#include "CO_CAN_Msg.h"

#define OD_UNSUPPORT_ENTRY	0xFFFFU

/****************************************************************
 *  Declare structure for CO identity Object */
/****************************************************************/
typedef struct{
         uint32_t vendor_id;
         uint32_t product_code;
         uint32_t revision_number;
         uint32_t serial_number;
}CO_OD_Identity_t;

/****************************************************************
 *  Declare structure for PDO Object */
/****************************************************************/
typedef union
{
	uint32_t w;
	struct
	{
		uint16_t	index;
		uint8_t		sub_index;
		uint8_t		len;			//<<unit: byte
	};
}CO_OD_PDO_Map_Para_App_Object_Register_t;

/**/
typedef struct
{
	uint8_t					object_number;					//sub-index = 0
	CO_OD_PDO_Map_Para_App_Object_Register_t app_object[8]; //sub-index = 1-8
} CO_OD_PDO_Map_Para_Object_t;

/****************************************************************
 *  Declare structure for TPDO Object */
/****************************************************************/
typedef struct
{
	co_id_t				id;					//sub-index = 0
	uint16_t			transmission_type;	//sub-index = 1
	uint16_t			max_inhibit_type_ms;//sub-index = 2
	uint16_t			max_event_time_ms;	//sub-index = 3
	uint8_t				sync_start_value;	//sub-index = 4
	uint32_t			sync_mask_reg;		//sub-index = 5
}CO_OD_TPDO_Comm_Para_Obj_t;

/****************************************************************
 *  Declare structure for RPDO Object */
/****************************************************************/
typedef struct
{
	co_id_t				id;					//sub-index = 0
	uint16_t			transmission_type;	//sub-index = 1
	uint16_t			max_inhibit_type_ms;//sub-index = 2
	uint8_t				compatibility_entry;//sub-index = 3
	uint16_t			max_event_time_ms;	//sub-index = 4
	uint8_t				sync_start_value;	//sub-index = 5
}CO_OD_RPDO_Comm_Para_Obj_t;

/****************************************************************
 *  Declare structure for SDO_server Object */
/****************************************************************/
typedef struct
{
	uint8_t				node_id;			//sub-index = 0
	uint16_t			trans_type;			//sub-index = 1
	uint16_t			allow_timeout_ms;	//sub-index = 2
}CO_OD_SDOserver_Comm_Para_Obj_t;

/****************************************************************
 *  Declare structure for SDO_client Object */
/****************************************************************/
typedef struct
{
	uint16_t			trans_type;			//sub-index = 0
	uint32_t			sync_mask_reg;		//sub-index = 3: Synchronous mask register
}CO_OD_SDOclient_Comm_Para_Obj_t;


/****************************************************************
 *  Declare some communication profile initializer data variable */
/****************************************************************/
typedef struct
{
    uint32_t x1000_device_type;
    co_id_t x1005_cob_id_sync_message;
    uint16_t x1006_comm_cycle_period_ms;
    uint16_t x1007_sync_window_length_ms;
    uint8_t	*x1008_device_name;
    uint32_t x1009_hardware_version;
    uint32_t x100A_software_version;
    uint16_t x100C_guard_time_ms;
    uint8_t x100D_life_time_factor;

    uint32_t x1012_cob_id_time_stamp_object;
    co_id_t x1014_cob_id_emcy;
    uint16_t x1015_inhibit_time_emcy;
    uint8_t x1016_consumer_heartbeat_time_sub0;
    uint32_t x1016_consumer_heartbeat_time[5];
    uint16_t x1017_producer_heartbeat_time;

    CO_OD_Identity_t x1018_identity;
    uint16_t x1019_sync_cter_over_flow_value;	/* 0x1019 Synchronous counter overflow value, set [1-30000]*/
    uint8_t x1030_sync_bit_pos_over_flow_value;	/* 0x1030 Auxiliary synchronous bit position overflow value, set [0-31]*/
    CO_OD_SDOserver_Comm_Para_Obj_t x1031_sdo_server_comm_para;
    CO_OD_SDOclient_Comm_Para_Obj_t x1032_sdo_client_comm_para;

//    struct {
//        co_id_t COB_IDClientToServerTx;
//        co_id_t COB_IDServerToClientRx;
//        uint8_t node_IDOfTheSDOServer;
//    } x1280_SDOClientParameter;

    /* RPDO1,2,3,4 Communication Parameter*/
    CO_OD_RPDO_Comm_Para_Obj_t x140x_rpdo_comm_para[4];

    /* RPDO1,2,3,4 Mapping Parameter*/
    CO_OD_PDO_Map_Para_Object_t x160x_rpdo_map_para[4];

    /* TPDO1,2,3,4 Communication Parameter*/
    CO_OD_TPDO_Comm_Para_Obj_t x180x_tpdo_comm_para[4];

    /* TPDO1,2,3,4 Mapping Parameter */
    CO_OD_PDO_Map_Para_Object_t x1A0x_tpdo_map_para[4];

}OD_Communication_Profile_Init_Data_t;

/****************************************************************
 *  Declare structure for OD class */
/****************************************************************/
typedef struct CO_OD_t CO_OD;
struct CO_OD_t
{
        uint16_t size;
        CO_Object* list;
};

//void CO_OD_write(CO_OD* p_od,const uint16_t entry,const uint8_t sub_index,const void* data,const uint8_t len);

uint16_t CO_OD_get_no(CO_OD* p_od,const uint16_t index);

static inline CO_Object* CO_OD_get_object_pointer(const CO_OD* p_od,
        const uint16_t od_list_no)
{
	if(od_list_no > p_od->size || od_list_no == OD_UNSUPPORT_ENTRY) return NULL;

	return &p_od->list[od_list_no];
}

static inline CO_Sub_Object* CO_OD_get_sub_object_pointer(const CO_OD* p_od,
        const uint16_t od_list_no,const uint8_t sub_index)
{
	if(od_list_no >= p_od->size || od_list_no == OD_UNSUPPORT_ENTRY
			|| p_od->list[od_list_no].sub_number <= sub_index) return NULL;

	return &p_od->list[od_list_no].subs[sub_index];
}

static inline void* CO_OD_get_data_pointer(const CO_OD* p_od,
                                const uint16_t od_list_no,const uint8_t sub_index)
{
	if(od_list_no >= p_od->size || od_list_no == OD_UNSUPPORT_ENTRY
			|| p_od->list[od_list_no].sub_number<= sub_index) return NULL;

	return CO_SUB_OBJ_get_data_pointer(&p_od->list[od_list_no],sub_index);
}

static inline uint16_t CO_OD_get_data_len(const CO_OD* p_od,
                                const uint16_t od_list_no,const uint8_t sub_index)
{
	if(od_list_no >= p_od->size || od_list_no == OD_UNSUPPORT_ENTRY
			|| p_od->list[od_list_no].sub_number <= sub_index) return 0;

	return CO_SUB_OBJ_get_data_len(&p_od->list[od_list_no],sub_index);
}

#endif


