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
    // AT_Send("+++a");
    // // AT_Send("AT+MODE?\r\n");
    // AT_Send("AT+ENTM\r\n");
    while(1)
    {
        user_app_run(); 
        BLE_status_run();
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