/*************************************************************
Copyright (c) 2020 Shawn Yan, All rights reserved
-------------------------------------------------------------
File Name: user_app.h

Desc     : 应用层函数头文件

Author   : Shawn Yan

Date     : 2020-11-23

*************************************************************/
#ifndef _USER_APP_H_
#define _USER_APP_H_
/*------------------------- Includes -----------------------*/
#include "main.h"

/*----------- Global Definitions and Declarations ----------*/


/*-------------------- Type Declarations -------------------*/


/*------------------ Variable Declarations -----------------*/


/*------------------- Function Prototype -------------------*/
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
void AT_Test_Demo(void);
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
void link_sta_detec(void);
/*------------------- Function Implement -------------------*/

#endif
/*--------------------------- END --------------------------*/