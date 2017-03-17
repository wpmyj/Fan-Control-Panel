/******************************************************************************
* @file    timer.c
* @author  xxc
* @version V1.0
* @date    2017-03-17
* @brief   
******************************************************************************
* @attention
*
*
*
*******************************************************************************/ 


#include "main.h"









/**
* @Function ：10us延时子程序
* @brief    ：
* @input    ：None 
* @output   ：None
* @retval   ：None
**/
void Delay_10us(void)
{ 
    uint8_t i;

    _nop_();
    _nop_();
    _nop_();
    i = 24;
    while (--i);
} 

/**
* @Function ：ms延时子程序
* @brief    ：
* @input    ：xms：延时毫秒 
* @output   ：None
* @retval   ：None
**/
void Delay_ms(uint16_t xms)
{
	uint16_t i,j;
	for(i=0;i<xms;i++)
		for(j=0;j<110;j++);
}