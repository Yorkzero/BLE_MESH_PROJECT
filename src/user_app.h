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
extern volatile uint8_t AT_flag;//AT cmd recorder

/*-------------------- Type Declarations -------------------*/
#define DEBUG_STATUS 0 //record the status of function

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
Function Name       : AT_Send
Function Description: send AT cmd
Param_in            : uint8_t *atcmd
Param_out           : 
Return Type         : u16 tag
Note                : 0: succeed/1: failed
Author              : Yan
Time                : 2020-11-27
*************************************************************/
uint8_t AT_Send(uint8_t *atcmd);
/*************************************************************
Function Name       : AT_Get_State
Function Description: use AT to get BLE state
Param_in            : char *sta
                      allow to query the following status:
                      NAME/MODE/MAC/CIVER/TPL/PASS/PASSEN/
                      UART/UARTIM/AUTOSLEEP/DEEPSLEEP/
                      HIBERNATE/HELLO/LINK/UUID etc.
Param_out           : USART1_STA_buf[]
Return Type         : u16 flag
Note                : 0: succeed/1: failed
Author              : Yan
Time                : 2020-11-27
*************************************************************/
uint8_t AT_Get_State(char *sta);
#if DEBUG_STATUS
/*************************************************************
Function Name       : BLE_AT_Init
Function Description: Bluetooth initialization
Param_in            : char *name, char *mode
                      M: master/S: slave/F: mesh/B: broadcast or iBeacon
Param_out           : 
Return Type         : u16 tag 
Note                : 0: succeed/1: failed
Author              : Yan
Time                : 2020-11-27
------------------------------------------
log                 : 2020-11-28
                      debug ing, can not be used!
*************************************************************/
uint8_t BLE_AT_Init(char *name, char *mode);
#endif
/*************************************************************
Function Name       : Norm_Send
Function Description: Send data to master through BLE
Param_in            : u8 *data
Param_out           : 
Return Type         : u16 tag 
Note                : 0: succeed/1: failed
Author              : Yan
Time                : 2020-11-28
*************************************************************/
uint8_t Norm_Send(uint8_t *data);
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
void key_led_run(void);
/*------------------- Function Implement -------------------*/

#endif
/*--------------------------- END --------------------------*/