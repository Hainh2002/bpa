#include "hal_data.h"
#include "stdlib.h"
#include "stdbool.h"


#define FLASH_HP_CF_BLOCK_SIZE_32KB       (32*1024)    /* Block Size 32 KB */
#define FLASH_HP_CF_BLOCK_SIZE_8KB        (8*1024)     /* Block Size 8KB */

#define FLASH_HP_CF_BLCOK_0               0x00000000U  /*    8 KB:  0x00000000 - 0x00001FFF */
#define FLASH_HP_CF_BLOCK_1               0x00002000U  /*    8 KB:  0x00002000 - 0x00003FFF */
#define FLASH_HP_CF_BLOCK_2               0x00004000U  /*    8 KB:  0x00004000 - 0x00005FFF */
#define FLASH_HP_CF_BLOCK_3               0x00006000U  /*    8 KB:  0x00006000 - 0x00007FFF */
#define FLASH_HP_CF_BLOCK_4               0x00008000U  /*    8 KB:  0x00008000 - 0x00009FFF */
#define FLASH_HP_CF_BLOCK_5               0x0000A000U  /*    8 KB:  0x0000A000 - 0x0000BFFF */
#define FLASH_HP_CF_BLOCK_6               0x0000C000U  /*    8 KB:  0x0000C000 - 0x0000DFFF */
#define FLASH_HP_CF_BLOCK_7               0x0000E000U  /*    8 KB:  0x0000E000 - 0x0000FFFF */
#define FLASH_HP_CF_BLOCK_8               0x00010000U  /*   32 KB: 0x00010000 - 0x00017FFF */
#define FLASH_HP_CF_BLOCK_9               0x00018000U  /*   32 KB: 0x00018000 - 0x0001FFFF */
#define FLASH_HP_CF_BLCOK_10              0x00020000U  /*   32 KB: 0x00020000 - 0x0004FFFF */

#define FLASH_HP_DF_BLOCK_SIZE            (64)
/* Data Flash */

#define FLASH_HP_DF_BLOCK_0               0x08000000U /*   64 B:  0x40100000 - 0x4010003F */
#define FLASH_HP_DF_BLOCK_1               0x08000040U /*   64 B:  0x40100040 - 0x4010007F */
#define FLASH_HP_DF_BLOCK_2               0x08000080U /*   64 B:  0x40100080 - 0x401000BF */
#define FLASH_HP_DF_BLOCK_3               0x080000C0U /*   64 B:  0x401000C0 - 0x401000FF */
#define FLASH_HP_DF_BLOCK_9               0x08000240U /*   64 B:  0x401000C0 - 0x401000FF */

#define BLOCK_SIZE                        (4)
#define BLOCK_NUM                         (2)
#define RESET_VALUE             (0x00)

FSP_CPP_HEADER
void R_BSP_WarmStart(bsp_warm_start_event_t event);
FSP_CPP_FOOTER
volatile int cnt = 0;
volatile int cnt1 = 0;
volatile int test_adc;
uint32_t test_flash;
volatile float temp ;
uint8_t *uart;
//volatile flash_result_t blank_check_result = FLASH_RESULT_BLANK;
volatile int errr;
//volatile int testsizeof;
volatile int32_t rocv = 0;
volatile int32_t flash = 0;
volatile fsp_err_t err = FSP_SUCCESS;
#define WAIT_TIME                       (500U)             //wait time value
#define CAN_DESTINATION_MAILBOX_3       (3U)               //destination mail box number
#define CAN_MAILBOX_NUMBER_0            (4U)               //mail box number
#define CAN_FRAME_TRANSMIT_DATA_BYTES   (8U)               //data length
#define ZERO                            (0U)
#define NULL_CHAR                       ('\0')
static can_frame_t g_can_tx_frame = {0};      //CAN transmit frame
static can_frame_t g_can_rx_frame = {0};      //CAN receive frame
int *testsizeof;
fsp_err_t flash_hp_data_flash_operations(void);
static volatile _Bool g_b_flash_event_not_blank = false;
static volatile _Bool g_b_flash_event_blank = false;
static volatile _Bool g_b_flash_event_erase_complete = false;
static volatile _Bool g_b_flash_event_write_complete = false;
void hal_entry(void)
{
    *uart = 'a';
    R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_04_PIN_07 , 1);
    R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_04_PIN_08 , 1);
    R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_04_PIN_09 , 1);
    R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_02_PIN_07 , 1);
//    R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_02_PIN_00 , 1);
    R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_02_PIN_01 , 1);
    //R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_02_PIN_06 , 1);
    testsizeof =(int*)malloc(10*sizeof(int));
    testsizeof[2]=1;
    //'timer_cfg_t p_bp1 = (timer_cfg_t*)malloc(sizeof(timer_cfg_t));
    // while (p_bp1 == NULL);
    R_AGT_Open(&g_timer0_ctrl, &g_timer0_cfg);
    R_AGT_Start(&g_timer0_ctrl);
    R_AGT_Open(&g_timer1_ctrl, &g_timer1_cfg);
    R_AGT_Start(&g_timer1_ctrl);
    R_AGT_Open(&g_timer2_ctrl, &g_timer2_cfg);
    R_AGT_Start(&g_timer2_ctrl);
    errr = R_CAN_Open(&g_can0_ctrl, &g_can0_cfg);
    R_BSP_SoftwareDelay(2000,BSP_DELAY_UNITS_MILLISECONDS);

    g_can_tx_frame.id = 0x580+4;
    g_can_tx_frame.type = CAN_FRAME_TYPE_DATA;
    g_can_tx_frame.data_length_code = CAN_FRAME_TRANSMIT_DATA_BYTES;
    g_can_tx_frame.data[0] =1;

    R_SCI_UART_Open(&g_uart0_ctrl, &g_uart0_cfg);


    R_ADC_Open(&g_adc0_ctrl, &g_adc0_cfg);
    R_ADC_ScanCfg(&g_adc0_ctrl, &g_adc0_channel_cfg);
    R_ADC_ScanStart(&g_adc0_ctrl);
    R_ICU_ExternalIrqOpen(&g_external_irq0_ctrl, &g_external_irq0_cfg);


    R_ADC_Read (&g_adc0_ctrl, ADC_CHANNEL_TEMPERATURE, &test_adc);
    // R_ADC_ScanCfg(&g_adc0_ctrl, &g_adc0_channel_cfg);
    //R_CAN_ModeTransition(&g_can0_ctrl, CAN_OPERATION_MODE_NORMAL, CAN_TEST_MODE_LOOPBACK_EXTERNAL);
    R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_04_PIN_07, 1);
    R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_04_PIN_08, 1);
    R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_04_PIN_09, 1);
    //R_CAN_CallbackSet(&g_can0_ctrl, can_callback_args_t, p_context, p_callback_memory);

    R_BSP_SoftwareDelay(500,BSP_DELAY_UNITS_MILLISECONDS);
    g_can_tx_frame.id = 0x80;
    R_BSP_SoftwareDelay(500,BSP_DELAY_UNITS_MILLISECONDS);

    flash_result_t blank_check_result = FLASH_RESULT_BLANK;


    R_FLASH_HP_Open(&g_flash_ctrl, &g_flash_cfg);
    R_FLASH_HP_StartUpAreaSelect(&g_flash_ctrl, FLASH_STARTUP_AREA_BLOCK0, 1);


    while(1){
        flash_hp_data_flash_operations();
        R_BSP_SoftwareDelay(500,BSP_DELAY_UNITS_MILLISECONDS);
        test_flash = *(uint32_t *)FLASH_HP_DF_BLOCK_1;

//        temp = R_BSP_PinRead(BSP_IO_PORT_02_PIN_00);
//        R_ADC_ScanStart(&g_adc0_ctrl);
//        R_ADC_Read (&g_adc0_ctrl, ADC_CHANNEL_16, &test_adc);
//        //temp= 3.3*test_adc/4096;
//        //g_can_tx_frame.data[1]++;
//        g_can_tx_frame.id ++;
//        if( g_can_tx_frame.id >0x89) g_can_tx_frame.id = 0x81;
//        cnt1++;
//       // R_CAN_Write(&g_can0_ctrl, CAN_MAILBOX_NUMBER_0, &g_can_tx_frame);
//        R_BSP_SoftwareDelay(500,BSP_DELAY_UNITS_MILLISECONDS);
//        R_SCI_UART_Write(&g_uart0_ctrl, (uint8_t *)"abooooonb", 6);
    }
}
/* Callback function */
void Ex_IRQ(external_irq_callback_args_t *p_args)
{
    /* TODO: add your own code here */
}

/*******************************************************************************************************************//**
 * This function is called at various points during the startup process.  This implementation uses the event that is
 * called right before main() to set up the pins.
 *
 * @param[in]  event    Where at in the start up process the code is currently at
 **********************************************************************************************************************/
void R_BSP_WarmStart(bsp_warm_start_event_t event)
{
    if (BSP_WARM_START_RESET == event)
    {
#if BSP_FEATURE_FLASH_LP_VERSION != 0

        /* Enable reading from data flash. */
        R_FACI_LP->DFLCTL = 1U;

        /* Would normally have to wait tDSTOP(6us) for data flash recovery. Placing the enable here, before clock and
         * C runtime initialization, should negate the need for a delay since the initialization will typically take more than 6us. */
#endif
    }

    if (BSP_WARM_START_POST_C == event)
    {
        /* C runtime environment and system clocks are setup. */
        /* Configure pins. */
        R_IOPORT_Open (&g_ioport_ctrl, g_ioport.p_cfg);
    }
}

#if BSP_TZ_SECURE_BUILD

BSP_CMSE_NONSECURE_ENTRY void template_nonsecure_callable ();

/* Trustzone Secure Projects require at least one nonsecure callable function in order to build (Remove this if it is not required to build). */
BSP_CMSE_NONSECURE_ENTRY void template_nonsecure_callable ()
{

}
#endif
/* Callback function */
/* Callback function */
void Timer_ISR(timer_callback_args_t *p_args)
{
    //    err = R_ADC_ScanStart(&g_adc0_ctrl);
    //    err = R_ADC_Calibrate(&g_adc0_ctrl, 1);
    //R_CAN_Write(&g_can0_ctrl, CAN_MAILBOX_NUMBER_0, &g_can_tx_frame);
    //R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_01_PIN_11, !R_BSP_PinRead(BSP_IO_PORT_01_PIN_11));
    //R_ADC_Read(&g_adc0_ctrl, ADC_CHANNEL_16, test_adc);
}
/* Callback function */
void Timer_ISR1(timer_callback_args_t *p_args)
{


}

/* Callback function */
void can_callback(can_callback_args_t *p_args)
{

    if (p_args->event== CAN_EVENT_RX_COMPLETE)
    {
        cnt ++ ;
        memcpy(&g_can_rx_frame, p_args->p_frame, sizeof(can_frame_t));  //copy the received data to rx_frame
    }
}
/* Callback function */
void ADC_ISR(adc_callback_args_t *p_args)
{

}


/* Callback function */
//void flash_ISR(flash_callback_args_t *p_args)
//{
//    /* TODO: add your own code here */
//}
/* Callback function */
void Timer_ISR2(timer_callback_args_t *p_args)
{
    /* TODO: add your own code here */
}
/* Callback function */
void user_uart_callback(uart_callback_args_t *p_args)
{

    cnt ++ ;
    /* TODO: add your own code here */
}
/* Callback function */
void ISR_Flash(flash_callback_args_t *p_args)
{
    /* TODO: add your own code here */
}
fsp_err_t flash_hp_data_flash_operations(void)
{
    fsp_err_t err = FSP_SUCCESS;
    flash_result_t blank_check_result = FLASH_RESULT_BLANK;
    uint8_t write_buffer[BLOCK_SIZE] = {RESET_VALUE};
    uint8_t read_buffer[BLOCK_SIZE]= {RESET_VALUE};

    /* Set write buffer, clear read buffer */
    for (uint8_t index = 0; index < BLOCK_SIZE; index++)
    {
        write_buffer[index] = index;
    }

    /* Erase Block */
    err = R_FLASH_HP_Erase(&g_flash_ctrl, FLASH_HP_DF_BLOCK_1, BLOCK_NUM);
    /* Error Handle */
    if (FSP_SUCCESS != err)
    {
     //   APP_ERR_PRINT("\r\nErase API failed, Restart the Application");
        return err;
    }

    /* Wait for the erase complete event flag, if BGO is SET  */
//    if (true == g_flash_cfg.data_flash_bgo)
//    {
//    //    APP_PRINT("\r\n BGO has enabled");
//        while (!g_b_flash_event_erase_complete);
//        g_b_flash_event_erase_complete = false;
//    }
    //APP_PRINT("\r\nErase successful");

    /* Data flash blank check */
    err = R_FLASH_HP_BlankCheck(&g_flash_ctrl, FLASH_HP_DF_BLOCK_1,FLASH_HP_DF_BLOCK_SIZE, &blank_check_result);
    /* Error Handle */
    if (FSP_SUCCESS != err)
    {
        //APP_ERR_PRINT("\r\nBlankCheck API failed, Restart the Application");
        return err;
    }

    /* Validate the blank check result */
//    if (FLASH_RESULT_BLANK == blank_check_result)
//    {
//        //APP_PRINT("\r\n BlankCheck is successful");
//    }
//    else if (FLASH_RESULT_NOT_BLANK == blank_check_result)
//    {
//        //APP_ERR_PRINT("\r\n BlankCheck is not blank,not to write the data. Restart the application");
//        return (fsp_err_t)FLASH_RESULT_NOT_BLANK;
//    }
//    else if (FLASH_RESULT_BGO_ACTIVE == blank_check_result)
//    {
//        /* BlankCheck will update in Callback */
//        /* Event flag will be updated in the blank check function when BGO is enabled */
//        err = blankcheck_event_flag();
//        if(FSP_SUCCESS != err)
//        {
//            return err;
//        }
//    }
//    else
//    {
//        /* No Operation */
//    }

    /* Write code flash data*/
    write_buffer[0] = 0x22;
    write_buffer[1] = 0x33;
    err = R_FLASH_HP_Write(&g_flash_ctrl, (uint32_t) write_buffer,FLASH_HP_DF_BLOCK_1, 2);
    /* Error Handle */
    if (FSP_SUCCESS != err)
    {
       // APP_ERR_PRINT("\r\nWrite API failed, Restart the Application");
        return err;
    }
    /* Wait for the write complete event flag, if BGO is SET  */
//    if (true == g_flash_cfg.data_flash_bgo)
//    {
//        while (!g_b_flash_event_write_complete);
//        g_b_flash_event_write_complete = false;
//    }

   // APP_PRINT("\r\nWriting flash data is successful\r\n");

    /*Read code flash data */
    memcpy(read_buffer, (uint8_t *) FLASH_HP_DF_BLOCK_1, BLOCK_SIZE);

    /* comparing the write_buffer and read_buffer */
    if (RESET_VALUE == memcmp(read_buffer, write_buffer, BLOCK_SIZE))
    {
        //APP_PRINT("\r\nRead and Write buffer is verified and successful");
        /* Print the read data on the RTT terminal */
        //APP_PRINT("\r\nRead Data : \r\n");
    }
    else
    {
        //APP_PRINT("Read and Write buffer is verified and not successful");
        return FSP_ERR_WRITE_FAILED;
    }

    return err;
}
