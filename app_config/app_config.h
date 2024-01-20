	/*
 * app_config.h
 *
 *  Created on: Oct 27, 2020
 *      Author: quangnd
 */

#ifndef APP_CONFIG_APP_CONFIG_H_
#define APP_CONFIG_APP_CONFIG_H_
#include "core_hal.h"


/* Timeout Parameters */
#define APP_STATE_MACHINE_UPDATE_TICK_mS                (1000/HAL_SYSTICK_FREQ_Hz)
#define BP_INACTIVE_TIMEOUT_mS                          3000
#define BP_INACTIVE_TIMEOUT_IN_RUNNNG_mS                10000
#define BP_INIT_INACTIVE_TIMEOUT_mS                    	600
#define BP_SW_TYPE_TIMEOUT_mS                          	3000
#define BAT_PORT_NUM									3
#define BP_INVALID_REMOVE_ms							20000

/* S2 ID Numbers */
#define BS_NODE_ID_NUM									1
#define MC_NODE_ID_NUM									2
#define HMI_NODE_ID_NUM									3
#define BP_DEFAULT_NODE_ID_NUM							4
#define BP_START_NODE_ID_NUM							5

/* BP Parameters */
#define BP_RESISTANCE_mOhm								120
#define BP_UPPER_WORKING_VOL_LEVEL_mV					63000
#define BP_LOWER_WORKING_VOL_LEVEL_mV					48000
#define BP_CAPACITY_Ah									16.0
#define MAX_DISTANCE_AVAILABLE_PER_BP_KM				40.0

/* SoC Temporary Calculation */
#define BP_WORKING_VOL_LEVEL_RANGE						(BP_UPPER_WORKING_VOL_LEVEL_mV - BP_LOWER_WORKING_VOL_LEVEL_mV)
#define TOTAL_COULOMBS									(float)(BP_CAPACITY_Ah*3600)
#define SOC_TEMP_CAL_FACTOR								(float)(100*UPDATE_DISTANCE_TIME_mS/1000/TOTAL_COULOMBS)

/* Distance Available Calculation */
#define UPDATE_DISTANCE_TIME_INTERVAL_mS				10000
#define UPDATE_DISTANCE_TIME_mS							10000.0
#define DISTANCE_SOC_CAL_FACTOR							(float)(MAX_DISTANCE_AVAILABLE_PER_BP_KM/100000.0)

/* IOC - Cabinet Switch */
#define CABINET_SIGNAL_DELAY_mS							1000
#define CABINET_ON_INTERVAL_mS							3000
#define HW_VER 											1
#define SDO_CLIENT_RESET_TIMEOUT_10ms					1000
#define BP_VERIFY_MODE									0
/* */

#define SDO_HMI_COMMAND_INDEX							0x2002
#define SDO_HMI_COMMAND_FIND_VEHICLES_SUB_INEX			0
#define SDO_HMI_COMMAND_DISABLE_VEHICLES_SUB_INEX		1
#define SDO_HMI_COMMAND_HORN_VEHICLES_SUB_INEX			2

#define SDO_HMI_OTA_INDEX								0x2200
#define SDO_HMI_OTA_SET_STATE_SUBINDEX					0

/* define data memory areas*/
#define PMU_SYSTEM_INFORMATION_ADDR						FLASH_HP_DF_BLOCK_2
#define	EV_ID_MEM_ADDR									PMU_SYSTEM_INFORMATION_ADDR + 28

#define	EV_PROTECT_MODE_FLAG_ADDR						0x08000240U          //FLASH_HP_DF_BLOCK_9
#define	EV_BLOCK_MODE_FLAG_ADDR							0x08000240U  //FLASH_HP_DF_BLOCK_9
#define	EV_LOCK_MODE_FLAG_ADDR							EV_BLOCK_MODE_FLAG_ADDR + 4
#define	EV_ANTI_MODE_FLAG_ADDR							EV_BLOCK_MODE_FLAG_ADDR + 8
#define	SYS_ENGERY_PAR_FLAG_ADDR						0x08000240U + 64 // lưu thông sô electric sys cần 10 byte



#define PMU_SYSTEM_INFORMATION_LEN						128
/* key*/

#define DEVICE_PASS										"selexpmu@"
#define DEVICE_PASS_LEN									9

/*MC Config*/

#define MC_INACTIVE_TIME								2000	// 2000ms
/*Upgrade firmware*/
#define BOOT_STATE_TIMEOUT								120000 // 120s

/*ABP*/
#define ABP_LOW_VOLTAGE									10000// mV

#endif /* APP_CONFIG_APP_CONFIG_H_ */
