
#include "io_hw.h"

void io_hw_init(void){
	R_IOPORT_Open (&g_ioport_ctrl, g_ioport.p_cfg);
}
