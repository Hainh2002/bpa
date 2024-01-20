/*
 * bat_sw_hal.h
 *
 *  Created on: Jan 13, 2021
 *      Author: quangnd
 */

#ifndef BOARD_BAT_PORT_HAL_H_
#define BOARD_BAT_PORT_HAL_H_
#include "compiler_optimize.h"

#define HAL_BAT_PORT_ENABLE(x)				BAT_PORT_ENABLE(x)
#define HAL_BAT_PORT_DISABLE(x)				BAT_PORT_DISABLE(x)

#define HAL_BAT_PORT_DETECT_ENABLE(x)		BAT_PORT_DETECT_ENABLE(x)
#define HAL_BAT_PORT_DETECT_DISABLE(x)		BAT_PORT_DETECT_DISABLE(x)

void bat_port_hw_init(void) WEAK;

#endif /* BOARD_BAT_PORT_HAL_H_ */
