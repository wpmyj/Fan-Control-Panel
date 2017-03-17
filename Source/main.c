/******************************************************************************
* @file    main.c
* @author  xxc
* @version V1.0
* @date    2017-03-14
* @brief   控制面板主程序
******************************************************************************
* @attention
*
*
*
*******************************************************************************/ 

#include "main.h"
#include "intrins.h"
#include "uart.h"






void CheckKey(void)
{
	uint8_t key_press_num;    

	if(!KEY_SWITCH)                //如果检测到低电平，说明按键按下
    {
		 Delay_ms(10);          //延时去抖，一般10-20ms
	     if(!KEY_SWITCH)           //再次确认按键是否按下，没有按下则退出
		 {	 	
	   		while(!KEY_SWITCH)
	    	{
				key_press_num++;
        		Delay_ms(10);               //10x150=1500ms=2s 
				if(key_press_num==150)      //大约2s
		  		{
		  			key_press_num=0;        //如果达到长按键标准,则进入长按键动作
           			while(!KEY_SWITCH)
		       		{
                        

					}
		    	}
			}
			key_press_num=0;        //防止累加造成错误识别
            //



	   	}
	}
}






/**
* @Function ：main
* @brief    ：
* @input    ：None 
* @output   ：None
* @retval   ：None
**/
void main()
{

    Uart_1_Init();


    // SendString("STC12C5A60S2\r\nUart Test !\r\n");
    while(1);

}



