/*
 * debug_io_hw_hal.h
 *
 *  Created on: Oct 9, 2020
 *      Author: quangnd
 */

#ifndef BOARD_DEBUG_IO_HW_HAL_H_
#define BOARD_DEBUG_IO_HW_HAL_H_
#include "compiler_optimize.h"
#include "debug_io_hw.h"

#define HAL_IO1_SET_HIGH	IO1_SET_HIGH;
#define HAL_IO1_SET_LOW		IO1_SET_LOW;
#define HAL_IO2_SET_HIGH	IO2_SET_HIGH;
#define HAL_IO2_SET_LOW		IO2_SET_LOW;

void debug_io_hw_init(void) WEAK;

#endif /* BOARD_DEBUG_IO_HW_HAL_H_ */
