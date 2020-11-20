/*************************************************************
Copyright (c) 2020 Shawn Yan, All rights reserved
-------------------------------------------------------------
File Name: main.h

Desc     : 蓝牙mesh项目

Author   : Shawn Yan

Date     : 2020-11-20

*************************************************************/
#ifndef _MAIN_H_
#define _MAIN_H_
/*------------------------- Includes -----------------------*/
#include <stdio.h>    /*<std io driver>*/
#include "stm8l15x.h" /*<std peripharal driver>*/
#include "bsp_conf.h" /*<program bsp driver>*/

/*----------- Global Definitions and Declarations ----------*/
//GPIO Definition
#define LEDG_PORT       GPIOC                     //LED GREEN
#define LEDG_PIN        GPIO_Pin_5                //LED GREEN
#define LEDR_PORT       GPIOC                     //LED RED
#define LEDR_PIN        GPIO_Pin_6                //LED RED
#define LEDG_H()        LEDG_PORT->ODR | = LEDG_PIN   //LEDG Level high
#define LEDG_L()        LEDG_PORT->ODR & = ~LEDG_PIN  //LEDG Level low
#define LEDG_R()        LEDG_PORT->ODR ^ = LEDG_PIN   //LEDG Level reversed
#define LEDR_H()        LEDR_PORT->ODR | = LEDR_PIN   //LEDR Level high
#define LEDR_L()        LEDR_PORT->ODR & = ~LEDR_PIN  //LEDR Level low
#define LEDR_R()        LEDR_PORT->ODR ^ = LEDR_PIN   //LEDG Level reversed

#define LOWV_PORT       GPIOD      //low power detec
#define LOWV_PIN        GPIO_Pin_4 //low power detec
#define LOWV_ENABLE()   GPIO_Init(LOWV_PORT, LOWV_PIN, GPIO_Mode_In_FL_IT)//LOWV IT
#define LOWV_DISABLE()  GPIO_Init(LOWV_PORT, LOWV_PIN, GPIO_Mode_In_FL_NO_IT)//LOWV NO IT

#define KEY_PORT        GPIOB     //key detec
#define KEY_PIN         GPIO_Pin_4 //key detec
#define KEY_ENABLE()    GPIO_Init(KEY_PORT, KEY_PIN, GPIO_Mode_In_PU_IT);//KEY IT
#define KEY_DISABLE()   GPIO_Init(KEY_PORT, KEY_PIN, GPIO_Mode_In_PU_No_IT);//KEY NO IT

#define BLE_RST_PORT    GPIOD      //ble reset
#define BLE_RST_PIN     GPIO_Pin_1 //ble reset

#define BEEP_PORT       GPIOB      //beep
#define BEEP_PIN        GPIO_Pin_0 //beep
#define BEEP_H()        BEEP_PORT->ODR | = BEEP_PIN   //BEEP Level high
#define BEEP_L()        BEEP_PORT->ODR & = ~BEEP_PIN  //BEEP Level low
#define BEEP_R()        BEEP_PORT->ODR ^ = BEEP_PIN   //BEEP Level reversed

#define UART_RX_PORT    GPIOC       //UART RX
#define UART_RX_PIN     GPIO_Pin_2  //UART RX
#define UART_TX_PORT    GPIOC       //UART TX
#define UART_TX_PIN     GPIO_Pin_3  //UART TX
#define LINK_PORT       GPIOC       //BLE LINK STA
#define LINK_PIN        GPIO_Pin_4  //BLE LINK STA

//Unused GPIO Definition
#define PA_UNUSED_PIN   (GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6)
#define PB_UNUSED_PIN   (GPIO_Pin_3 | GPIO_Pin_5 | GPIO_Pin_7)
#define PC_UNUSED_PIN   (GPIO_Pin_0 | GPIO_Pin_1)
#define PD_UNUSED_PIN   (GPIO_Pin_0 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7)

/*-------------------- Type Declarations -------------------*/


/*------------------ Variable Declarations -----------------*/
//系统时钟值宏定义
#define SYS_CLK_FREQ_16M       16000000
#define SYS_CLK_FREQ_8M        8000000
#define SYS_CLK_FREQ_4M        4000000
#define SYS_CLK_FREQ_2M        2000000
#define SYS_CLK_FREQ_1M        1000000
#define SYS_CLK_FREQ_500K      500000
#define SYS_CLK_FREQ_250K      250000
#define SYS_CLK_FREQ_125K      125000


//配置系统时钟，在初始化时钟时根据此定义初始化，注：本系统使用内部16MHz HSI时钟源
#define SYS_CLK_FREQ          SYS_CLK_FREQ_16M    

#define SIM_UART_PRINTF_EN 0 //模拟打印开关
#define CODE_VERSION 1       //用于版本号控制
/*------------------- Function Prototype -------------------*/


/*------------------- Function Implement -------------------*/

#endif
/*--------------------------- END --------------------------*/