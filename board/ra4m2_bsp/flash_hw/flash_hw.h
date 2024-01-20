/*
 * flash_hw.h
 *
 *  Created on: Apr 21, 2022
 *      Author: VanCong
 */

#ifndef BOARD_RA4M2_BSP_FLASH_HW_FLASH_HW_H_
#define BOARD_RA4M2_BSP_FLASH_HW_FLASH_HW_H_
#include "hal_data.h"
/* Code Flash */
#define FLASH_HP_CF_BLOCK_SIZE_32KB       (32*1024)    /* Block Size 32 KB */
#define FLASH_HP_CF_BLOCK_SIZE_8KB        (8*1024)     /* Block Size 8KB */

#define FLASH_HP_CF_BLCOK_0               0x00000000U  /*    8 KB:  0x00000000 - 0x00001FFF */
#define FLASH_HP_CF_BLOCK_1               0x00002000U  /*    8 KB:  0x00002000 - 0x00003FFF */
#define FLASH_HP_CF_BLOCK_2               0x00004000U  /*    8 KB:  0x00004000 - 0x00005FFF */
#define FLASH_HP_CF_BLOCK_3               0x00006000U  /*    8 KB:  0x00006000 - 0x00007FFF */
#define FLASH_HP_CF_BLOCK_4				  0x00008000U  /*	 8 KB:  0x00008000 - 0x00009FFF */
#define FLASH_HP_CF_BLOCK_5				  0x0000A000U  /*    8 KB:  0x0000A000 - 0x0000BFFF */
#define FLASH_HP_CF_BLOCK_6				  0x0000C000U  /*    8 KB:  0x0000C000 - 0x0000DFFF */
#define FLASH_HP_CF_BLOCK_7				  0x0000E000U  /*    8 KB:  0x0000E000 - 0x0000FFFF */
#define FLASH_HP_CF_BLOCK_8               0x00010000U  /*   32 KB: 0x00010000 - 0x00017FFF */
#define FLASH_HP_CF_BLOCK_9               0x00018000U  /*   32 KB: 0x00018000 - 0x0001FFFF */
#define FLASH_HP_CF_BLCOK_10              0x00020000U  /*   32 KB: 0x00020000 - 0x0004FFFF */

#define FLASH_HP_DF_BLOCK_SIZE            (64)
/* Data Flash */

#define FLASH_HP_DF_BLOCK_0               0x08000000U /*   64 B:  0x40100000 - 0x4010003F */
#define FLASH_HP_DF_BLOCK_1               0x08000040U /*   64 B:  0x40100040 - 0x4010007F */
#define FLASH_HP_DF_BLOCK_2               0x08000080U /*   64 B:  0x40100080 - 0x401000BF */
#define FLASH_HP_DF_BLOCK_3               0x080000C0U /*   64 B:  0x401000C0 - 0x401000FF */

#define FLASH_HP_DF_BLOCK_9               0x08000240U /*   64 B:  0x401000C0 - 0x401000FF */


fsp_err_t flash_hp_code_flash_operations(	uint8_t  write_buffer[],
											uint32_t const address,
											uint32_t const num_blocks,
											uint32_t     num_bytes);
fsp_err_t flash_hp_data_flash_operations(	uint8_t  write_buffer[],
											uint32_t const address,
											uint32_t const num_blocks,
											uint32_t     num_bytes);
void flash_init(void);
int8_t flash_hp_data_flash_write_memory(uint32_t addr, uint8_t* write_buff, uint8_t len);


#endif /* BOARD_RA4M2_BSP_FLASH_HW_FLASH_HW_H_ */
