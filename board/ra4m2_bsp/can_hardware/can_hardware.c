
#include "can_hardware.h"

CAN_Hw can_port;
can_frame_t can_tx_t;
#define CAN_MAILBOX_NUMBER_0            (0U)               //mail box number
static void memcpy_can_hw_rx(can_callback_args_t *p_args);

void can_hardware_init(void) {

	R_CAN_Open(&can_port.can_module, &g_can0_cfg);
	can_hardware_init_tx();
	can_port.sw_on =  enabled_can_transceiver;
	can_port.sw_off = disabled_can_transceiver;
	HAL_OUTPUT_SW_SET_LOW(CAN_MODE);

}
void can_send(CAN_Hw* p_hw, uint8_t* buff){
	p_hw->can_tx_hw.id = p_hw->can_tx.StdId;
	p_hw->can_tx_hw.data_length_code = p_hw->can_tx.DLC;
	CO_memcpy_buff(p_hw->can_tx_hw.data, buff, 8);
	R_CAN_Write(&can_port.can_module, CAN_MAILBOX_NUMBER_0, &can_port.can_tx_hw);
}
void can_send_func(CAN_Hw* p_hw,uint32_t StdID,uint32_t DLC, uint8_t* buff){
	(void)p_hw;
	can_frame_t can_tx_t;
	can_tx_t.id = StdID;
	can_tx_t.data_length_code = DLC;
	can_tx_t.id_mode = CAN_ID_MODE_STANDARD;
	can_tx_t.type = CAN_FRAME_TYPE_DATA;
	CO_memcpy_buff(can_tx_t.data, buff, 8);
	R_CAN_Write(&can_port.can_module, CAN_MAILBOX_NUMBER_0, &can_tx_t);
}
/* Callback function CAN ISR */
void can_callback(can_callback_args_t *p_args)
{
	if ( p_args->event != CAN_EVENT_RX_COMPLETE) return;
	memcpy_can_hw_rx(p_args);
	if(can_port.receive_handle != NULL){
		can_port.receive_handle(&can_port);
	}
}
static void memcpy_can_hw_rx(can_callback_args_t *p_args){
	can_port.can_rx.StdId = p_args->frame.id;
	can_port.can_rx.DLC = p_args->frame.data_length_code;
	CO_memcpy_buff(can_port.rx_data, p_args->frame.data, 8);
}
static void can_hardware_init_tx(void){
	can_port.can_tx.StdId = 1;
	can_port.can_tx.DLC = 8;
	can_port.can_tx_hw.id_mode = CAN_ID_MODE_STANDARD;
	can_port.can_tx_hw.type = CAN_FRAME_TYPE_DATA;
}
void can_set_receive_handle(CAN_Hw* p_hw,void (*receive_handle)(CAN_Hw* p_hw)){
	(void)p_hw;
	p_hw->receive_handle=receive_handle;
}

