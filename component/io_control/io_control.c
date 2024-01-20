#include "io_control.h"
#include "bs_app.h"

static void ioc_update_input_sw_state(IOC* p_ioc, uint32_t timestamp);
static void ioc_update_output_sw_state(IOC* p_ioc, uint32_t timestamp, uint8_t mode);

void ioc_init(IOC* p_ioc){
	p_ioc->state = IOC_ST_INACTIVE;
}

void ioc_process(IOC* p_ioc, uint32_t timestamp, uint8_t mode){
	ioc_update_input_sw_state(p_ioc, timestamp);
	ioc_update_output_sw_state(p_ioc, timestamp,mode);
}

static void ioc_update_input_sw_state(IOC* p_ioc, uint32_t timestamp){
	(void)timestamp;
	(void)p_ioc;
}

static void ioc_update_output_sw_state(IOC* p_ioc, uint32_t timestamp,uint8_t mode){
	(void)timestamp;
	(void)mode;
	sw_update_output_state(&p_ioc->mc_power);
}
