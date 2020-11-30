# BLE_MESH_PROJECT
WH-BLE103 based on STM8L151K4

## DAY_LOG 
* 11-25 
```c
void bsp_key_it(void)
{
    delay_ms_1(20);//avoid shaking
    if (!KEY_READ())
    {
        LEDG_R();
        LEDR_R();
    }
    
    
}
```
This is a button trigger function, the interested thing is I can only light the green one. But when I used *LEDR_R()* in the *main(void)* function, it works!<br>
Markdown here, and I will try to fix it.
* 11-28
```c
// EXTI_SetPortSensitivity(KEY_EXTI_PORT, EXTI_Trigger_Falling);       //key trigger falling
    EXTI_SetPinSensitivity(KEY_EXTI_PIN, EXTI_Trigger_Falling); //key trigger falling

    //IT Priority
    // ITC_SetSoftwarePriority(EXTIB_IRQn,ITC_PriorityLevel_3); //key first
    // ITC_SetSoftwarePriority(EXTI4_IRQn,ITC_PriorityLevel_2); //led second
```
The problem is fixed. The reason is that the button pin(PB4) and the link pin(PC4) are all using the same EXTI4, which is the program never considered. So I add a function used to detect which pin is really triggered.<br>

