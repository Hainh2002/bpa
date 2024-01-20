#include "stdlib.h"
#include "stdint.h"
#include "flash_hw.h"
extern uint32_t f_rx;

#define ADDRESS_APP_UPDATE (0x00020000)
#define ADDRESS_APP_CURRENT (0x00010000)
#define ADDRESS_APP_INDICATION (0x0000E000)
#define ADDRESS_APP_BOOTLOADER (0x00000000)

extern uint8_t addr_app_update[4];
extern uint8_t data_app_update[1024];
extern uint8_t data_app_update_buff[1024];
extern uint8_t checksum_app_update[5];
extern uint8_t signature_app_update[7];


typedef struct{

	uint8_t addr_app_update[4];
	uint8_t data_app_update[1024];
	uint8_t checksum_app_update[5];
	uint8_t signature_app_update[7];
	uint32_t	adrr;

}seg_frame;

typedef struct{

	seg_frame seg_receive_hmi;
	seg_frame seg_receive_buff;
	uint32_t	checksum;
	uint32_t	checksum_arr_1K[64];
	uint32_t	start_addr;
	uint32_t	vesion;
	uint32_t	num_byte_seg;
	uint8_t		boot_st[2];

}App_update;
extern App_update app_update_ser;

void app_update_init(App_update* );
void app_update_set_indication(void);
void app_update_receive_data_process(void);
fsp_err_t overwrite_app_current(void);
fsp_err_t erase_app_update_area(uint32_t const addr_area,uint32_t const num_blocks);
void write_seg_app_current(uint32_t const flash_address,uint32_t const data,uint32_t const byte);
