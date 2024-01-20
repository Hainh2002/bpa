#ifndef _BOARD_H_
#define _BOARD_H_

#include "hal_data.h"
#include "compiler_optimize.h"
#include "stdint.h"
#include "core_hw.h"
#include "io_hw.h"
#include "timer_hw.h"
#include "adc_hw.h"
#include "io_hal.h"
#include "timer_hal.h"
#include "flash_hw.h"
#include "can_hardware.h"


void board_init(void) ;


//void global_interrupt_enable(void) WEAK;
//void global_interrupt_disable(void) WEAK;
//

#endif
