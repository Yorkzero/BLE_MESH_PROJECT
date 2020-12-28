/*************************************************************
Copyright (c) 2020 Shawn Yan, All rights reserved
-------------------------------------------------------------
File Name: main.c

Desc     : À¶ÑÀmeshÏîÄ¿

Author   : Shawn Yan

Date     : 2020-11-20

*************************************************************/

/*------------------------- Includes -----------------------*/
#include "main.h"

/*----------- Global Definitions and Declarations ----------*/


/*-------------------- Type Declarations -------------------*/


/*------------------ Variable Declarations -----------------*/


/*------------------- Function Prototype -------------------*/


/*------------------- Function Implement -------------------*/

/*************************************************************
Function Name       : main
Function Description: 
Param_in            : 
Param_out           : 
Return Type         : 
Note                : 
Author              : Yan
Time                : 2020-11-20
*************************************************************/
int main(void)
{ 
    //system init
    bsp_clk_init();
    bsp_gpio_init();
    bsp_uart_init(); 
#if (SIM_UART_PRINTF_EN)
    GPIO_Init(SIM_TX_PORT, SIM_TX_PIN, GPIO_Mode_Out_PP_High_Fast);
#endif
#if (1 == CODE_VERSION)//Version 1     
    KEY_ENABLE();
    // LINK_ENABLE();
    delay_ms_1(100);
    __enable_interrupt();
    // AT_Send("+++a");
    // // AT_Send("AT+MODE?\r\n");
    // AT_Send("AT+ENTM\r\n");
    while(1)
    {
       BLE_status_run();
       key_led_run();
    }
#endif
#if (2 == CODE_VERSION)//Version 2     
    KEY_ENABLE();
    // LINK_ENABLE();
    delay_ms_1(100);
    __enable_interrupt();
    delay_ms_1(500);
    //init
    memset(USART1_RX_buf, 0, sizeof(USART1_RX_buf));
    USART1_RX_STA = 0;    
#if (RELAY_DEV == DEVICE_ID)
    MESH_cmd(DISABLE);
    BLE_status_it();
    SYS_STA_flag = 1;
    
#else
    MESH_cmd(ENABLE);
    BLE_status_it();
    PWR_UltraLowPowerCmd(ENABLE);
#endif
    delay_ms_1(500);
    // AT_Send("+++a");
    // // AT_Send("AT+MODE?\r\n");
    // AT_Send("AT+ENTM\r\n");
    while(1)
    {
        if (0 == SYS_STA_flag)//halt
        {
            USART_Cmd(USART1, DISABLE);
            CLK_PeripheralClockConfig(CLK_Peripheral_USART1, DISABLE);
            
            CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, DISABLE);
            CLK_PeripheralClockConfig(CLK_Peripheral_TIM3, DISABLE);
            CLK_PeripheralClockConfig(CLK_Peripheral_TIM4,DISABLE);

            GPIO_Init(UART_RX_PORT, UART_RX_PIN, GPIO_Mode_In_PU_IT);      //UART receive init
            halt();
        }
        if (1 == SYS_STA_flag)//run
            user_app_run(); 
        // BLE_status_run();
    }
#endif
}

#ifdef USE_FULL_ASSERT
void assert_failed(u8* file,u32 line)
{
    while(1)
    {
    }
}
#endif
/*--------------------------- END --------------------------*/