/* generated vector source file - do not edit */
#include "bsp_api.h"
/* Do not build these data structures if no interrupts are currently allocated because IAR will have build errors. */
#if VECTOR_DATA_IRQ_COUNT > 0
        BSP_DONT_REMOVE const fsp_vector_t g_vector_table[BSP_ICU_VECTOR_MAX_ENTRIES] BSP_PLACE_IN_SECTION(BSP_SECTION_APPLICATION_VECTORS) =
        {
                        [0] = can_error_isr, /* CAN0 ERROR (Error interrupt) */
            [1] = can_rx_isr, /* CAN0 MAILBOX RX (Reception complete interrupt) */
            [2] = can_tx_isr, /* CAN0 MAILBOX TX (Transmission complete interrupt) */
            [3] = can_rx_isr, /* CAN0 FIFO RX (Receive FIFO interrupt) */
            [4] = can_tx_isr, /* CAN0 FIFO TX (Transmit FIFO interrupt) */
            [5] = agt_int_isr, /* AGT0 INT (AGT interrupt) */
            [6] = sci_uart_rxi_isr, /* SCI0 RXI (Receive data full) */
            [7] = sci_uart_txi_isr, /* SCI0 TXI (Transmit data empty) */
            [8] = sci_uart_tei_isr, /* SCI0 TEI (Transmit end) */
            [9] = sci_uart_eri_isr, /* SCI0 ERI (Receive error) */
            [10] = adc_scan_end_isr, /* ADC0 SCAN END (A/D scan end interrupt) */
            [11] = agt_int_isr, /* AGT1 INT (AGT interrupt) */
            [12] = agt_int_isr, /* AGT2 INT (AGT interrupt) */
            [13] = r_icu_isr, /* ICU IRQ0 (External pin interrupt 0) */
            [14] = fcu_frdyi_isr, /* FCU FRDYI (Flash ready interrupt) */
            [15] = fcu_fiferr_isr, /* FCU FIFERR (Flash access error interrupt) */
            [16] = gpt_counter_overflow_isr, /* GPT0 COUNTER OVERFLOW (Overflow) */
        };
        const bsp_interrupt_event_t g_interrupt_event_link_select[BSP_ICU_VECTOR_MAX_ENTRIES] =
        {
            [0] = BSP_PRV_IELS_ENUM(EVENT_CAN0_ERROR), /* CAN0 ERROR (Error interrupt) */
            [1] = BSP_PRV_IELS_ENUM(EVENT_CAN0_MAILBOX_RX), /* CAN0 MAILBOX RX (Reception complete interrupt) */
            [2] = BSP_PRV_IELS_ENUM(EVENT_CAN0_MAILBOX_TX), /* CAN0 MAILBOX TX (Transmission complete interrupt) */
            [3] = BSP_PRV_IELS_ENUM(EVENT_CAN0_FIFO_RX), /* CAN0 FIFO RX (Receive FIFO interrupt) */
            [4] = BSP_PRV_IELS_ENUM(EVENT_CAN0_FIFO_TX), /* CAN0 FIFO TX (Transmit FIFO interrupt) */
            [5] = BSP_PRV_IELS_ENUM(EVENT_AGT0_INT), /* AGT0 INT (AGT interrupt) */
            [6] = BSP_PRV_IELS_ENUM(EVENT_SCI0_RXI), /* SCI0 RXI (Receive data full) */
            [7] = BSP_PRV_IELS_ENUM(EVENT_SCI0_TXI), /* SCI0 TXI (Transmit data empty) */
            [8] = BSP_PRV_IELS_ENUM(EVENT_SCI0_TEI), /* SCI0 TEI (Transmit end) */
            [9] = BSP_PRV_IELS_ENUM(EVENT_SCI0_ERI), /* SCI0 ERI (Receive error) */
            [10] = BSP_PRV_IELS_ENUM(EVENT_ADC0_SCAN_END), /* ADC0 SCAN END (A/D scan end interrupt) */
            [11] = BSP_PRV_IELS_ENUM(EVENT_AGT1_INT), /* AGT1 INT (AGT interrupt) */
            [12] = BSP_PRV_IELS_ENUM(EVENT_AGT2_INT), /* AGT2 INT (AGT interrupt) */
            [13] = BSP_PRV_IELS_ENUM(EVENT_ICU_IRQ0), /* ICU IRQ0 (External pin interrupt 0) */
            [14] = BSP_PRV_IELS_ENUM(EVENT_FCU_FRDYI), /* FCU FRDYI (Flash ready interrupt) */
            [15] = BSP_PRV_IELS_ENUM(EVENT_FCU_FIFERR), /* FCU FIFERR (Flash access error interrupt) */
            [16] = BSP_PRV_IELS_ENUM(EVENT_GPT0_COUNTER_OVERFLOW), /* GPT0 COUNTER OVERFLOW (Overflow) */
        };
        #endif
