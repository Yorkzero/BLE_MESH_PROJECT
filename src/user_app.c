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
    
}
/*************************************************************
Function Name       : link_sta_detec
Function Description: ble Link state detection in IT
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
        LEDR_H();
    }
    if (!BLE_STA_READ())//low level
    {
        LEDG_H();
        LEDR_L();
    }
    
    
}
/*--------------------------- END --------------------------*/