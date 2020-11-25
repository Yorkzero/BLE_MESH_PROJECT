/*************************************************************
Copyright (c) 2020 Shawn Yan, All rights reserved
-------------------------------------------------------------
File Name: bsp_conf.h

Desc     : 底层驱动关联头文件

Author   : Shawn Yan

Date     : 2020-11-20

*************************************************************/
#ifndef _BSP_CONF_H_
#define _BSP_CONF_H_
/*------------------------- Includes -----------------------*/
#include "main.h"

/*----------- Global Definitions and Declarations ----------*/


/*-------------------- Type Declarations -------------------*/


/*------------------ Variable Declarations -----------------*/


/*------------------- Function Prototype -------------------*/
/*************************************************************
Function Name       : bsp_gpio_init
Function Description: gpio initialization
Param_in            : 
Param_out           : 
Return Type         : 
Note                : 
Author              : Yan
Time                : 2020-11-20
*************************************************************/
void bsp_gpio_init(void);
/*************************************************************
Function Name       : bsp_clk_init
Function Description: system clk initializatiom
Param_in            : 
Param_out           : 
Return Type         : 
Note                : 
Author              : Yan
Time                : 2020-11-20
*************************************************************/
void bsp_clk_init(void);
/*************************************************************
Function Name       : bsp_key_it
Function Description: key state detection
Param_in            : 
Param_out           : 
Return Type         : 
Note                : 
Author              : Yan
Time                : 2020-11-20
*************************************************************/
void bsp_key_it(void);
/*************************************************************
Function Name       : bsp_uart_init
Function Description: uart initialization
Param_in            : 
Param_out           : 
Return Type         : 
Note                : 
Author              : Yan
Time                : 2020-11-20
*************************************************************/
void bsp_uart_init(void);
/*************************************************************
Function Name       : USART1_SendWord
Function Description: Transmits 8 bit *data through the USART1 peripharal
Param_in            : *Data
Param_out           : 
Return Type         : 
Note                : eg. USART1_SendWord("AT+NAME/r/n")
Author              : Yan
Time                : 2020-11-23
*************************************************************/
void USART1_SendWord(uint8_t *Data);


//模拟串口打印
#if (SIM_UART_PRINTF_EN)
extern void sim_uart_printf(uint8_t data);
extern void sim_uart_printf_it(uint8_t data);

extern void sim_printf_string(uint8_t *str);
extern void sim_printf_hex(uint8_t data);

#else
#define sim_uart_printf(N)
#define sim_uart_printf_it(N)
#define sim_printf_string(N)
#define sim_printf_hex(N)
#endif
/*------------------- Function Implement -------------------*/

#endif
/*--------------------------- END --------------------------*/