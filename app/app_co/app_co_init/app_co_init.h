/*
 * app_co_init.h
 *
 *  Created on: Jun 17, 2022
 *      Author: Dakaka
 */

#ifndef APP_APP_CANOPEN_APP_CO_INIT_H_
#define APP_APP_CANOPEN_APP_CO_INIT_H_

#include "app_co_storage_init.h"
#include "app_co_od.h"
#include "CO.h"

extern void* tpdo_build_data_impl[TPDO_NUMBER];

void app_co_init(void);
void app_co_can_receive(const uint32_t can_id, uint8_t* data);
void app_co_can_receive_handle(const uint32_t can_id, uint8_t* data);
/* USER CODE 1 BEGIN*/
void co_process_rewrite(CO* p_co, uint16_t time_diff_ms);
/* USER CODE 1 END*/
#endif /* APP_APP_CANOPEN_APP_CO_INIT_H_ */

