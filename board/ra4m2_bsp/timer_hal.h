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

static inline void timer0_set_irq_handler(Timer_hw* p_hw, void (*timer_irq_handle)(void)){
	p_hw->timer0_irq_handle = timer_irq_handle;
};
static inline void timer1_set_irq_handler(Timer_hw* p_hw, void (*timer_irq_handle)(void)){
	p_hw->timer1_irq_handle = timer_irq_handle;
};
static inline void timer2_set_irq_handler(Timer_hw* p_hw, void (*timer_irq_handle)(void)){
	p_hw->timer2_irq_handle = timer_irq_handle;
};
static inline void timer3_set_irq_handler(Timer_hw* p_hw, void (*timer_irq_handle)(void)){
	p_hw->timer3_irq_handle = timer_irq_handle;
};
static inline void timer0_reset_irq_handler(Timer_hw* p_hw){
	p_hw->timer0_irq_handle = NULL;
};
static inline void timer1_reset_irq_handler(Timer_hw* p_hw){
	p_hw->timer1_irq_handle = NULL;
};
static inline void timer2_reset_irq_handler(Timer_hw* p_hw){
	p_hw->timer2_irq_handle = NULL;
};

#endif /* BOARD_TIMER_HAL_H_ */
