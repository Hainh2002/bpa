/*
 * timer_hal.h
 *
 *  Created on: Aug 25, 2021
 *      Author: KhanhDinh
 */

#ifndef BOARD_TIMER_HAL_H_
#define BOARD_TIMER_HAL_H_

#include "timer_hw.h"
#include "compiler_optimize.h"

void timer_hw_init(void) WEAK;

static inline void timer_set_irq_handler(Timer_hw* p_hw, void (*timer_irq_handle)(void)){
	p_hw->timer_irq_handle = timer_irq_handle;
};

#endif /* BOARD_TIMER_HAL_H_ */
