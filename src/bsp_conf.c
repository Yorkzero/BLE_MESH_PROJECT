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
const uint8_t beep_period_buf[E_BEEP_PERIOD_END] = {1, 2, 5, 6, 12, 63, 124, 250};
uint8_t USART1_RX_buf[USART1_RX_MAX_LEN];//USART1 receive buffer
uint8_t USART1_STA_buf[USART1_STA_MAX_LEN];//USART1 state buffer
/*------------------ Variable Declarations -----------------*/
volatile uint8_t key_flag = 0;       //key state flag
volatile uint8_t exti4_sta_flag = 0; //used to know which IO(PC4/PB4) triggers the IT. sta: 0(PC4), 1(PB4)
volatile uint32_t beep_play_time = 0; //record the beep play time
/**
 * @brief define the receive state of usart1 
 * @param bit [15] 0: waiting for receive, 1: received a set of data
 * @param bit [14:0] record the length of data
 */
volatile uint16_t USART1_RX_STA = 0; 
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
    __disable_interrupt();
    //Initialization of all used pin, change the mode if needed.
    GPIO_Init(LEDG_PORT, LEDG_PIN, GPIO_Mode_Out_PP_Low_Slow);        //Green led init
    GPIO_Init(LEDR_PORT, LEDR_PIN, GPIO_Mode_Out_PP_Low_Slow);        //Red led init
    GPIO_Init(LOWV_PORT, LOWV_PIN, GPIO_Mode_In_FL_No_IT);            //Low pwr detection, opening interrupt when used.
    GPIO_Init(KEY_PORT, KEY_PIN, GPIO_Mode_In_PU_No_IT);              //key detection, opening interrupt when used.
    GPIO_Init(BLE_RST_PORT, BLE_RST_PIN, GPIO_Mode_Out_PP_High_Slow); //ble chip enable state
    GPIO_Init(BLE_LINK_PORT, BLE_LINK_PIN, GPIO_Mode_In_PU_No_IT);    //ble link state/active low
    GPIO_Init(BEEP_PORT, BEEP_PIN, GPIO_Mode_Out_PP_Low_Slow);        //beep init
    GPIO_Init(UART_RX_PORT, UART_RX_PIN, GPIO_Mode_In_PU_No_IT);      //UART receive init
    GPIO_Init(UART_TX_PORT, UART_TX_PIN, GPIO_Mode_Out_PP_High_Fast); //UART transmitt init
    //Initialization of EXIT
    // EXTI_SetPortSensitivity(KEY_EXTI_PORT, EXTI_Trigger_Falling);       //key trigger falling
    EXTI_SetPinSensitivity(KEY_EXTI_PIN, EXTI_Trigger_Falling); //key trigger falling

    //IT Priority
    ITC_SetSoftwarePriority(EXTI4_IRQn,ITC_PriorityLevel_1); //key 1
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
    CLK_HSICmd(ENABLE);                           //internal RC 16M enable
    CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSI); //choose HSI as system clk
    CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);         // div 1

    //peripharal clk state
    //comment when used
    CLK_PeripheralClockConfig(CLK_Peripheral_TIM1, DISABLE);
    //CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, DISABLE);
    //CLK_PeripheralClockConfig(CLK_Peripheral_TIM3, DISABLE);
    //CLK_PeripheralClockConfig(CLK_Peripheral_TIM4,DISABLE);
    CLK_PeripheralClockConfig(CLK_Peripheral_I2C1, DISABLE);
    CLK_PeripheralClockConfig(CLK_Peripheral_SPI1, DISABLE);
    //CLK_PeripheralClockConfig(CLK_Peripheral_USART1,DISABLE);
    CLK_PeripheralClockConfig(CLK_Peripheral_BEEP, DISABLE);
    CLK_PeripheralClockConfig(CLK_Peripheral_ADC1, DISABLE);
    CLK_PeripheralClockConfig(CLK_Peripheral_RTC, DISABLE);
}
/*************************************************************
Function Name       : bsp_key_it
Function Description: key state detection
Param_in            : 
Param_out           : 
Return Type         : 
Note                : everytime clicking the button will reverse the key_flag
Author              : Yan
Time                : 2020-11-20
*************************************************************/
void bsp_key_it(void)
{
    
    beep_play(E_BEEP_MODE_ERR);
}
/*************************************************************
Function Name       : bsp_key_detec
Function Description: used to detec press time
Param_in            : 
Param_out           : 
Return Type         : 
Note                : short press: <3s
                      long press: >3s
Author              : Yan
Time                : 2020-11-30
*************************************************************/
void bsp_key_detec(void)
{
    LEDG_L();
    LEDR_L();
    bsp_tim2_init(12500);//100ms upload
    delay_ms_1(20);//avoid shaking
    if(!KEY_READ())
    {
        TIM2_Cmd(ENABLE);
        while(!KEY_READ());
        TIM2_Cmd(DISABLE);
        CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, DISABLE); //enable the clk
    }
    if (30 > key_flag)//short press
    {
#if(RELAY_DEV == DEVICE_ID)
        uint8_t *temp_string;//intermediate variables
        temp_string = (uint8_t *)ctrl_string;
        USART1_SendWord(temp_string);
#endif
    }
    if (30 <= key_flag)//long press
    {
        if (1 == BLE_STA_flag)
            MESH_cmd(ENABLE);
        if (0 == BLE_STA_flag)
            MESH_cmd(DISABLE);
        BLE_status_it();
    }
    key_flag = 0;
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
    // if (BLE_STA_READ())//high level
    // {
    //     LEDG_L();
    //     //LEDR_H();
    // }
    // if (!BLE_STA_READ())//low level
    // {
    //     LEDG_H();
    //     //LEDR_L();
    // }
    LEDG_R();
}
/*************************************************************
Function Name       : EXTI4_Sta_detec
Function Description: Used to avoid interruption detection confusion
Param_in            : 
Param_out           : 
Return Type         : 
Note                : PC4(LINK_STA)/PB4(KEY_STA) mixed
Author              : Yan
Time                : 2020-11-26
*************************************************************/
void EXTI4_Sta_detec(void)
{
    delay_ms_1(20); //avoid shaking
    if (!KEY_READ())
    {
        exti4_sta_flag = 1; //key
    }
    else
        exti4_sta_flag = 0; //ble link
}
/*************************************************************
Function Name       : bsp_tim2_init
Function Description: initialization of TIM2
Param_in            : u16 period
Param_out           : 
Return Type         : 
Note                : used when key state detecting
Author              : Yan
Time                : 2020-11-30
*************************************************************/
void bsp_tim2_init(uint16_t period)
{
    TIM2_DeInit();
    CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, ENABLE); //enable the clk
#if (SYS_CLK_FREQ_16M == SYS_CLK_FREQ)
    TIM2_TimeBaseInit(TIM2_Prescaler_128, TIM2_CounterMode_Up, period);
#elif (SYS_CLK_FREQ_8M == SYS_CLK_FREQ)
    TIM2_TimeBaseInit(TIM2_Prescaler_64, TIM2_CounterMode_Up, period);
#elif (SYS_CLK_FREQ_4M == SYS_CLK_FREQ)
    TIM2_TimeBaseInit(TIM2_Prescaler_32, TIM2_CounterMode_Up, period);
#elif (SYS_CLK_FREQ_2M == SYS_CLK_FREQ)
    TIM2_TimeBaseInit(TIM2_Prescaler_16, TIM2_CounterMode_Up, period);
#elif (SYS_CLK_FREQ_1M == SYS_CLK_FREQ)
    TIM2_TimeBaseInit(TIM2_Prescaler_8, TIM2_CounterMode_Up, period);
#else
#error "Invalid system clk value..."
#endif
    TIM2_ARRPreloadConfig(ENABLE); //enable auto reload
    TIM2_ClearFlag(TIM2_FLAG_Update);
    TIM2_ITConfig(TIM2_IT_Update, ENABLE);
    ITC_SetSoftwarePriority(TIM2_UPD_OVF_TRG_BRK_IRQn, ITC_PriorityLevel_3);//priority 3(HIGHEST)
    TIM2_Cmd(DISABLE);
}
/*************************************************************
Function Name       : TIM2_IRQHandler
Function Description: TIM2 IT function
Param_in            : 
Param_out           : 
Return Type         : 
Note                : 
Author              : Yan
Time                : 2020-11-30
*************************************************************/
void TIM2_IRQHandler(void)
{
    if(TIM2_GetFlagStatus(TIM2_FLAG_Update) != RESET)
    {
        if (255 == key_flag)
            TIM2_Cmd(DISABLE);
        else
            key_flag++;
        TIM2_SetCounter(0);//recounter    
    }
    TIM2_ClearITPendingBit(TIM2_IT_Update);
}
/*************************************************************
Function Name       : bsp_tim3_init
Function Description: initialization of TIM3
Param_in            : u16 period
Param_out           : 
Return Type         : 
Note                : used when usart1 receives data
Author              : Yan
Time                : 2020-11-27
*************************************************************/
void bsp_tim3_init(uint16_t period)
{
    TIM3_DeInit();
    CLK_PeripheralClockConfig(CLK_Peripheral_TIM3, ENABLE); //enable the clk
#if (SYS_CLK_FREQ_16M == SYS_CLK_FREQ)
    TIM3_TimeBaseInit(TIM3_Prescaler_128, TIM3_CounterMode_Up, period);
#elif (SYS_CLK_FREQ_8M == SYS_CLK_FREQ)
    TIM3_TimeBaseInit(TIM3_Prescaler_64, TIM3_CounterMode_Up, period);
#elif (SYS_CLK_FREQ_4M == SYS_CLK_FREQ)
    TIM3_TimeBaseInit(TIM3_Prescaler_32, TIM3_CounterMode_Up, period);
#elif (SYS_CLK_FREQ_2M == SYS_CLK_FREQ)
    TIM3_TimeBaseInit(TIM3_Prescaler_16, TIM3_CounterMode_Up, period);
#elif (SYS_CLK_FREQ_1M == SYS_CLK_FREQ)
    TIM3_TimeBaseInit(TIM3_Prescaler_8, TIM3_CounterMode_Up, period);
#else
#error "Invalid system clk value..."
#endif
    TIM3_ARRPreloadConfig(ENABLE); //enable auto reload
    TIM3_ClearFlag(TIM3_FLAG_Update);
    TIM3_ITConfig(TIM3_IT_Update, ENABLE);
    ITC_SetSoftwarePriority(TIM3_UPD_OVF_TRG_BRK_IRQn, ITC_PriorityLevel_3);//priority 3(HIGHEST)
    TIM3_Cmd(DISABLE);
}
/*************************************************************
Function Name       : TIM3_IRQHandler
Function Description: TIM3 IT function
Param_in            : 
Param_out           : 
Return Type         : 
Note                : 
Author              : Yan
Time                : 2020-11-27
*************************************************************/
void TIM3_IRQHandler(void)
{
    if(TIM3_GetFlagStatus(TIM3_FLAG_Update) != RESET)
    {
        USART1_RX_STA |= (uint16_t) 1<<15;//receive finished
        TIM3_ClearITPendingBit(TIM3_IT_Update);
        TIM3_Cmd(DISABLE);
        return;
    }
}
/*************************************************************
Function Name       : bsp_tim4_init
Function Description: initialization of TIM4
Param_in            : u8 period
Param_out           : 
Return Type         : 
Note                : used in beep play
Author              : Yan
Time                : 2020-11-26
*************************************************************/
void bsp_tim4_init(uint8_t period)
{
    TIM4_DeInit();
    CLK_PeripheralClockConfig(CLK_Peripheral_TIM4, ENABLE); //enable the clk
#if (SYS_CLK_FREQ_16M == SYS_CLK_FREQ)
    TIM4_TimeBaseInit(TIM4_Prescaler_128, period);
#elif (SYS_CLK_FREQ_8M == SYS_CLK_FREQ)
    TIM4_TimeBaseInit(TIM4_Prescaler_64, period);
#elif (SYS_CLK_FREQ_4M == SYS_CLK_FREQ)
    TIM4_TimeBaseInit(TIM4_Prescaler_32, period);
#elif (SYS_CLK_FREQ_2M == SYS_CLK_FREQ)
    TIM4_TimeBaseInit(TIM4_Prescaler_16, period);
#elif (SYS_CLK_FREQ_1M == SYS_CLK_FREQ)
    TIM4_TimeBaseInit(TIM4_Prescaler_8, period);
#else
#error "Invalid system clk value..."
#endif
    TIM4_ARRPreloadConfig(ENABLE); //enable auto reload
    TIM4_ClearFlag(TIM4_FLAG_Update);
    TIM4_ITConfig(TIM4_IT_Update, ENABLE);
    ITC_SetSoftwarePriority(TIM4_UPD_OVF_TRG_IRQn, ITC_PriorityLevel_3);//priority 3
    TIM4_Cmd(ENABLE);
}
/*************************************************************
Function Name       : bsp_beep_it
Function Description: beep ctrl function used in TIM4 IRQHandler
Param_in            : 
Param_out           : 
Return Type         : 
Note                : use this function to play different kinds of music
Author              : Yan
Time                : 2020-11-26
*************************************************************/
void bsp_beep_it(void)
{
    if (beep_play_time > 0)
    {
        BEEP_R();
        beep_play_time -= 1;
    }
    else
    {
        BEEP_L();
        TIM4_Cmd(DISABLE);
        TIM4_DeInit();
        CLK_PeripheralClockConfig(CLK_Peripheral_TIM4,DISABLE);
    }
    TIM4_ClearITPendingBit(TIM4_IT_Update);
}
/*************************************************************
Function Name       : bsp_beep_freq
Function Description: set the frequency of beep
Param_in            : u8 freq
Param_out           : 
Return Type         : 
Note                : 
Author              : Yan
Time                : 2020-11-27
*************************************************************/
void bsp_beep_freq(uint8_t freq)
{
    if (E_BEEP_PERIOD_10US == freq)
        beep_play_time = 100;
    else if (E_BEEP_PERIOD_20US == freq)
        beep_play_time = 50;
    else if (E_BEEP_PERIOD_40US == freq)
        beep_play_time = 25;
    else if (E_BEEP_PERIOD_50US == freq)
        beep_play_time = 20;
    else if (E_BEEP_PERIOD_100US == freq)
        beep_play_time = 10;
    else if (E_BEEP_PERIOD_500US == freq)
        beep_play_time = 2;
    else if (E_BEEP_PERIOD_1MS == freq)
        beep_play_time = 1;    
    else if (E_BEEP_PERIOD_2MS == freq)
        // beep_play_time = 0.5;//error value, avoid to use it
        return;
    else
        return;//invalid value
}
/*************************************************************
Function Name       : bsp_beep_play_ms
Function Description: used to control the play time
Param_in            : u8 freq, u16 time
Param_out           : 
Return Type         : 
Note                : range from 2 to 65535ms
Author              : Yan
Time                : 2020-11-26
*************************************************************/
void bsp_beep_play_ms(uint8_t freq, uint16_t time)
{
    bsp_beep_freq(freq);
    beep_play_time *= time;
    bsp_tim4_init(beep_period_buf[freq]);
}
/*************************************************************
Function Name       : beep_play
Function Description: play different kinds of music
Param_in            : style
Param_out           : 
Return Type         : 
Note                : 
Author              : Yan
Time                : 2020-11-26
*************************************************************/
void beep_play(uint8_t style)
{
    LEDG_L();
    LEDR_L();
    switch (style)
    {
    case E_BEEP_MODE_INIT:
        bsp_beep_play_ms(E_BEEP_PERIOD_500US, 300);
        delay_ms_1(300);
        bsp_beep_play_ms(E_BEEP_PERIOD_500US, 300);
        goto EXIT;
        break;
    case E_BEEP_MODE_ERR:
        bsp_beep_play_ms(E_BEEP_PERIOD_100US, 2000);
        LEDR_H();
        delay_ms_1(2000);
        LEDR_L();
        break;
    case E_BEEP_MODE_SUCCESS:
        LEDR_L();
        LEDG_H();
        delay_ms_1(100);
        bsp_beep_play_ms(E_BEEP_PERIOD_100US, 250);
        LEDR_H();
        LEDG_L();
        delay_ms_1(100);
        bsp_beep_play_ms(E_BEEP_PERIOD_100US, 250);
        LEDR_L();
        LEDG_H();
        delay_ms_1(100);
        bsp_beep_play_ms(E_BEEP_PERIOD_100US, 250);
        LEDR_H();
        LEDG_L();
        delay_ms_1(100);
        bsp_beep_play_ms(E_BEEP_PERIOD_100US, 250);
        LEDR_L();
        LEDG_L();
        goto EXIT;
        break;
    case E_BEEP_MODE_RX:
        
        break;
    case E_BEEP_MODE_WAIT:

        break;
    default:
        break;
    }
EXIT:
    return;
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
    
    USART_DeInit(USART1);
    CLK_PeripheralClockConfig(CLK_Peripheral_USART1, ENABLE);
    //remappin
    //SYSCFG_REMAPPinConfig(REMAP_Pin_USART1TxRxPortC, ENABLE);
    /**
    * @param USART1 ,
    * @param BR: 57600, 
    * @param WL: 8bit, 
    * @param SB: 1 bit, 
    * @param PR: no parity, 
    * @param MODE: RX/TX mode 
    */
    USART_Init(USART1, (uint32_t)57600, USART_WordLength_8b, USART_StopBits_1, USART_Parity_No, (USART_Mode_Tx | USART_Mode_Rx));

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    // USART_ITConfig(USART1, USART_IT_OR, ENABLE);
    ITC_SetSoftwarePriority(USART1_RX_IRQn, ITC_PriorityLevel_2);//priority 2
    USART_Cmd(USART1, ENABLE);
    USART1_RX_STA = 0;
    bsp_tim3_init(1250);//Enter interrupt after 10ms
}
/*************************************************************
Function Name       : USART1_IRQHandler
Function Description: usart1 IT function
Param_in            : 
Param_out           : 
Return Type         : 
Note                : The interval between each data can not exceed 10ms
Author              : Yan
Time                : 2020-11-27
*************************************************************/
void USART1_IRQHandler(void)
{
    uint8_t receive_data;
    if(USART_GetITStatus(USART1,USART_IT_RXNE) != RESET)        //Check whether the specified UART1 interrupt occurs. 
    {
        receive_data = USART_ReceiveData8(USART1);
        if((USART1_RX_STA & (uint16_t)(1<<15)) == 0)                      
        {
            if(USART1_RX_STA < USART1_RX_MAX_LEN)               //Determine whether the byte overflows
            {
                TIM3_SetCounter(0);//recount
                if(0 == USART1_RX_STA)
                    TIM3_Cmd(ENABLE);
                USART1_RX_buf[USART1_RX_STA++] = receive_data;
                // if(RELAY_DEV == DEVICE_ID)
                //     USART1_RELAY_buf[USART1_RX_STA++] = receive_data;
                
            }
            else
                USART1_RX_STA |= (uint16_t) 1<<15;//enforce finish receive 
        }

        USART_ClearITPendingBit(USART1,USART_IT_RXNE);            //Clear UART1 pending flag
    }
}
/*************************************************************
Function Name       : USART1_SendWord
Function Description: Transmits 8 bit *data through the USART1 peripharal
Param_in            : *Data
Param_out           : 
Return Type         : 
Note                : 
Author              : Yan
Time                : 2020-11-23
*************************************************************/
void USART1_SendWord(uint8_t *Data)
{
    while (*Data)
    {
        USART_SendData8(USART1, *Data++);
        while (!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
    }
}
/*************************************************************
Function Name       : MESH_cmd
Function Description: enable or disable mesh
Param_in            : FunctionalState NewState                      
Param_out           : 
Return Type         : 
Note                : This parameter can be ENABLE or DISABLE
Author              : Yan
Time                : 2020-11-30
*************************************************************/
void MESH_cmd(FunctionalState NewState)
{
    assert_param(IS_FUNCTIONAL_STATE(NewState));
    uint8_t i = 0;
    if (ENABLE == NewState)
    {
        if (0 == AT_Send("+++a"))//Enter AT Mode
        {
            // delay_ms_1(100);
#if (1 == DEVICE_ID)            
            AT_Send("AT+NAME=N1\r\n");//NODE 1 in MESH
            // delay_ms_1(100);
#endif
#if (2 == DEVICE_ID)            
            AT_Send("AT+NAME=N2\r\n");//NODE 2 in MESH
            // delay_ms_1(100);
#endif
#if (3 == DEVICE_ID)            
            AT_Send("AT+NAME=N3\r\n");//NODE 3 in MESH
            // delay_ms_1(100);
#endif
#if (4 == DEVICE_ID)            
            AT_Send("AT+NAME=N4\r\n");//NODE 4 in MESH
            // delay_ms_1(100);
#endif
#if (5 == DEVICE_ID)            
            AT_Send("AT+NAME=N5\r\n");//NODE 5 in MESH
            // delay_ms_1(100);
#endif
#if (6 == DEVICE_ID)            
            AT_Send("AT+NAME=N6\r\n");//NODE 6 in MESH
            // delay_ms_1(100);
#endif
#if (7 == DEVICE_ID)            
            AT_Send("AT+NAME=N7\r\n");//NODE 7 in MESH
            // delay_ms_1(100);
#endif
#if (8 == DEVICE_ID)            
            AT_Send("AT+NAME=N8\r\n");//NODE 8 in MESH
            // delay_ms_1(100);
#endif
#if (9 == DEVICE_ID)            
            AT_Send("AT+NAME=N9\r\n");//NODE 9 in MESH
            // delay_ms_1(100);
#endif
#if (10 == DEVICE_ID)            
            AT_Send("AT+NAME=N10\r\n");//NODE 10 in MESH
            // delay_ms_1(100);
#endif
            AT_Send("AT+PASS=111111\r\n");//Set matching password: 111111
            // delay_ms_1(100);
            while(0 != AT_Send("AT+MODE=F\r\n"))//Enter AT Mode
            {
                if ((i++) >= 5)
                {
                    i = 0;
                    USART1_SendWord("Enter MESH error...\r\n");
                    beep_play(E_BEEP_MODE_ERR);
                    memset(USART1_RX_buf, 0, sizeof(USART1_RX_buf));
                    return;
                }    
            }
            USART1_SendWord("Enter MESH successfully...\r\n");
            beep_play(E_BEEP_MODE_SUCCESS);
            memset(USART1_RX_buf, 0, sizeof(USART1_RX_buf));
        }
        else
        {
            USART1_SendWord("MESH ERROR...\r\n");
        }    
    }
    if (DISABLE == NewState)    
    {   
        if (0 == AT_Send("+++a"))//Enter AT Mode
        {
#if (1 == DEVICE_ID)            
            AT_Send("AT+NAME=D1\r\n");//DEVICE 1 out MESH
#endif
#if (2 == DEVICE_ID)            
            AT_Send("AT+NAME=D2\r\n");//DEVICE 2 out MESH
#endif
#if (3 == DEVICE_ID)            
            AT_Send("AT+NAME=D3\r\n");//DEVICE 3 out MESH
#endif
#if (4 == DEVICE_ID)            
            AT_Send("AT+NAME=D4\r\n");//DEVICE 4 out MESH
#endif
#if (5 == DEVICE_ID)            
            AT_Send("AT+NAME=D5\r\n");//DEVICE 5 out MESH
#endif
#if (6 == DEVICE_ID)            
            AT_Send("AT+NAME=D6\r\n");//DEVICE 6 out MESH
#endif
#if (7 == DEVICE_ID)            
            AT_Send("AT+NAME=D7\r\n");//DEVICE 7 out MESH
#endif
#if (8 == DEVICE_ID)            
            AT_Send("AT+NAME=D8\r\n");//DEVICE 8 out MESH
#endif
#if (9 == DEVICE_ID)            
            AT_Send("AT+NAME=D9\r\n");//DEVICE 9 out MESH
#endif
#if (10 == DEVICE_ID)            
            AT_Send("AT+NAME=D10\r\n");//DEVICE 10 out MESH
#endif
            AT_Send("AT+MODE=S\r\n");//mode:Slave
            while(AT_Send("AT+ENTM\r\n"))//Exit AT Mode
            {
                if ((i++) >= 5)
                {
                    i = 0;
                    USART1_SendWord("Exit MESH error...\r\n");
                    beep_play(E_BEEP_MODE_ERR);
                    return;
                }
            }
            USART1_SendWord("Exit MESH successfully...\r\n");
            beep_play(E_BEEP_MODE_SUCCESS);
        }
        else
            USART1_SendWord("NON-MESH ERROR...\r\n");
    }

}
#if (RELAY_DEV == DEVICE_ID)
/*************************************************************
Function Name       : bsp_phone_recevier
Function Description: used in the relay device
Param_in            : 
Param_out           : 
Return Type         : 
Note                : receive and process the messsage from phone
Author              : Yan
Time                : 2020-12-08
*************************************************************/
void bsp_phone_recevier(void)
{
    /**
     * @brief: Receive two bits of data at a time
     * @param bit [1]: set the status of the device(0, 1)
     * @param bit [0]: selected device ID(2, 3, 4, 5...)
     * @note eg. (30) means N3 with low level, (41) means N4 with high level
     */
    MESH_cmd(DISABLE);//make sure D1 works on the non-mesh mode

    
}
#endif
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

    asm("SIM"); //关总中断

    //发送起始位
    GPIOB->ODR &= ~GPIO_Pin_0;

#if (SYS_CLK_FREQ == SYS_CLK_FREQ_16M)
    asm(
        //保存本程序使用到的寄存器
        "PUSHW Y\n" //2T
        "PUSHW X\n" //2T

        "LDW Y,#9\n"       //2T用来控制循环
        "LDW X,#41\n"      //2T，赋值，实测得到
        "Delay0: DECW X\n" //减1，1个指令周期
        "JRNE Delay0\n"    //判断跳转，跳转要2T，不跳要1T
        "NOP\n"

        //到设置新的电平，需要6T
        "SEND_DATA:DECW Y\n" //1T,判断循环是否结束
        "JREQ STOP\n"        //结束则跳转，2T，不跳转则1T
        "NOP\n"              //1T，补偿未跳转的1T

        "SRL A\n"               //1T,逻辑右移，同时bit0在C标志位中
        "JRC SET1\n"            //等于1则跳转，跳转2T，不跳转1T
        "NOP\n"                 //1T，补偿未跳转的1T
        "BRES 0x5005,#0\n"      //1T，输出低
        "JP Delay1\n"           //1T，无条件跳转
        "SET1:BSET 0x5005,#0\n" //1T，输出高
        "NOP\n"

        "Delay1:NOP\n"
        "LDW X,#41\n"    //2T，实测得到
        "LOOP1:DECW X\n" //1T
        "JRNE LOOP1\n"   //跳转则2T，不跳转则1T

        "JP SEND_DATA\n" //1T，无条件跳转

        //发送2位时长的停止位
        "STOP:TNZW X\n"    //2T,只是为了延时2T
        "TNZW X\n"         //2T,只是为了延时2T
        "BSET 0x5005,#0\n" //1T，输出高
        "LDW X,#92\n"      //2T，延时139x2-2=276T
        "LOOP2:DECW X\n"   //1T
        "JRNE LOOP2\n"     //跳转则2T，不跳转则1T
        "NOP\n"

        //恢复本程序使用到的寄存器
        "POPW X\n"
        "POPW Y");
#elif (SYS_CLK_FREQ == SYS_CLK_FREQ_4M)
    //每个bit延时34T
    asm(
        //保存本程序使用到的寄存器
        "PUSHW Y\n"
        "PUSHW X\n"

        "LDW Y,#9\n" //2T用来控制循环
        "LDW X,#6\n" //2T，赋值
        //本段延时19T
        "Delay0: DECW X\n" //减1，1个指令周期
        "JRNE Delay0\n"    //判断跳转，跳转要2T，不跳要1T
        "NOP\n"
        "NOP\n"

        //到设置新的电平，需要6T
        "SEND_DATA:DECW Y\n" //1T,判断循环是否结束
        "JREQ STOP\n"        //结束则跳转，2T，不跳转则1T
        "NOP\n"              //1T，补偿未跳转的1T

        "SRL A\n"    //1T,逻辑右移，同时bit0在C标志位中
        "JRC SET1\n" //等于1则跳转，跳转2T，确认不跳转1T
        //"NOP\n"//1T，补偿未跳转的1T，实测调整
        "BRES 0x5005,#0\n"      //1T，输出低
        "JP Delay1\n"           //1T，无条件跳转
        "SET1:BSET 0x5005,#0\n" //1T，输出高
        "NOP\n"
        "NOP\n" //实测调整
        "NOP\n"

        //到下个bit变化前，需要再延时26T
        "Delay1:LDW X,#7\n" //2T，
        //本段延时21T
        "LOOP1:DECW X\n" //1T
        "JRNE LOOP1\n"   //跳转则2T，不跳转则1T
        "JP SEND_DATA\n" //1T，无条件跳转

        //发送2位时长的停止位
        "STOP:TNZW X\n"    //2T,只是为了延时2T
        "NOP\n"            //1T,只是为了延时1T
        "BSET 0x5005,#0\n" //1T，输出高

        //需要延时68T
        "LDW X,#22\n" //2T
        //本段总延时66T
        "LOOP2:DECW X\n" //1T
        "JRNE LOOP2\n"   //跳转则2T，不跳转则1T
        "NOP\n"

        //恢复本程序使用到的寄存器
        "POPW X\n"
        "POPW Y");
#endif
    asm("RIM"); //使能全局中断
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
    asm(
        //保存本程序使用到的寄存器
        "PUSHW Y\n" //2T
        "PUSHW X\n" //2T

        "LDW Y,#9\n"       //2T用来控制循环
        "LDW X,#41\n"      //2T，赋值，实测得到
        "Delay0: DECW X\n" //减1，1个指令周期
        "JRNE Delay0\n"    //判断跳转，跳转要2T，不跳要1T
        "NOP\n"

        //到设置新的电平，需要6T
        "SEND_DATA:DECW Y\n" //1T,判断循环是否结束
        "JREQ STOP\n"        //结束则跳转，2T，不跳转则1T
        "NOP\n"              //1T，补偿未跳转的1T

        "SRL A\n"               //1T,逻辑右移，同时bit0在C标志位中
        "JRC SET1\n"            //等于1则跳转，跳转2T，不跳转1T
        "NOP\n"                 //1T，补偿未跳转的1T
        "BRES 0x5005,#0\n"      //1T，输出低
        "JP Delay1\n"           //1T，无条件跳转
        "SET1:BSET 0x5005,#0\n" //1T，输出高
        "NOP\n"

        "Delay1:NOP\n"
        "LDW X,#41\n"    //2T，实测得到
        "LOOP1:DECW X\n" //1T
        "JRNE LOOP1\n"   //跳转则2T，不跳转则1T

        "JP SEND_DATA\n" //1T，无条件跳转

        //发送2位时长的停止位
        "STOP:TNZW X\n"    //2T,只是为了延时2T
        "TNZW X\n"         //2T,只是为了延时2T
        "BSET 0x5005,#0\n" //1T，输出高
        "LDW X,#92\n"      //2T，延时139x2-2=276T
        "LOOP2:DECW X\n"   //1T
        "JRNE LOOP2\n"     //跳转则2T，不跳转则1T
        "NOP\n"

        //恢复本程序使用到的寄存器
        "POPW X\n"
        "POPW Y");
#elif (SYS_CLK_FREQ == SYS_CLK_FREQ_4M)
    //每个bit延时34T
    asm(
        //保存本程序使用到的寄存器
        "PUSHW Y\n"
        "PUSHW X\n"

        "LDW Y,#9\n" //2T用来控制循环
        "LDW X,#6\n" //2T，赋值
        //本段延时19T
        "Delay0: DECW X\n" //减1，1个指令周期
        "JRNE Delay0\n"    //判断跳转，跳转要2T，不跳要1T
        "NOP\n"
        "NOP\n"

        //到设置新的电平，需要6T
        "SEND_DATA:DECW Y\n" //1T,判断循环是否结束
        "JREQ STOP\n"        //结束则跳转，2T，不跳转则1T
        "NOP\n"              //1T，补偿未跳转的1T

        "SRL A\n"    //1T,逻辑右移，同时bit0在C标志位中
        "JRC SET1\n" //等于1则跳转，跳转2T，确认不跳转1T
        //"NOP\n"//1T，补偿未跳转的1T，实测调整
        "BRES 0x5005,#0\n"      //1T，输出低
        "JP Delay1\n"           //1T，无条件跳转
        "SET1:BSET 0x5005,#0\n" //1T，输出高
        "NOP\n"
        "NOP\n" //实测调整
        "NOP\n"

        //到下个bit变化前，需要再延时26T
        "Delay1:LDW X,#7\n" //2T，
        //本段延时21T
        "LOOP1:DECW X\n" //1T
        "JRNE LOOP1\n"   //跳转则2T，不跳转则1T
        "JP SEND_DATA\n" //1T，无条件跳转

        //发送2位时长的停止位
        "STOP:TNZW X\n"    //2T,只是为了延时2T
        "NOP\n"            //1T,只是为了延时1T
        "BSET 0x5005,#0\n" //1T，输出高

        //需要延时68T
        "LDW X,#22\n" //2T
        //本段总延时66T
        "LOOP2:DECW X\n" //1T
        "JRNE LOOP2\n"   //跳转则2T，不跳转则1T
        "NOP\n"

        //恢复本程序使用到的寄存器
        "POPW X\n"
        "POPW Y");
#endif
    //    asm("RIM");	//使能全局中断
}

void sim_printf_string(uint8_t *str)
{
    if (0 == str)
        return;

    while (*str)
    {
        sim_uart_printf(*str++);
    }
}

const uint8_t hex_tab[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
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