/*
 * canopen_init.h
 *
 *  Created on: Oct 21, 2020
 *      Author: quangnd
 */

#ifndef APP_APP_CO_H_
#define APP_APP_CO_H_

#include "bs_app.h"
#include "can_master.h"

void app_co_init1(pmu_app*);
void pdo_data_processing( pmu_app * p_app);
void CO_SDO_Client_reset_timeout( CAN_master * p_cm);
void sysreset_upgrade_firmware(void);

#endif /* APP_APP_CO_H_ */
