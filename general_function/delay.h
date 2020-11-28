/******************************************************************************
* Copyright 2020-2025 cenlinbo
* FileName :delay.h 
* Desc     :基于STM8L051XX/STM8L151XX系列单片机平台实现的延时功能模块
* 
* 
* Author   :CenLinbo
* Date     :2020/06/09
* Notes    :根据配置的系统时钟实现不同时钟频率下的延时函数

*           2020/08/22 11:32, add this by CenLinbo
*           1、要实现10us内的准确延时，需要系统主频至少在1M以上
* 
* -----------------------------------------------------------------
* Histroy:v1.0   2020/06/09, CenLinbo create this file

*           2020/08/18 16:17, add this by CenLinbo
*           1、由于在使用500K和250KHz的主频时无法实现1us的精确延时，而且平时
               几乎不使用如何此低频的时钟作为系统时钟，因此删除此时钟相关的延时代码
*         
******************************************************************************/
#ifndef _DELAY_H_     
#define _DELAY_H_    


/*------------------------------- Includes ----------------------------------*/
#include "main.h"

/*----------------------------- Global Defines ------------------------------*/


/*----------------------------- Global Typedefs -----------------------------*/


/*----------------------------- External Variables --------------------------*/
extern volatile char rtc_delay_flag;  //用于使用RTC延时唤醒时的标志位，需要在RTC唤醒中断中清0

/*------------------------ Global Function Prototypes -----------------------*/
#if ((SYS_CLK_FREQ == SYS_CLK_FREQ_2M) || (SYS_CLK_FREQ == SYS_CLK_FREQ_4M) || (SYS_CLK_FREQ == SYS_CLK_FREQ_8M) || (SYS_CLK_FREQ == SYS_CLK_FREQ_16M))
/******************************************************************************
* Name     :delay_us_1 
*
* Desc     :
* Param_in :n：延时的us数，最大只能输入16383，ms级延时建议使用ms级延时函数
* Param_out:
* Return   :
* Global   :
* Note     :1、调用函数前，LD装载参数到X寄存器要2T
            2、调用函数CALL要4T，RET返回要4T
            3、主频16M：最小延时2us，大于1us为精确延时
            4、主频8M：最小延时3us，大于2us为精确延时
            5、主频4M：最小延时4us，大于3us为精确延时
            6、主频2M：最小延时11us，大于10us且为偶数时精确延时，为奇数时比理想值少1us
            7、主频1M、500K、250K时此函数的最低延时太大，不能准确实现100us内的延时，故不实现
            
* Author   :CenLinbo
* -------------------------------------
* Log     :2020/06/11, Create this function by CenLinbo

*           2020/08/21 10:22, add this by CenLinbo
*           1、重新对16M/8M/4M/2M主频下的延时作计算并测试，目前可用
 ******************************************************************************/
extern void delay_us_1(uint16_t n);

/******************************************************************************
* Name      :delay_ms_1 
*
* Desc     :基于us延时实现的ms级延时，由于1000us的延时是相当准确的，所以ms级延时也很准确
* Param_in :
* Param_out:
* Return   :
* Global   :
* Note     :
* Author   :bobde163
* -------------------------------------
* Log     :2020/06/17 21:16:22, Create this function by bobde163
 ******************************************************************************/
extern void delay_ms_1(uint16_t n);
#endif

/******************************************************************************
* Name     :delay_ms_2 
*
* Desc     :通过片上RTC的自动唤醒功能实现低功耗延时
* Param_in :n：要延时的ms数，范围1~32767ms
* Param_out:
* Return   :
* Global   :
* Note     :
* Author   :CenLinbo
* -------------------------------------
* Log     :2020/08/22, Create this function by CenLinbo
 ******************************************************************************/

extern void delay_ms_2(uint16_t n);
// extern char *Connect(char* str1, char* str2, char* p);
extern char *connect2(char *a, char *b);
extern char *connect3(char *a, char *b, char *d);
#if 1
//系统时钟1M时，指令周期为1us
#if (SYS_CLK_FREQ == SYS_CLK_FREQ_1M)
#define delay_1us()    nop();
#define delay_2us()    asm("TNZW X")
#define delay_5us()    {asm("TNZW X");asm("TNZW X");nop();}

extern void delay_10us(void);


//系统时钟2M时，指令周期为0.5us
#elif (SYS_CLK_FREQ == SYS_CLK_FREQ_2M)
#define delay_1us()    asm("TNZW X")
#define delay_2us()    {asm("TNZW X");asm("TNZW X");}

//delay_5us使用函数实现
extern void delay_5us(void);

#define delay_10us()   {delay_5us();delay_5us();}

//系统时钟4M时，指令周期为0.25us
#elif (SYS_CLK_FREQ == SYS_CLK_FREQ_4M)
#define delay_1us()    {asm("TNZW X");asm("TNZW X");}


#define delay_5us()    delay_us_1(5)
#define delay_10us()   delay_us_1(10)

//系统时钟8M时，指令周期为0.125us
#elif (SYS_CLK_FREQ == SYS_CLK_FREQ_8M)
extern void delay_1us(void);

#define delay_2us()   {delay_1us();delay_1us();}
#define delay_5us()    delay_us_1(5)
#define delay_10us()   delay_us_1(10)

//系统时钟16M时，指令周期为0.0625us
#elif (SYS_CLK_FREQ == SYS_CLK_FREQ_16M)
extern void delay_1us(void);

#define delay_2us()    delay_us_1(2)
#define delay_5us()    delay_us_1(5)
#define delay_10us()   delay_us_1(10)

#else
#error "Invalid system clock value...."
#endif

#endif


#endif //_DELAY_H_

