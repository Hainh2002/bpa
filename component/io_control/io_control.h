#ifndef COMPONENT_IO_CONTROL_IO_CONTROL_H_
#define COMPONENT_IO_CONTROL_IO_CONTROL_H_

#include "switch.h"
//#include "button.h"

typedef enum IOC_STATE{
	IOC_ST_INACTIVE = 0,
	IOC_ST_ACTIVE,
	IOC_ST_FAIL
} IOC_STATE;

typedef struct IOC_t IOC;
struct IOC_t{
	IOC_STATE	state;
	Switch		mc_power;
};

void ioc_init(IOC* p_ioc);
void ioc_process(IOC* p_ioc, uint32_t timestamp,uint8_t mode);

static inline void ioc_set_state(IOC* p_ioc, IOC_STATE state){
	p_ioc->state = state;
}

static inline IOC_STATE ioc_get_state(IOC* p_ioc){
	return p_ioc->state;
}

#endif /* COMPONENT_IO_CONTROL_IO_CONTROL_H_ */
