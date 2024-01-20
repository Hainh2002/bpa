/*
 * hw_delay_hal.h
 *
 *  Created on: Oct 16, 2020
 *      Author: quangnd
 */

#ifndef BOARD_HW_DELAY_HAL_H_
#define BOARD_HW_DELAY_HAL_H_
#include "compiler_optimize.h"
#include "stdint.h"
#include "delay_hw.h"

void hw_delay_us(uint32_t us) WEAK;
void hw_delay_ms(uint32_t ms) WEAK;

#endif /* BOARD_HW_DELAY_HAL_H_ */
