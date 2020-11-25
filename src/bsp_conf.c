/*************************************************************
Copyright (c) 2020 Shawn Yan, All rights reserved
-------------------------------------------------------------
File Name: bsp_conf.c

Desc     : ���ڴ�Ÿ���ײ������ļ�

Author   : Shawn Yan

Date     : 2020-11-20

*************************************************************/

/*------------------------- Includes -----------------------*/
#include "bsp_conf.h"

/*----------- Global Definitions and Declarations ----------*/


/*-------------------- Type Declarations -------------------*/


/*------------------ Variable Declarations -----------------*/
volatile uint8_t key_flag = 0;   //key state flag

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
    // ITC_SetSoftwarePriority(EXTIB_IRQn,ITC_PriorityLevel_3); //key first
    // ITC_SetSoftwarePriority(EXTI4_IRQn,ITC_PriorityLevel_2); //led second
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
    delay_ms_1(20);//avoid shaking
    if (!KEY_READ())
    {
        if(0 == key_flag)
            key_flag = 1;
        else
            key_flag = 0;
    }
    
    
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
    USART_Init(USART1, (uint32_t)57600, USART_WordLength_8b, USART_StopBits_1, USART_Parity_No, (USART_Mode_TypeDef)(USART_Mode_Tx | USART_Mode_Rx));

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_ITConfig(USART1, USART_IT_OR, ENABLE);
    USART_Cmd(USART1, ENABLE);
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
        while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
    }
    
}

/*************************************************************
Function Name       : sim_uart_printf
Function Description: ģ�⴮�ڴ�ӡ
Param_in            : data
Param_out           : 
Return Type         : 
Note                : PB0�˿ڴ�ӡ
Author              : Yan
Time                : 2020-11-20
*************************************************************/
#if (SIM_UART_PRINTF_EN)
void sim_uart_printf(uint8_t data)
{
    //PB_ODR |= bit0;
    //PB_DDR |= bit0;
    //PB_CR1 |= bit0;

    asm("SIM");  //�����ж�

    //������ʼλ
    GPIOB->ODR &= ~GPIO_Pin_0;

#if (SYS_CLK_FREQ == SYS_CLK_FREQ_16M)
asm
(
    //���汾����ʹ�õ��ļĴ���
    "PUSHW Y\n"  //2T
    "PUSHW X\n"  //2T
    
    "LDW Y,#9\n"  //2T��������ѭ��
    "LDW X,#41\n" //2T����ֵ��ʵ��õ�
"Delay0: DECW X\n" //��1��1��ָ������
    "JRNE Delay0\n" //�ж���ת����תҪ2T������Ҫ1T
    "NOP\n"

    //�������µĵ�ƽ����Ҫ6T
"SEND_DATA:DECW Y\n"  //1T,�ж�ѭ���Ƿ����
    "JREQ STOP\n"//��������ת��2T������ת��1T
    "NOP\n"//1T������δ��ת��1T
    
    "SRL A\n" //1T,�߼����ƣ�ͬʱbit0��C��־λ��
    "JRC SET1\n"  //����1����ת����ת2T������ת1T
    "NOP\n"//1T������δ��ת��1T
    "BRES 0x5005,#0\n" //1T�������
    "JP Delay1\n"  //1T����������ת
"SET1:BSET 0x5005,#0\n" //1T�������
    "NOP\n"

"Delay1:NOP\n"
    "LDW X,#41\n" //2T��ʵ��õ�
"LOOP1:DECW X\n"  //1T
    "JRNE LOOP1\n"//��ת��2T������ת��1T
    
    "JP SEND_DATA\n"  //1T����������ת


    //����2λʱ����ֹͣλ
"STOP:TNZW X\n" //2T,ֻ��Ϊ����ʱ2T
    "TNZW X\n" //2T,ֻ��Ϊ����ʱ2T
    "BSET 0x5005,#0\n" //1T�������
    "LDW X,#92\n" //2T����ʱ139x2-2=276T
"LOOP2:DECW X\n"  //1T
    "JRNE LOOP2\n"//��ת��2T������ת��1T
    "NOP\n"

    //�ָ�������ʹ�õ��ļĴ���
    "POPW X\n"
    "POPW Y"
)       ;
#elif (SYS_CLK_FREQ == SYS_CLK_FREQ_4M)
    //ÿ��bit��ʱ34T
asm
(
    //���汾����ʹ�õ��ļĴ���
    "PUSHW Y\n"
    "PUSHW X\n"
    
    "LDW Y,#9\n"  //2T��������ѭ��
    "LDW X,#6\n" //2T����ֵ
    //������ʱ19T
"Delay0: DECW X\n" //��1��1��ָ������
    "JRNE Delay0\n" //�ж���ת����תҪ2T������Ҫ1T
    "NOP\n"
    "NOP\n"

    //�������µĵ�ƽ����Ҫ6T
"SEND_DATA:DECW Y\n"  //1T,�ж�ѭ���Ƿ����
    "JREQ STOP\n"//��������ת��2T������ת��1T
    "NOP\n"//1T������δ��ת��1T

    "SRL A\n" //1T,�߼����ƣ�ͬʱbit0��C��־λ��
    "JRC SET1\n"  //����1����ת����ת2T��ȷ�ϲ���ת1T
    //"NOP\n"//1T������δ��ת��1T��ʵ�����
    "BRES 0x5005,#0\n" //1T�������
    "JP Delay1\n"  //1T����������ת
"SET1:BSET 0x5005,#0\n" //1T�������
    "NOP\n"
    "NOP\n" //ʵ�����
    "NOP\n"

    //���¸�bit�仯ǰ����Ҫ����ʱ26T
"Delay1:LDW X,#7\n" //2T��
    //������ʱ21T
"LOOP1:DECW X\n"  //1T
    "JRNE LOOP1\n"//��ת��2T������ת��1T
    "JP SEND_DATA\n"  //1T����������ת


    //����2λʱ����ֹͣλ
"STOP:TNZW X\n" //2T,ֻ��Ϊ����ʱ2T
    "NOP\n" //1T,ֻ��Ϊ����ʱ1T
    "BSET 0x5005,#0\n" //1T�������

    //��Ҫ��ʱ68T
    "LDW X,#22\n" //2T
    //��������ʱ66T
"LOOP2:DECW X\n"  //1T
    "JRNE LOOP2\n"//��ת��2T������ת��1T
    "NOP\n"

    //�ָ�������ʹ�õ��ļĴ���
    "POPW X\n"
    "POPW Y"    
);
#endif
    asm("RIM");	//ʹ��ȫ���ж�
}

void sim_uart_printf_it(uint8_t data)
{
    //PB_ODR |= bit0;
    //PB_DDR |= bit0;
    //PB_CR1 |= bit0;

    //asm("SIM");  //�����ж�

    //������ʼλ
    GPIOB->ODR &= ~GPIO_Pin_0;

#if (SYS_CLK_FREQ == SYS_CLK_FREQ_16M)
asm
(
    //���汾����ʹ�õ��ļĴ���
    "PUSHW Y\n"  //2T
    "PUSHW X\n"  //2T
    
    "LDW Y,#9\n"  //2T��������ѭ��
    "LDW X,#41\n" //2T����ֵ��ʵ��õ�
"Delay0: DECW X\n" //��1��1��ָ������
    "JRNE Delay0\n" //�ж���ת����תҪ2T������Ҫ1T
    "NOP\n"

    //�������µĵ�ƽ����Ҫ6T
"SEND_DATA:DECW Y\n"  //1T,�ж�ѭ���Ƿ����
    "JREQ STOP\n"//��������ת��2T������ת��1T
    "NOP\n"//1T������δ��ת��1T
    
    "SRL A\n" //1T,�߼����ƣ�ͬʱbit0��C��־λ��
    "JRC SET1\n"  //����1����ת����ת2T������ת1T
    "NOP\n"//1T������δ��ת��1T
    "BRES 0x5005,#0\n" //1T�������
    "JP Delay1\n"  //1T����������ת
"SET1:BSET 0x5005,#0\n" //1T�������
    "NOP\n"

"Delay1:NOP\n"
    "LDW X,#41\n" //2T��ʵ��õ�
"LOOP1:DECW X\n"  //1T
    "JRNE LOOP1\n"//��ת��2T������ת��1T
    
    "JP SEND_DATA\n"  //1T����������ת


    //����2λʱ����ֹͣλ
"STOP:TNZW X\n" //2T,ֻ��Ϊ����ʱ2T
    "TNZW X\n" //2T,ֻ��Ϊ����ʱ2T
    "BSET 0x5005,#0\n" //1T�������
    "LDW X,#92\n" //2T����ʱ139x2-2=276T
"LOOP2:DECW X\n"  //1T
    "JRNE LOOP2\n"//��ת��2T������ת��1T
    "NOP\n"

    //�ָ�������ʹ�õ��ļĴ���
    "POPW X\n"
    "POPW Y"
)       ;
#elif (SYS_CLK_FREQ == SYS_CLK_FREQ_4M)
    //ÿ��bit��ʱ34T
asm
(
    //���汾����ʹ�õ��ļĴ���
    "PUSHW Y\n"
    "PUSHW X\n"
    
    "LDW Y,#9\n"  //2T��������ѭ��
    "LDW X,#6\n" //2T����ֵ
    //������ʱ19T
"Delay0: DECW X\n" //��1��1��ָ������
    "JRNE Delay0\n" //�ж���ת����תҪ2T������Ҫ1T
    "NOP\n"
    "NOP\n"

    //�������µĵ�ƽ����Ҫ6T
"SEND_DATA:DECW Y\n"  //1T,�ж�ѭ���Ƿ����
    "JREQ STOP\n"//��������ת��2T������ת��1T
    "NOP\n"//1T������δ��ת��1T

    "SRL A\n" //1T,�߼����ƣ�ͬʱbit0��C��־λ��
    "JRC SET1\n"  //����1����ת����ת2T��ȷ�ϲ���ת1T
    //"NOP\n"//1T������δ��ת��1T��ʵ�����
    "BRES 0x5005,#0\n" //1T�������
    "JP Delay1\n"  //1T����������ת
"SET1:BSET 0x5005,#0\n" //1T�������
    "NOP\n"
    "NOP\n" //ʵ�����
    "NOP\n"

    //���¸�bit�仯ǰ����Ҫ����ʱ26T
"Delay1:LDW X,#7\n" //2T��
    //������ʱ21T
"LOOP1:DECW X\n"  //1T
    "JRNE LOOP1\n"//��ת��2T������ת��1T
    "JP SEND_DATA\n"  //1T����������ת


    //����2λʱ����ֹͣλ
"STOP:TNZW X\n" //2T,ֻ��Ϊ����ʱ2T
    "NOP\n" //1T,ֻ��Ϊ����ʱ1T
    "BSET 0x5005,#0\n" //1T�������

    //��Ҫ��ʱ68T
    "LDW X,#22\n" //2T
    //��������ʱ66T
"LOOP2:DECW X\n"  //1T
    "JRNE LOOP2\n"//��ת��2T������ת��1T
    "NOP\n"

    //�ָ�������ʹ�õ��ļĴ���
    "POPW X\n"
    "POPW Y"    
);
#endif
//    asm("RIM");	//ʹ��ȫ���ж�
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