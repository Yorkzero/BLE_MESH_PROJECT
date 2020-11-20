/*************************************************************
Copyright (c) 2020 Shawn Yan, All rights reserved
-------------------------------------------------------------
File Name: bsp_conf.c

Desc     : 用于存放各类底层驱动文件

Author   : Shawn Yan

Date     : 2020-11-20

*************************************************************/

/*------------------------- Includes -----------------------*/
#include "bsp_conf.h"

/*----------- Global Definitions and Declarations ----------*/


/*-------------------- Type Declarations -------------------*/


/*------------------ Variable Declarations -----------------*/


/*------------------- Function Prototype -------------------*/


/*------------------- Function Implement -------------------*/
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
void bsp_gpio_init(void)
{
    GPIO_Init(LEDG_PORT, LEDG_PIN, GPIO_Mode_Out_PP_Low_Slow);       //Green led init
    GPIO_Init(LEDR_PORT, LEDR_PIN, GPIO_Mode_Out_PP_Low_Slow);       //Red led init
    GPIO_Init(LOWV_PORT, LOWV_PIN, GPIO_Mode_In_FL_No_IT);           //Low pwr detection, opening interrupt when used.
    GPIO_Init(KEY_PORT, KEY_PIN, GPIO_Mode_In_PU_No_IT);             //key detection, opening interrupt when used.
    GPIO_Init(BLE_RST_PORT, BLE_RST_PIN, GPIO_Mode_Out_PP_Low_Slow); //ble chip enable state
    GPIO_Init(BEEP_PORT, BEEP_PIN, GPIO_Mode_Out_PP_Low_Slow);       //beep init
    GPIO_Init(UART_RX_PORT, UART_RX_PIN, GPIO_Mode_In_PU_No_IT);     //UART receive init
    GPIO_Init(UART_TX_PORT, UART_TX_PIN, GPIO_Mode_Out_PP_Low_Fast); //UART transmitt init
    //Set unused pin mode: IN_PU_NO_IT
    GPIO_Init(GPIOA, PA_UNUSED_PIN, GPIO_Mode_In_PU_No_IT);
    GPIO_Init(GPIOB, PB_UNUSED_PIN, GPIO_Mode_In_PU_No_IT);
    GPIO_Init(GPIOC, PC_UNUSED_PIN, GPIO_Mode_In_PU_No_IT);
    GPIO_Init(GPIOD, PD_UNUSED_PIN, GPIO_Mode_In_PU_No_IT);
}
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
void bsp_clk_init(void)
{
    CLK_HSICmd(ENABLE);//internal RC 16M enable
    CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSI);//choose HSI as system clk
    CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);// div 1

    //peripharal clk state
    //comment when used
  	CLK_PeripheralClockConfig(CLK_Peripheral_TIM1,DISABLE);
    CLK_PeripheralClockConfig(CLK_Peripheral_TIM2,DISABLE);
  	CLK_PeripheralClockConfig(CLK_Peripheral_TIM3,DISABLE);
	CLK_PeripheralClockConfig(CLK_Peripheral_TIM4,DISABLE);
	CLK_PeripheralClockConfig(CLK_Peripheral_I2C1,DISABLE);
	CLK_PeripheralClockConfig(CLK_Peripheral_SPI1,DISABLE);
	//CLK_PeripheralClockConfig(CLK_Peripheral_USART1,DISABLE);
	CLK_PeripheralClockConfig(CLK_Peripheral_BEEP,DISABLE);
	CLK_PeripheralClockConfig(CLK_Peripheral_ADC1,DISABLE);
	CLK_PeripheralClockConfig(CLK_Peripheral_RTC,DISABLE);
}
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
void bsp_key_it(void)
{

}
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
void bsp_uart_init(void)
{
    CLK_PeripheralClockConfig(CLK_Peripheral_USART1, ENABLE);
    //remappin
    //SYSCFG_REMAPPinConfig(REMAP_Pin_USART1TxRxPortC, ENABLE);
    /*USART1 ,
    *BR: 57600, 
    *8bit, 
    *1stop bit, 
    *no parity, 
    *RX/TX mode 
    */
    USART_Init(USART1, (uint32_t)57600, USART_WordLength_8b, USART_StopBits_1, USART_Parity_No, (USART_Mode_TypeDef)(USART_Mode_Tx | USART_Mode_Rx));

    USART_ITConfig(USART1, USART_IT_OR, ENABLE);
    USART_Cmd(USART1, ENABLE);
}

/*--------------------------- END --------------------------*/