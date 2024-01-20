
#ifndef BOARD_RA2L1_BSP_CAN_HARDWARE_CAN_HARDWARE_H_
#define BOARD_RA2L1_BSP_CAN_HARDWARE_CAN_HARDWARE_H_
#include "hal_data.h"
#include "io_hal.h"
typedef struct CAN_Hw_t CAN_Hw;

typedef struct
{
	uint32_t StdId;    /*!< Specifies the standard identifier.
                          This parameter must be a number between Min_Data = 0 and Max_Data = 0x7FF. */

	uint32_t ExtId;    /*!< Specifies the extended identifier.
                          This parameter must be a number between Min_Data = 0 and Max_Data = 0x1FFFFFFF. */

	uint32_t IDE;      /*!< Specifies the type of identifier for the message that will be transmitted.
                          This parameter can be a value of @ref CAN_identifier_type */

	uint32_t RTR;      /*!< Specifies the type of frame for the message that will be transmitted.
                          This parameter can be a value of @ref CAN_remote_transmission_request */

	uint32_t DLC;      /*!< Specifies the length of the frame that will be transmitted.
                          This parameter must be a number between Min_Data = 0 and Max_Data = 8. */

} CAN_TxHeaderTypeDef;
typedef struct
{
	uint32_t StdId;    /*!< Specifies the standard identifier.
                          This parameter must be a number between Min_Data = 0 and Max_Data = 0x7FF. */

	uint32_t ExtId;    /*!< Specifies the extended identifier.
                          This parameter must be a number between Min_Data = 0 and Max_Data = 0x1FFFFFFF. */

	uint32_t IDE;      /*!< Specifies the type of identifier for the message that will be transmitted.
                          This parameter can be a value of @ref CAN_identifier_type */

	uint32_t RTR;      /*!< Specifies the type of frame for the message that will be transmitted.
                          This parameter can be a value of @ref CAN_remote_transmission_request */

	uint32_t DLC;      /*!< Specifies the length of the frame that will be transmitted.
                          This parameter must be a number between Min_Data = 0 and Max_Data = 8. */

	uint32_t Timestamp; /*!< Specifies the timestamp counter value captured on start of frame reception.
                          @note: Time Triggered Communication Mode must be enabled.
                          This parameter must be a number between Min_Data = 0 and Max_Data = 0xFFFF. */

	uint32_t FilterMatchIndex; /*!< Specifies the index of matching acceptance filter element.
                          This parameter must be a number between Min_Data = 0 and Max_Data = 0xFF. */

} CAN_RxHeaderTypeDef;


struct CAN_Hw_t {
	can_instance_ctrl_t 	can_module;
	CAN_TxHeaderTypeDef	can_tx;
	CAN_RxHeaderTypeDef	can_rx;
	can_frame_t	can_tx_hw;
	can_frame_t	can_rx_hw;
	uint32_t			tx_mailbox;
	uint8_t				tx_data[8];
	uint8_t				tx_data_pdo[8];
	uint8_t				rx_data[8];
	void 				(*sw_on)(void);
	void 				(*sw_off)(void);
	void 				(*can_send)(CAN_Hw* p_hw);
	void				(*receive_handle)(CAN_Hw* p_hw);
};


extern CAN_Hw	can_port;
void can_hardware_init(void);
void can_send(CAN_Hw* p_hw, uint8_t* buff);
void can_send_func(CAN_Hw* p_hw,uint32_t StdID,uint32_t DLC, uint8_t* buff);
void can_receive(CAN_Hw* p_hw, uint8_t* buff);
static void can_hardware_init_tx(void);
static inline void can_hw_disable_irq(void){
	R_BSP_IrqDisable(VECTOR_NUMBER_CAN0_MAILBOX_RX);
}

static inline void can_hw_enable_irq(void){
	R_BSP_IrqEnable(VECTOR_NUMBER_CAN0_MAILBOX_RX);
}
static inline void CO_memcpy_buff(uint8_t dest[], const uint8_t src[], const uint16_t size){
	uint16_t i;
	for(i = 0; i < size; i++){
		dest[i] = src[i];
	}
}
static inline void disabled_can_transceiver(void){
	HAL_OUTPUT_SW_SET_HIGH(CAN_MODE);
}
static inline void enabled_can_transceiver(void){
	HAL_OUTPUT_SW_SET_LOW(CAN_MODE);
}
void can_set_receive_handle(CAN_Hw* p_hw,void (*receive_handle)(CAN_Hw* p_hw));
#endif /* BOARD_RA2L1_BSP_CAN_HARDWARE_CAN_HARDWARE_H_ */
