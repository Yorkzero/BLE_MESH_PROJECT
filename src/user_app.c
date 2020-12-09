/*************************************************************
Copyright (c) 2020 Shawn Yan, All rights reserved
-------------------------------------------------------------
File Name: user_app.c

Desc     : user application

Author   : Shawn Yan

Date     : 2020-11-23

*************************************************************/
/*------------------------- Includes -----------------------*/
#include "user_app.h"

/*----------- Global Definitions and Declarations ----------*/


/*-------------------- Type Declarations -------------------*/

/*------------------ Variable Declarations -----------------*/
volatile uint8_t BLE_STA_flag = 1;//BLE state flag 0:MESH, 1:NON-MESH
#if (1 == DEVICE_ID)
volatile uint8_t ctrl_string[] = "::0000000000";//used to control LED group
#endif
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
    // uint16_t k;
    USART1_RX_STA = 0;
    memset(USART1_RX_buf, 0, sizeof(USART1_RX_buf));
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
            // k = TIM3_GetCounter();
        }
        if ((USART1_RX_STA & 0x8000))//receive the data
        {
            tag = USART1_RX_STA & 0x7FFF;//get the length of data
            USART1_RX_STA = 0;//clear the state flag
            if ((('O' == USART1_RX_buf[tag-4]) &&
                ('K' == USART1_RX_buf[tag-3])) ||
                (('o' == USART1_RX_buf[tag-4]) &&
                ('k' == USART1_RX_buf[tag-3])))
            {
                tag = 0;//enter succeed
                break;
            }
        }
    }
    //clear the rx buffer
    USART1_RX_STA = 0;
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
                      remeber to clear the state buffer after used.
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
    char *stastring = connect2("AT+", sta);
    while (*sta)    //get the length of sta
    {
        *sta ++;
        stalen += 1;
    }
    while (retry--)
    {
        USART1_SendWord((uint8_t*) (stastring));
        USART1_SendWord("?\r\n");
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
                ('O' == USART1_RX_buf[temp-4]) &&
                ('K' == USART1_RX_buf[temp-3]) )
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
            USART1_STA_buf[i] = USART1_RX_buf[t];
            i++;
        }
    }
    //clear the rx buffer
    memset(USART1_RX_buf, 0, sizeof(USART1_RX_buf));
    if(0 == retry)tag = 1;//enter failed
    return tag;
}
#if DEBUG_STATUS//Currently unavailable
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
            if (tag == 6
            && ('a' == USART1_RX_buf[0]) && ('+' == USART1_RX_buf[1]) && ('O' == USART1_RX_buf[2]) && ('K' == USART1_RX_buf[3]))
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
Function Name       : BLE_status_it
Function Description: Use peripherals to express BLE status
Param_in            : 
Param_out           : 
Return Type         : 
Note                : 
Author              : Yan
Time                : 2020-11-30
*************************************************************/
void BLE_status_it(void)
{
/**
   Non-MESH mode
            NAME: Dx >>>Device x (x=1, 2, 3,...)
        LED MODE: Flashing LEDG in freq 1000ms
        KEY MODE: Enter MESH Mode through short press(<3s)
       LINK PROC: Flashing LEDR in freq 100ms(NU)
         LINK OK: The LEDR(G) flash alternately for 1.5s with beep play
        LINK ERR: The buzzer beeps for 2s and the LEDR is on
       MESH mode
            NAME: Nx >>>Node x (x=1, 2, 3,...)
        LED MODE: Flashing LEDR in freq 1000ms
        KEY MODE: Exit MESH Mode through long press(>3s)
    DISLINK PROC: Flashing LEDG in freq 100ms(NU)
      DISLINK OK: The LEDR(G) flash alternately for 1.5s with beep play
     DISLINK ERR: The buzzer beeps for 2s and the LEDG is on   
**/
    uint8_t i = 1;
    if (0 == AT_Send("+++a"))//enter AT mode
    {
        if (0 == AT_Get_State("MODE"))//get the mode
        {
            if ('e' == USART1_STA_buf[1])//MESH mode
            {
                memset(USART1_STA_buf, 0, sizeof(USART1_STA_buf));//clear the buffer
                BLE_STA_flag = 0;
            }
            else//Non-MESH mode
            {
                memset(USART1_STA_buf, 0, sizeof(USART1_STA_buf));//clear the buffer
                BLE_STA_flag = 1;
            }
            
        }
        while (AT_Send("AT+ENTM\r\n"))//exit AT mode
        {
            if((i++) >= 5)
            {                
                USART1_SendWord("Unable to exit AT mode...\r\n");
                break;
            }
        }
    }
    else
        USART1_SendWord("Unable to get the state...\r\n");
}
/*************************************************************
Function Name       : BLE_status_run
Function Description: show the BLE status in main
Param_in            : 
Param_out           : 
Return Type         : 
Note                : 
Author              : Yan
Time                : 2020-12-04
*************************************************************/
void BLE_status_run(void)
{
    // LEDR_L();
    // LEDG_L();
    if (1 == BLE_STA_flag)//NON-MESH
    {
        
        LEDG_R();
        delay_ms_1(500);
        LEDG_R();
        delay_ms_1(500);
    }
    else
    {
        
        LEDR_R();
        delay_ms_1(500);
        LEDR_R();
        delay_ms_1(500);
    }
    
}
/*************************************************************
Function Name       : user_app_run
Function Description: use key/phone to control led
Param_in            : 
Param_out           : 
Return Type         : 
Note                : 
Author              : Yan
Time                : 2020-11-25
--------------------------------------------------------------
log                 : 2020-12-7
                      This function changed to control LED group
                      2020-12-9
                      This function added the features of phone 
                      control LED group
*************************************************************/
void user_app_run(void)
{
#if (RELAY_DEV != DEVICE_ID)
    if((USART1_RX_STA & (uint16_t)(1<<15)) == 0)//No message
        return;    
    if(('0' != USART1_RX_buf[0]) && ('1' != USART1_RX_buf[0]))//non-key message
    {
        memset(USART1_RX_buf, 0, sizeof(USART1_RX_buf));
        USART1_RX_STA = 0;
        return;
    }
    if((USART1_RX_STA & (uint16_t)(1<<15)) == 32768)//Get key message
    {
#if (1 == DEVICE_ID)            
        if ('1' == USART1_RX_buf[0])
            LEDG_H();
        if ('0' == USART1_RX_buf[0])
            LEDG_L();
#endif
#if (2 == DEVICE_ID)            
        if ('1' == USART1_RX_buf[1])
            LEDG_H();
        if ('0' == USART1_RX_buf[1])
            LEDG_L();
#endif
#if (3 == DEVICE_ID)            
        if ('1' == USART1_RX_buf[2])
            LEDG_H();
        if ('0' == USART1_RX_buf[2])
            LEDG_L();
#endif
#if (4 == DEVICE_ID)            
        if ('1' == USART1_RX_buf[3])
            LEDG_H();
        if ('0' == USART1_RX_buf[3])
            LEDG_L();
#endif
#if (5 == DEVICE_ID)            
        if ('1' == USART1_RX_buf[4])
            LEDG_H();
        if ('0' == USART1_RX_buf[4])
            LEDG_L();
#endif
#if (6 == DEVICE_ID)            
        if ('1' == USART1_RX_buf[5])
            LEDG_H();
        if ('0' == USART1_RX_buf[5])
            LEDG_L();
#endif
#if (7 == DEVICE_ID)            
        if ('1' == USART1_RX_buf[6])
            LEDG_H();
        if ('0' == USART1_RX_buf[6])
            LEDG_L();
#endif
#if (8 == DEVICE_ID)            
        if ('1' == USART1_RX_buf[7])
            LEDG_H();
        if ('0' == USART1_RX_buf[7])
            LEDG_L();
#endif
#if (9 == DEVICE_ID)            
        if ('1' == USART1_RX_buf[8])
            LEDG_H();
        if ('0' == USART1_RX_buf[8])
            LEDG_L();
#endif
#if (10 == DEVICE_ID)            
        if ('1' == USART1_RX_buf[9])
            LEDG_H();
        if ('0' == USART1_RX_buf[9])
            LEDG_L();
#endif
        memset(USART1_RX_buf, 0, sizeof(USART1_RX_buf));
        USART1_RX_STA = 0;
        
    }
#endif
#if (RELAY_DEV == DEVICE_ID)
    if(0 == BLE_STA_flag)//process msg when enter mesh mode
    {
        delay_ms_1(100);
        //init
        memset(USART1_RX_buf, 0, sizeof(USART1_RX_buf));
        USART1_RX_STA = 0;
        //process
        uint8_t *temp_string;//intermediate variables
        temp_string = (uint8_t *)ctrl_string;
        USART1_SendWord(temp_string);
        delay_ms_1(500);
        MESH_cmd(DISABLE);
        BLE_status_it();
    }
    if((USART1_RX_STA & (uint16_t)(1<<15)) == 0)//No message
        return;    
    if(('0' != USART1_RX_buf[1]) && ('1' != USART1_RX_buf[1]))//non-phone message
    {
        memset(USART1_RX_buf, 0, sizeof(USART1_RX_buf));
        USART1_RX_STA = 0;
        return;
    }
    if((USART1_RX_STA & (uint16_t)(1<<15)) == 0x8000)//Get phone message
    {
        if (0 != (USART1_RX_STA & 0X0001))//invaild cmd
        {
            USART1_SendWord("Invaild message...");
            memset(USART1_RX_buf, 0, sizeof(USART1_RX_buf));
            USART1_RX_STA = 0;
            return;
        }
        uint8_t id;//get the id
        uint8_t sta;//get the status
        uint16_t i = 0;
        while (i < (USART1_RX_STA & 0x7fff))
        {
            id = USART1_RX_buf[i];
            sta = USART1_RX_buf[i+1];
            id -= 48;//ascii code offset compasation
            ctrl_string[id+1] = sta;
            i += 2;
        }
        memset(USART1_RX_buf, 0, sizeof(USART1_RX_buf));
        USART1_RX_STA = 0;
        AT_Send("+++a");
        AT_Send("AT+DISCONN\r\n");
        AT_Send("AT+ENTM\r\n");
        delay_ms_1(100);
        MESH_cmd(ENABLE);
        BLE_status_it();
    }
    
#endif
}
/*--------------------------- END --------------------------*/