#include "board.h"
#include "uart_hw.h"

void board_init(void) {
	core_hw_init();
	io_hw_init();
	can_hardware_init();
	timer_hw_init();
	uart_hw_init();
	flash_init();
}
