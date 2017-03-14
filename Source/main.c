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



