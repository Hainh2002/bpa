#ifndef _BOARD_H_
#define _BOARD_H_

#include "interrupt_hal.h"
#include "compiler_optimize.h"
#include "stdint.h"
#include "core_hal.h"
#include "can_hal.h"
#include "io_hal.h"
#include "timer_hal.h"


void board_init(void) WEAK;
void global_interrupt_enable(void) WEAK;
void global_interrupt_disable(void) WEAK;


#endif
