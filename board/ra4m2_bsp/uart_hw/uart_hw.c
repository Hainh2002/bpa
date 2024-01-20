/*
 * uart_hw.c
 *
 *  Created on: Jan 3, 2022
 *      Author: VanCong
 */

#include "uart_hw.h"

void uart_hw_init(void){
	R_SCI_UART_Open(&g_uart0_ctrl, &g_uart0_cfg);
}

/* Callback function */
void user_uart_callback(uart_callback_args_t *p_args)
{
	(void)p_args;
	/* TODO: add your own code here */
}
void USART_SendData(uint8_t const * const p_src,uint32_t const bytes){
	R_SCI_UART_Write(&g_uart0_ctrl, p_src, bytes);

}
