/*
 * timer_hw.c
 *
 *  Created on: Nov 26, 2021
 *      Author: KhanhDinh
 */

#include "timer_hw.h"

Timer_hw io_scan_timer;
static void io_scan_timer_init(void);
void timer_hw_init(void){
	io_scan_timer_init();
}

static void io_scan_timer_init(void){
    R_AGT_Open(&io_scan_timer.p_hw_timer0, &g_timer0_cfg);
    R_AGT_Start(&io_scan_timer.p_hw_timer0);
    R_BSP_SoftwareDelay(3,BSP_DELAY_UNITS_MILLISECONDS);
    R_AGT_Open(&io_scan_timer.p_hw_timer1, &g_timer1_cfg);
    R_AGT_Start(&io_scan_timer.p_hw_timer1);
    R_BSP_SoftwareDelay(3,BSP_DELAY_UNITS_MILLISECONDS);
    R_AGT_Open(&io_scan_timer.p_hw_timer2, &g_timer2_cfg);
    R_AGT_Start(&io_scan_timer.p_hw_timer2);
    R_GPT_Open(&io_scan_timer.p_hw_timer3, &g_timer3_cfg);
    R_GPT_Start(&io_scan_timer.p_hw_timer3);

}
void Timer_ISR(timer_callback_args_t *p_args)
{
	(void)p_args;
	if(io_scan_timer.timer0_irq_handle != NULL){
		io_scan_timer.timer0_irq_handle();
	}
}

void Timer_ISR1(timer_callback_args_t *p_args)
{
	(void)p_args;
	if(io_scan_timer.timer1_irq_handle != NULL){
		io_scan_timer.timer1_irq_handle();
	}
}
void Timer_ISR2(timer_callback_args_t *p_args)
{
	(void)p_args;
	if(io_scan_timer.timer2_irq_handle != NULL){
		io_scan_timer.timer2_irq_handle();
	}
}
void timer3_ISR(timer_callback_args_t *p_args)
{
	(void)p_args;
	if(io_scan_timer.timer3_irq_handle != NULL){
		io_scan_timer.timer3_irq_handle();
	}
}
