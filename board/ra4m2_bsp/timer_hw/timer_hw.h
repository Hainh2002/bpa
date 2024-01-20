
#ifndef BOARD_RA2L1_BSP_TIMER_HW_TIMER_HW_H_
#define BOARD_RA2L1_BSP_TIMER_HW_TIMER_HW_H_

#include "hal_data.h"
typedef struct Timer_hw_t Timer_hw;
typedef void	(*irq_handle)(void);
struct Timer_hw_t{
	agt_instance_ctrl_t p_hw_timer0;
	agt_instance_ctrl_t p_hw_timer1;
	agt_instance_ctrl_t p_hw_timer2;
	agt_instance_ctrl_t p_hw_timer3;
	void	(*timer0_irq_handle)(void);
	void	(*timer1_irq_handle)(void);
	void	(*timer2_irq_handle)(void);
	void	(*timer3_irq_handle)(void);
};

extern Timer_hw io_scan_timer;
void timer_hw_init(void);
#endif /* BOARD_RA2L1_BSP_TIMER_HW_TIMER_HW_H_ */
