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

