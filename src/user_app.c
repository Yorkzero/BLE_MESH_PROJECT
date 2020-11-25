/*************************************************************
Copyright (c) 2020 Shawn Yan, All rights reserved
-------------------------------------------------------------
File Name: user_app.c

Desc     : Ó¦ÓÃ²ãº¯Êý

Author   : Shawn Yan

Date     : 2020-11-23

*************************************************************/
/*------------------------- Includes -----------------------*/
#include "user_app.h"

/*----------- Global Definitions and Declarations ----------*/


/*-------------------- Type Declarations -------------------*/


/*------------------ Variable Declarations -----------------*/
volatile uint8_t AT_flag = 0;//AT cmd recorder

/*------------------- Function Prototype -------------------*/


/*------------------- Function Implement -------------------*/
/*************************************************************
Function Name       : AT_Test_Demo
Function Description: AT instuction test
Param_in            : 
Param_out           : 
Return Type         : 
Note                : 
Author              : Yan
Time                : 2020-11-23
*************************************************************/
void AT_Test_Demo(void)
{
    if (AT_flag != key_flag)
    {
        LEDR_H();
        USART1_SendWord("+++a");
        delay_ms_1(200);
        USART1_SendWord("AT+NAME=YSY\r\n");
        delay_ms_1(200);
        
        USART1_SendWord("AT+ENTM\r\n");
        AT_flag = key_flag;
        LEDR_L();
    }
    
}
/*************************************************************
Function Name       : link_sta_detec
Function Description: ble Link state detection
Param_in            : 
Param_out           : 
Return Type         : 
Note                : use green led to show the ble state
Author              : Yan
Time                : 2020-11-23
*************************************************************/
void link_sta_detec(void)
{
    delay_ms_1(20);//avoid shaking
    if (BLE_STA_READ())//high level
    {
        LEDG_L();
        //LEDR_H();
    }
    if (!BLE_STA_READ())//low level
    {
        LEDG_H();
        //LEDR_L();
    }
    
    
}
/*************************************************************
Function Name       : key_led_run
Function Description: use key to control led
Param_in            : 
Param_out           : 
Return Type         : 
Note                : 
Author              : Yan
Time                : 2020-11-25
*************************************************************/
void key_led_run(void)
{
    if(0 == key_flag)
        LEDR_L();
    if(1 == key_flag)
        LEDR_H();
}
/*--------------------------- END --------------------------*/