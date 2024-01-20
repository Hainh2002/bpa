/*
 * app_update.c
 *
 *  Created on: Apr 24, 2022
 *      Author: VanCong
 */
#include "app_update.h"
uint8_t addr_app_update[4];
uint8_t data_app_update[1024];
uint8_t data_app_update_buff[1024];
uint8_t checksum_app_update[5];
uint8_t signature_app_update[7];

App_update app_update_ser;
void jump_to_application( uint32_t address ) __attribute__( ( naked ) );
void jump_to_application( uint32_t address )
{
if( address != 0 ) /* Do this test just to stop the compiler giving a warning */
{
__asm volatile
(
"mov pc, r0 \n"
"nop \n"
"nop \n"
"nop \n"
);
}
}
void app_update_init(App_update* p_app){

	app_update_ser.seg_receive_hmi.signature_app_update[0] = 10001 >> 16;
	app_update_ser.seg_receive_hmi.signature_app_update[1] = 10001 >> 8;
	app_update_ser.seg_receive_hmi.signature_app_update[2] = 10001;
}

void app_update_set_indication(void){
	uint8_t write_data[4] = {0x00};
	for ( uint8_t i = 0; i < 4; i++){
		write_data[i] = 0x11;
	}
	flash_hp_code_flash_operations(write_data, ADDRESS_APP_INDICATION, 1, 4);
}
/*command jumps to another application*/
static uint8_t data[256];
static uint8_t check_data[256];
fsp_err_t overwrite_app_current(void){
	fsp_err_t err = FSP_SUCCESS;
	flash_result_t blank_check_result = FLASH_RESULT_BLANK;

	/* Erase Block */
	err = R_FLASH_HP_Erase(&g_flash_ctrl, ADDRESS_APP_CURRENT, 2);
	/* Error Handle */

	if (FSP_SUCCESS != err)
	{
		return FSP_ERR_WRITE_FAILED;
	}
	/* Blank Check */
	err = R_FLASH_HP_BlankCheck(&g_flash_ctrl, ADDRESS_APP_CURRENT, 64*1024, &blank_check_result);
	/* Error Handle */
	if (FSP_SUCCESS != err)
	{
		return FSP_ERR_WRITE_FAILED ;
	}
	for( uint16_t i = 0; i< 256; i++){

		for( uint16_t j = 0; j< 256; j++){
			data[j] = *(uint8_t *)(ADDRESS_APP_UPDATE + i*256 + j);
		}
		err = R_FLASH_HP_Write(&g_flash_ctrl,  (uint32_t)data,ADDRESS_APP_CURRENT + i*256, 256);
		/* Error Handle */
		if (FSP_SUCCESS != err)
		{
			return FSP_ERR_WRITE_FAILED ;
		}
		memcpy(check_data, (uint8_t *) ADDRESS_APP_CURRENT + i*1024, 1024);
		if (memcmp(check_data, data, 1024) != 0){
			return FSP_ERR_WRITE_FAILED ;
		}
	}
	return err;
}
fsp_err_t erase_app_update_area(uint32_t const addr_area,uint32_t const num_blocks){
	fsp_err_t err = FSP_SUCCESS;
	err = R_FLASH_HP_Erase(&g_flash_ctrl, addr_area, num_blocks);
	return err;
}
void write_seg_app_current(uint32_t const flash_address,uint32_t const data,uint32_t const byte){
	__disable_irq();
	R_FLASH_HP_Write(&g_flash_ctrl,  (uint32_t)data,flash_address, byte);
	memcpy(check_data, (uint8_t *) flash_address, 1024);
	if (memcmp(check_data, data, 1024) != 0){
		//return FSP_ERR_WRITE_FAILED ;
	}
	__enable_irq();
}

