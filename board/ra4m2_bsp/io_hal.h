/*
 * io_hal.h
 *
 *  Created on: Jan 12, 2021
 *      Author: quangnd
 */

#ifndef BOARD_STM32_BSP_IO_HAL_H_
#define BOARD_STM32_BSP_IO_HAL_H_

#include "compiler_optimize.h"
#include "io_hw.h"

void io_hw_init(void) WEAK;
void node_id_init(void) WEAK;

#define HAL_NODE_ID_SET_HIGH(x)		NODE_ID_SET_HIGH(x)
#define HAL_NODE_ID_SET_LOW(x)		NODE_ID_SET_LOW(x)

#define HAL_INPUT_SW_GET_STATE(x)	INPUT_SW_GET_STATE(x)
#define HAL_OUTPUT_SW_SET_HIGH(x)	OUTPUT_SW_SET_HIGH(x)
#define HAL_OUTPUT_SW_SET_LOW(x)	OUTPUT_SW_SET_LOW(x)
#define HAL_OUTPUT_SW_TOGGLE(x)		OUTPUT_SW_TOGGLE(x)

#endif /* BOARD_STM32_BSP_IO_HAL_H_ */
