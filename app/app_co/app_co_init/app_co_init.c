/*
 * app_co_init.c
 *
 *  Created on: Jun 17, 2022
 *      Author: Dakaka
 */

#include "app_co_init.h"
//#include "app_io.h"
#include "app_co.h"
#include "bs_app.h"

#include <stdbool.h>

#if 0
/* Save all communication profile data value in flash to use after reset */
static void app_co_save_all_comm_data_in_flash(void){}
/* Save all manufacturer-specific profile data value in flash to use after reset */
static void app_co_save_all_app_data_in_flash(void){}

/* Update all communication profile initializer data value from flash */
static void app_co_update_all_init_comm_data_from_flash(void){}
/* Update all manufacturer-specific profile initializer data value from flash */
static void app_co_update_all_init_app_data_from_flash(void){}
#endif

/********************************************************************************************
 * 									MANDATORY
 * *******************************************************************************************/
/* Declare for build TPDO message function when using "tpdo_build_data_manually" mode*/
static void tpdo1_build_data_impl(uint8_t* buffer);
static void tpdo2_build_data_impl(uint8_t* buffer);
static void tpdo3_build_data_impl(uint8_t* buffer);
static void tpdo4_build_data_impl(uint8_t* buffer);

void* tpdo_build_data_impl[TPDO_NUMBER] =
{
		tpdo1_build_data_impl,
		tpdo2_build_data_impl,
		tpdo3_build_data_impl,
		tpdo4_build_data_impl


};

static void tpdo1_build_data_impl(uint8_t* buffer)
{

}

static void tpdo2_build_data_impl(uint8_t* buffer)
{


}

static void tpdo3_build_data_impl(uint8_t* buffer)
{
	/* USER CODE BEGIN */


	/* USER CODE END */
}

static void tpdo4_build_data_impl(uint8_t* buffer)
{
	/* USER CODE BEGIN */


	/* USER CODE END */
}

/* Define can_send message function */
static void app_co_can_send_impl(CO_CAN_Msg* p_msg)
{
	can_send_func(selex_bs_app.base.p_hw,p_msg->id.can_id,p_msg->data_len,p_msg->data);
}
/* Call in receive can interrupt */
void app_co_can_receive_handle(const uint32_t can_id, uint8_t* data)
{
	CO_can_receive_basic_handle(&CO_DEVICE, can_id, data) ;

	/* USER CODE BEGIN */

	/* USER CODE END */
}

void app_co_init(void)
{
	app_co_init_storage();
	/* [Mandatory] Set CO_CAN_send interface */
	CO_CAN_set_can_send_interface(app_co_can_send_impl);

	/* [Mandatory] Init CO object */
	CO_init_basic(&CO_DEVICE,
			od_comm_prof_init_data.x1000_device_type,
			&od_comm_prof_init_data.x1018_identity,
			p_co_od);

	CO_SYNC_init(&CO_DEVICE.sync, &od_comm_prof_init_data);

	for(uint8_t i = 0; i < TPDO_NUMBER; i++)
	{
		CO_TPDO_init(&CO_DEVICE.tpdos[i],
				&CO_DEVICE.sync,
				&od_comm_prof_init_data.x1A0x_tpdo_map_para[i],
				CO_DEVICE.p_od,
				&od_comm_prof_init_data.x180x_tpdo_comm_para[i],
				CO_TPDO_build_data_manually, tpdo_build_data_impl[i]);
	}
	CO_SDOserver_init(&CO_DEVICE.sdo_server,
			&od_comm_prof_init_data,
			&CO_DEVICE.sync,
			CO_DEVICE.p_od);
	CO_SDOclient_init(&CO_DEVICE.sdo_client,
			&od_comm_prof_init_data,
			&CO_DEVICE.sync);
	//CO_DEVICE.sdo_server.co_get_od_data_buff = co_od_get_object_data_buff;

	/* USER CODE BEGIN */
	/* USER CODE END */
}


/********************************************************************************************
 * 									USER CODE
 * *******************************************************************************************/
/* Example for bms */
/*void app_co_bms_set_node_id(node_id)
{
	switch(node_id)
	case 5:
		CO_set_node_id(&CO_DEVICE, node_id);
		CO_TPDO_set_transmission_type(&CO_DEVICE.tpdos[1], 9);
		CO_TPDO_set_transmission_type(&CO_DEVICE.tpdos[1], 10);
		CO_TPDO_set_transmission_type(&CO_DEVICE.tpdos[1], 11);
		CO_TPDO_set_transmission_type(&CO_DEVICE.tpdos[1], 12);
		break;

	case 6:
		CO_set_node_id(&CO_DEVICE, node_id);
		CO_TPDO_set_transmission_type(&CO_DEVICE.tpdos[1], 13);
		CO_TPDO_set_transmission_type(&CO_DEVICE.tpdos[1], 14);
		CO_TPDO_set_transmission_type(&CO_DEVICE.tpdos[1], 15);
		CO_TPDO_set_transmission_type(&CO_DEVICE.tpdos[1], 16);
		CO_SDO_set_sync_mask_reg(&CO_DEVICE.sdo_client, mask);
		break;
	case 7:
		CO_set_node_id(&CO_DEVICE, node_id);
		CO_TPDO_set_transmission_type(&CO_DEVICE.tpdos[1], 17);
		CO_TPDO_set_transmission_type(&CO_DEVICE.tpdos[1], 18);
		CO_TPDO_set_transmission_type(&CO_DEVICE.tpdos[1], 19);
		CO_TPDO_set_transmission_type(&CO_DEVICE.tpdos[1], 20);
		CO_SDO_set_sync_mask_reg(&CO_DEVICE.sdo_client, mask);
		break;
	case 7:
		CO_set_node_id(&CO_DEVICE, node_id);
		CO_TPDO_set_transmission_type(&CO_DEVICE.tpdos[1], 17);
		CO_TPDO_set_transmission_type(&CO_DEVICE.tpdos[1], 18);
		CO_TPDO_set_transmission_type(&CO_DEVICE.tpdos[1], 19);
		CO_TPDO_set_transmission_type(&CO_DEVICE.tpdos[1], 20);
		CO_SDO_set_sync_mask_reg(&CO_DEVICE.sdo_client, mask);
		break;
	default:
		break;
}*/
