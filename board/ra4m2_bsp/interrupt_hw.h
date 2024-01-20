/*
 * interrupt_hw.h
 *
 *  Created on: Aug 22, 2020
 *      Author: quangnd
 */

#ifndef BOARD_STM32_BSP_INTERRUPT_HW_H_
#define BOARD_STM32_BSP_INTERRUPT_HW_H_

#include "timer_hw.h"

/* System Tick IRQ */
#define STATE_MACHINE_UPDATE_TICK 	SysTick_Handler
#define DISABLE_STM_UPDATE_TICK	 		SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |\
                   SysTick_CTRL_TICKINT_Msk   |\
                   SysTick_CTRL_ENABLE_Msk

#define ENABLE_STM_UPDATE_TICK SysTick->CTRL &= ~(SysTick_CTRL_CLKSOURCE_Msk |\
                   SysTick_CTRL_TICKINT_Msk   |\
                   SysTick_CTRL_ENABLE_Msk)

/* IRQ_Priority */
#define CAN_IRQN_PRIORITY		       	 	0
#define IO_SCAN_TIMER_PRIORITY				1

#endif /* BOARD_STM32_BSP_INTERRUPT_HW_H_ */
