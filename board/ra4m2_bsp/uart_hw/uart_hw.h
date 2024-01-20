/*
 * uart_hw.h
 *
 *  Created on: Jan 3, 2022
 *      Author: VanCong
 */

#ifndef BOARD_RA4M2_BSP_UART_HW_UART_HW_H_
#define BOARD_RA4M2_BSP_UART_HW_UART_HW_H_

#include "hal_data.h"
void uart_hw_init(void);
void USART_SendData(uint8_t const * const p_src,uint32_t const bytes);

#endif /* BOARD_RA4M2_BSP_UART_HW_UART_HW_H_ */
