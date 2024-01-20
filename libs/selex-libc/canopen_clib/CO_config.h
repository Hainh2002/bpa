/*
 * CO_error.h
 *
 *  Created on: Jun 15, 2022
 *      Author: dacnq
 */

#ifndef CO_ERROR_H_
#define CO_ERROR_H_

#include "CO_utils.h"



//#define NULL ((void *)0)

#define CO_SDO_CHECK_C_BIT_OF_CS										1
#define CO_SDO_CHECK_MATCH_DATA_LENGTH									1
#define CO_SDO_ENABLE_PROCESS_EXTENSION_IN_SERVER_DOWNLOAD				1
#define CO_SDO_USE_SYNC_WINDOW_LENGTH									0

#define CO_TPDO_USE_INHIBIT_TIMER										0
#define CO_TPDO_USE_EVENT_TIMER											0


static inline void CO_detect_error(void)
{
  while (1){}
}

#endif /* CO_ERROR_H_ */
