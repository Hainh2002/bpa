/*
 * uart_hal.h
 *
 *  Created on: Aug 21, 2020
 *      Author: quangnd
 */

#ifndef BOARD_DEBUG_COM_PORT_HAL_H_
#define BOARD_DEBUG_COM_PORT_HAL_H_
#include "compiler_optimize.h"
#include "stdint.h"
#include "debug_com_port_hardware.h"

extern DEBUG_COM_HW debug_port;

void debug_com_hw_init(void) WEAK;
void debug_com_sends(DEBUG_COM_HW* p_hw,const uint8_t* s) WEAK;

#endif /* BOARD_DEBUG_COM_PORT_HAL_H_ */
