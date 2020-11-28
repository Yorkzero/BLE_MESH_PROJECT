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
Function Name       : AT_Send
Function Description: send AT cmd
Param_in            : uint8_t *atcmd
Param_out           : 
Return Type         : u16 tag
Note                : 0: succeed/1: failed
Author              : Yan
Time                : 2020-11-27
*************************************************************/
uint8_t AT_Send(uint8_t *atcmd)
{
    uint16_t tag = 1;
    uint8_t t;
    uint8_t retry = 10;//number of AT command sending attempts
    while (retry--)
    {
        USART1_SendWord(atcmd);
        delay_ms_1(10);
        for (t = 0; t < 10; t++)
        {
            if(USART1_RX_STA & 0x8000)
                break;
            delay_ms_1(5);
        }
        if ((USART1_RX_STA & 0x8000))//receive the data
        {
            tag = USART1_RX_STA & 0x7FFF;//get the length of data
            USART1_RX_STA = 0;//clear the state flag
            if (('O' == USART1_RX_buf[tag-4]) &&
                ('K' == USART1_RX_buf[tag-3]) )
            {
                tag = 0;//enter succeed
                break;
            }
        }
    }
    //clear the rx buffer
    memset(USART1_RX_buf, 0, sizeof(USART1_RX_buf));
    if(0 == retry)tag = 1;//enter failed
    return tag;
}
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
uint8_t AT_Get_State(char *sta)
{
    uint16_t tag = 1;
    uint16_t temp = 1;
    uint8_t t;
    uint8_t retry = 10;
    uint8_t stalen = 0;
    char *stastring = connect3("AT+", sta, "?\r\n");
    while (*sta)    //get the length of sta
    {
        *sta ++;
        stalen += 1;
    }
    while (retry--)
    {
        USART1_SendWord(stastring);
        delay_ms_1(10);
        for ( t = 0; t < 10; t++)//50ms outtime
        {
            if(USART1_RX_STA & 0x8000)
                break;
            delay_ms_1(5);
        }
        if ((USART1_RX_STA & 0x8000))//receive the data
        {
            temp = USART1_RX_STA & 0x7FFF;//get the length of data
            USART1_RX_STA = 0;//clear the state flag
            if (
                ('O' == USART1_RX_buf[tag-4]) &&
                ('K' == USART1_RX_buf[tag-3]) )
            {
                tag = 0;//enter succeed 
                break;  
            }
            
        }
    }
    if(!tag && (0 != retry))
    {
        uint16_t i = 0;  
        for (t = 4 + stalen; t < (temp - 6); t++)
        {   //storage the relevant data in the buffer, empty it when access.   
            USART1_STA_buf[i] = USART1_RX_buf[t-1];
            i++;
        }
    }
    if(0 == retry)tag = 1;//enter failed
    return tag;
}
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
*************************************************************/
uint8_t BLE_AT_Init(char *name, char *mode)
{
    uint16_t tag  = 1;
    uint16_t tag1 = 1;
    uint16_t tag2 = 1;
    uint8_t t;
    uint8_t retry = 10;  //number of AT command sending attempts
    // char *namestr = connect2("AT+NAME=", name);
    // char *modestr = connect2("AT+MODE=", mode);
    while (retry--)
    {
        ENTER_AT();//Enter AT command mode
        delay_ms_1(10);
        for ( t = 0; t < 10; t++)//50ms outtime
        {
            if(USART1_RX_STA & 0x8000)
                break;
            delay_ms_1(5);
        }
        if ((USART1_RX_STA & 0x8000))//receive the data
        {
            tag = USART1_RX_STA & 0x7FFF;//get the length of data
            USART1_RX_STA = 0;//clear the state flag
            //if (tag == 6
            // && ('a' == USART1_RX_buf[0]) && ('+' == USART1_RX_buf[1]) && ('O' == USART1_RX_buf[2]) && ('K' == USART1_RX_buf[3]))
            {
                tag = 0;//enter succeed 
                break;  
            }
            
        }
    }
    
    if(!tag && (0 != retry))
    {
        tag1 = AT_Send((uint8_t *)(connect3("AT+NAME=", name, "\r\n")));
        tag2 = AT_Send((uint8_t *)(connect3("AT+MODE=", mode, "\r\n")));
    }    
    EXIT_AT();//exit AT command mode
    
    if ((0 == retry) || tag1 || tag2)
    {
        tag = 1;//init failed
    }
    return tag;
    
}
#endif
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