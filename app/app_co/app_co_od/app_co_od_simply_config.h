/*
 * app_co_od_simply_config.h
 *
 *  Created on: Jul 15, 2022
 *      Author: Dakaka
 *
 *      For selex guys who love simplicity!
 */

#ifndef APP_APP_CANOPEN_APP_CO_OD_SIMPLY_CONFIG_H_
#define APP_APP_CANOPEN_APP_CO_OD_SIMPLY_CONFIG_H_

#include "CO.h"
#include "bs_app.h"
/************************************************************************************
 *					Set default value for CAN communication
 ************************************************************************************/
#define	NODE_ID_DEFAULT		0x01
#define CO_DEVICE			selex_bs_app.base.co_app		//<< canopen object

/* Identity */
#define VENDER_ID			0x00000000	//<< option
#define	PRODUCT_CODE		0x00000000	//<< option
#define	REVISION_NUMBER		0x00000000	//<< option
#define	SERIAL_NUMBER		0x00000000	//<< option

/* Version */
#define HW_VERSION			0x00000000	//<< option
#define SW_VERSION			0x00010007	//<< option

#define FRIMWARE_NAME		"SELEX_PMU_MAINAPP"

/* */
#define CO_CF_1008_DEVICE_NAME	"SELEX_PMU"
#define CO_CF_1008_DEVICE_NAME_SIZE		(sizeof(CO_CF_1008_DEVICE_NAME))

/* Synchronous schedule */
#define CO_CF_1006_COMM_CYCLE_PERIOD_MS					(10U)	//<< Set according to synchronous scheduled
#define CO_CF_1007_SYNC_WINDO_LENGTH_MS					(5U)	//<< Set according to synchronous scheduled
#define CO_CF_1019_SYNC_CTER_OVER_FLOW_VALUE			(25U)	//<< Set according to synchronous scheduled. Setting range: 1-30000
#define CO_CF_1030_SYNC_BIT_POS_OVER_FLOW_VALUE			(11U)	//<< Set according to synchronous scheduled. Setting range: 0-31

#define CO_CF_100500_SYNC_ENABLE						CO_SYNC_VALID		//<< Enable sync mode: Set CO_SYNC_VALID
																			//   Disable sync mode: set CO_SYNC_NOT_VALID
#define CO_CF_100500_SYNC_GEN							CO_SYNC_GEN			//<< PMU/BSS_MASTER: Set CO_SYNC_GEN
																			//   MC/BP/...: Set CO_SYNC_NOT_GEN

/* Synchronous server SDOs */
#define CO_CF_103101_SDO_SERV_TIMEOUT_MS				(1000)

/* Synchronous client SDOs */
#define CO_CF_103200_SDO_CLI_SYNC_TRANS_TYPE			(0)				//<< Set according to synchronous scheduled
																		//   Set 0 if you want disable sync mode, switch to always communication mode
#define CO_CF_103201_SDO_CLI_SYNC_MASK_REGISTER			(0x00000FFF)	//<< Set according to synchronous scheduled, default: 0x00000000

/* Synchronous TPDOs */
#define CO_CF_180000_TPDO1_ENABLE						CO_PDO_VALID	//<< Set CO_PDO_VALID if used, otherwise, set CO_PDO_NOT_VALID
#define CO_CF_180001_TPDO1_TRANS_TYPE					(21)				//<< Set according to synchronous scheduled
#define CO_CF_180005_TPDO1_SYNC_MASK_REGISTER			(0x00000FFF)	//<< Set according to synchronous scheduled, default: 0x00000000

#define CO_CF_180100_TPDO2_ENABLE						CO_PDO_VALID	//<< Set CO_PDO_VALID if used, otherwise, set CO_PDO_NOT_VALID
#define CO_CF_180101_TPDO2_TRANS_TYPE					(21)				//<< Set according to synchronous scheduled
#define CO_CF_180105_TPDO2_SYNC_MASK_REGISTER			(0x00000FFF)	//<< Set according to synchronous scheduled, default: 0x00000000

#define CO_CF_180200_TPDO3_ENABLE						CO_PDO_VALID	//<< Set CO_PDO_VALID if used, otherwise, set CO_PDO_NOT_VALID
#define CO_CF_180201_TPDO3_TRANS_TYPE					(22)				//<< Set according to synchronous scheduled
#define CO_CF_180205_TPDO3_SYNC_MASK_REGISTER			(0x00000FFF)	//<< Set according to synchronous scheduled, default: 0x00000000

#define CO_CF_180300_TPDO4_ENABLE						CO_PDO_VALID//<< Set CO_PDO_VALID if used, otherwise, set CO_PDO_NOT_VALID
#define CO_CF_180301_TPDO4_TRANS_TYPE					(22)				//<< Set according to synchronous scheduled
#define CO_CF_180305_TPDO4_SYNC_MASK_REGISTER			(0x00000FFF)	//<< Set according to synchronous scheduled, default: 0x00000000

#endif /* APP_APP_CANOPEN_APP_CO_OD_SIMPLY_CONFIG_H_ */
