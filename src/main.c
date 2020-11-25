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
#if (1 == CODE_VERSION)//Version 1 
    bsp_clk_init();
    bsp_gpio_init();
    bsp_uart_init();
    delay_ms_1(10);
    
    delay_ms_1(100);
    
    LINK_ENABLE();
    //KEY_ENABLE();
    delay_ms_1(100);
    __enable_interrupt();
    while(1)
    {
        
        
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