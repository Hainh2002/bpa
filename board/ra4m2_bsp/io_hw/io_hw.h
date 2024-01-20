/*
 * io_hw.h
 *
 *  Created on: Dec 11, 2021
 *      Author: VanCong
 */

#ifndef BOARD_RA4M2_BSP_IO_HW_IO_HW_H_
#define BOARD_RA4M2_BSP_IO_HW_IO_HW_H_
#include "hal_data.h"
#include "app_config.h"


void io_hw_init(void);

#define INPUT_SW_GET_STATE(x)	R_BSP_PinRead(x)
#define OUTPUT_SW_SET_HIGH(x)	R_IOPORT_PinWrite(&g_ioport_ctrl, x, 1)
#define OUTPUT_SW_SET_LOW(x)	R_IOPORT_PinWrite(&g_ioport_ctrl, x, 0)
#define OUTPUT_SW_TOGGLE(x)		R_IOPORT_PinWrite(&g_ioport_ctrl, x, !R_BSP_PinRead(x))
#define NODE_ID_SET_HIGH(x)		R_IOPORT_PinWrite(&g_ioport_ctrl, x, 1)
#define NODE_ID_SET_LOW(x)		R_IOPORT_PinWrite(&g_ioport_ctrl, x, 0)
typedef enum {
	CAN_MODE			 = IOPORT_PORT_01_PIN_04,
	NodeID1			 	 = IOPORT_PORT_00_PIN_02,
	NodeID2				 = IOPORT_PORT_00_PIN_01,
	NodeID3			 	 = IOPORT_PORT_00_PIN_00,

}IO_BS;


#endif /* BOARD_RA4M2_BSP_IO_HW_IO_HW_H_ */
