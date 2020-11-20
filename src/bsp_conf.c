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

/*************************************************************
Function Name       : sim_uart_printf
Function Description: 模拟串口打印
Param_in            : data
Param_out           : 
Return Type         : 
Note                : PB0端口打印
Author              : Yan
Time                : 2020-11-20
*************************************************************/
#if (SIM_UART_PRINTF_EN)
void sim_uart_printf(uint8_t data)
{
    //PB_ODR |= bit0;
    //PB_DDR |= bit0;
    //PB_CR1 |= bit0;

    asm("SIM");  //关总中断

    //发送起始位
    GPIOB->ODR &= ~GPIO_Pin_0;

#if (SYS_CLK_FREQ == SYS_CLK_FREQ_16M)
asm
(
    //保存本程序使用到的寄存器
    "PUSHW Y\n"  //2T
    "PUSHW X\n"  //2T
    
    "LDW Y,#9\n"  //2T用来控制循环
    "LDW X,#41\n" //2T，赋值，实测得到
"Delay0: DECW X\n" //减1，1个指令周期
    "JRNE Delay0\n" //判断跳转，跳转要2T，不跳要1T
    "NOP\n"

    //到设置新的电平，需要6T
"SEND_DATA:DECW Y\n"  //1T,判断循环是否结束
    "JREQ STOP\n"//结束则跳转，2T，不跳转则1T
    "NOP\n"//1T，补偿未跳转的1T
    
    "SRL A\n" //1T,逻辑右移，同时bit0在C标志位中
    "JRC SET1\n"  //等于1则跳转，跳转2T，不跳转1T
    "NOP\n"//1T，补偿未跳转的1T
    "BRES 0x5005,#0\n" //1T，输出低
    "JP Delay1\n"  //1T，无条件跳转
"SET1:BSET 0x5005,#0\n" //1T，输出高
    "NOP\n"

"Delay1:NOP\n"
    "LDW X,#41\n" //2T，实测得到
"LOOP1:DECW X\n"  //1T
    "JRNE LOOP1\n"//跳转则2T，不跳转则1T
    
    "JP SEND_DATA\n"  //1T，无条件跳转


    //发送2位时长的停止位
"STOP:TNZW X\n" //2T,只是为了延时2T
    "TNZW X\n" //2T,只是为了延时2T
    "BSET 0x5005,#0\n" //1T，输出高
    "LDW X,#92\n" //2T，延时139x2-2=276T
"LOOP2:DECW X\n"  //1T
    "JRNE LOOP2\n"//跳转则2T，不跳转则1T
    "NOP\n"

    //恢复本程序使用到的寄存器
    "POPW X\n"
    "POPW Y"
)       ;
#elif (SYS_CLK_FREQ == SYS_CLK_FREQ_4M)
    //每个bit延时34T
asm
(
    //保存本程序使用到的寄存器
    "PUSHW Y\n"
    "PUSHW X\n"
    
    "LDW Y,#9\n"  //2T用来控制循环
    "LDW X,#6\n" //2T，赋值
    //本段延时19T
"Delay0: DECW X\n" //减1，1个指令周期
    "JRNE Delay0\n" //判断跳转，跳转要2T，不跳要1T
    "NOP\n"
    "NOP\n"

    //到设置新的电平，需要6T
"SEND_DATA:DECW Y\n"  //1T,判断循环是否结束
    "JREQ STOP\n"//结束则跳转，2T，不跳转则1T
    "NOP\n"//1T，补偿未跳转的1T

    "SRL A\n" //1T,逻辑右移，同时bit0在C标志位中
    "JRC SET1\n"  //等于1则跳转，跳转2T，确认不跳转1T
    //"NOP\n"//1T，补偿未跳转的1T，实测调整
    "BRES 0x5005,#0\n" //1T，输出低
    "JP Delay1\n"  //1T，无条件跳转
"SET1:BSET 0x5005,#0\n" //1T，输出高
    "NOP\n"
    "NOP\n" //实测调整
    "NOP\n"

    //到下个bit变化前，需要再延时26T
"Delay1:LDW X,#7\n" //2T，
    //本段延时21T
"LOOP1:DECW X\n"  //1T
    "JRNE LOOP1\n"//跳转则2T，不跳转则1T
    "JP SEND_DATA\n"  //1T，无条件跳转


    //发送2位时长的停止位
"STOP:TNZW X\n" //2T,只是为了延时2T
    "NOP\n" //1T,只是为了延时1T
    "BSET 0x5005,#0\n" //1T，输出高

    //需要延时68T
    "LDW X,#22\n" //2T
    //本段总延时66T
"LOOP2:DECW X\n"  //1T
    "JRNE LOOP2\n"//跳转则2T，不跳转则1T
    "NOP\n"

    //恢复本程序使用到的寄存器
    "POPW X\n"
    "POPW Y"    
);
#endif
    asm("RIM");	//使能全局中断
}

void sim_uart_printf_it(uint8_t data)
{
    //PB_ODR |= bit0;
    //PB_DDR |= bit0;
    //PB_CR1 |= bit0;

    //asm("SIM");  //关总中断

    //发送起始位
    GPIOB->ODR &= ~GPIO_Pin_0;

#if (SYS_CLK_FREQ == SYS_CLK_FREQ_16M)
asm
(
    //保存本程序使用到的寄存器
    "PUSHW Y\n"  //2T
    "PUSHW X\n"  //2T
    
    "LDW Y,#9\n"  //2T用来控制循环
    "LDW X,#41\n" //2T，赋值，实测得到
"Delay0: DECW X\n" //减1，1个指令周期
    "JRNE Delay0\n" //判断跳转，跳转要2T，不跳要1T
    "NOP\n"

    //到设置新的电平，需要6T
"SEND_DATA:DECW Y\n"  //1T,判断循环是否结束
    "JREQ STOP\n"//结束则跳转，2T，不跳转则1T
    "NOP\n"//1T，补偿未跳转的1T
    
    "SRL A\n" //1T,逻辑右移，同时bit0在C标志位中
    "JRC SET1\n"  //等于1则跳转，跳转2T，不跳转1T
    "NOP\n"//1T，补偿未跳转的1T
    "BRES 0x5005,#0\n" //1T，输出低
    "JP Delay1\n"  //1T，无条件跳转
"SET1:BSET 0x5005,#0\n" //1T，输出高
    "NOP\n"

"Delay1:NOP\n"
    "LDW X,#41\n" //2T，实测得到
"LOOP1:DECW X\n"  //1T
    "JRNE LOOP1\n"//跳转则2T，不跳转则1T
    
    "JP SEND_DATA\n"  //1T，无条件跳转


    //发送2位时长的停止位
"STOP:TNZW X\n" //2T,只是为了延时2T
    "TNZW X\n" //2T,只是为了延时2T
    "BSET 0x5005,#0\n" //1T，输出高
    "LDW X,#92\n" //2T，延时139x2-2=276T
"LOOP2:DECW X\n"  //1T
    "JRNE LOOP2\n"//跳转则2T，不跳转则1T
    "NOP\n"

    //恢复本程序使用到的寄存器
    "POPW X\n"
    "POPW Y"
)       ;
#elif (SYS_CLK_FREQ == SYS_CLK_FREQ_4M)
    //每个bit延时34T
asm
(
    //保存本程序使用到的寄存器
    "PUSHW Y\n"
    "PUSHW X\n"
    
    "LDW Y,#9\n"  //2T用来控制循环
    "LDW X,#6\n" //2T，赋值
    //本段延时19T
"Delay0: DECW X\n" //减1，1个指令周期
    "JRNE Delay0\n" //判断跳转，跳转要2T，不跳要1T
    "NOP\n"
    "NOP\n"

    //到设置新的电平，需要6T
"SEND_DATA:DECW Y\n"  //1T,判断循环是否结束
    "JREQ STOP\n"//结束则跳转，2T，不跳转则1T
    "NOP\n"//1T，补偿未跳转的1T

    "SRL A\n" //1T,逻辑右移，同时bit0在C标志位中
    "JRC SET1\n"  //等于1则跳转，跳转2T，确认不跳转1T
    //"NOP\n"//1T，补偿未跳转的1T，实测调整
    "BRES 0x5005,#0\n" //1T，输出低
    "JP Delay1\n"  //1T，无条件跳转
"SET1:BSET 0x5005,#0\n" //1T，输出高
    "NOP\n"
    "NOP\n" //实测调整
    "NOP\n"

    //到下个bit变化前，需要再延时26T
"Delay1:LDW X,#7\n" //2T，
    //本段延时21T
"LOOP1:DECW X\n"  //1T
    "JRNE LOOP1\n"//跳转则2T，不跳转则1T
    "JP SEND_DATA\n"  //1T，无条件跳转


    //发送2位时长的停止位
"STOP:TNZW X\n" //2T,只是为了延时2T
    "NOP\n" //1T,只是为了延时1T
    "BSET 0x5005,#0\n" //1T，输出高

    //需要延时68T
    "LDW X,#22\n" //2T
    //本段总延时66T
"LOOP2:DECW X\n"  //1T
    "JRNE LOOP2\n"//跳转则2T，不跳转则1T
    "NOP\n"

    //恢复本程序使用到的寄存器
    "POPW X\n"
    "POPW Y"    
);
#endif
//    asm("RIM");	//使能全局中断
}


void sim_printf_string(uint8_t *str)
{
    if(0 == str)
        return;
    
    while(*str)
    {
        sim_uart_printf(*str++);
    }
}

const uint8_t hex_tab[16]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
void sim_printf_hex(uint8_t data)
{
    sim_uart_printf('0');
    sim_uart_printf('x');
    sim_uart_printf(hex_tab[data >> 4]);
    sim_uart_printf(hex_tab[data & 0x0F]);
    sim_uart_printf(' ');
}

#endif
/*--------------------------- END --------------------------*/