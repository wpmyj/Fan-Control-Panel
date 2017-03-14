/******************************************************************************
* @file    uart.c
* @author  xxc
* @version V1.0
* @date    2017-03-14
* @brief   串口程序
******************************************************************************
* @attention
*
*
*
*******************************************************************************/ 

#include "main.h"
#include "uart.h"




/**
* @Function ：串口1初始函数
* @brief    ：占用定时器1，波特率9600，系统频率：11.0592MHz
* @input    ：None 
* @output   ：None
* @retval   ：None
**/
void Uart_1_Init(void)
{
    SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x40;		//定时器1时钟为Fosc,即1T
	AUXR &= 0xFE;		//串口1选择定时器1为波特率发生器
	TMOD &= 0x0F;		//设定定时器1为16位自动重装方式
    TH1 = TL1 = -(FOSC/32/UART_1_BAUD); 
	// TL1 = 0xE0;		//设定定时初值  波特率9600，系统频率：11.0592MHz
	// TH1 = 0xFE;		//设定定时初值
	ET1 = 0;		    //禁止定时器1中断
	TR1 = 1;		    //启动定时器1
    ES = 1;             //启动串口1中断
}

/**
* @Function ：串口1中断程序
* @brief    ：
* @input    ：None 
* @output   ：None
* @retval   ：None
**/
void Uart_1_Isr(void) interrupt 4 using 1
{
    if (RI)
    {
        RI = 0;             //Clear receive interrupt flag

    }
    if (TI)
    {
        TI = 0;             //Clear transmit interrupt flag

    }
}


/**
* @Function : 串口1发送数据函数
* @brief    ：
* @input    ：dat - 发送的数据 
* @output   ：None
* @retval   ：None
**/
void Uart_1_SendByte(uint8_t dat)
{
 	SBUF = dat;
 	while(!TI);
     {
    	TI = 0;
     }
}

/**
* @Function : 串口1发送字符串函数
* @brief    ：
* @input    ：*s - 发送字符串的地址 
* @output   ：None
* @retval   ：None
**/
void Uart_1_SendString(uint8_t *s)
{
    while (*s)
    {
        Uart_1_SendByte(*s++);
    }
}



/**
* @Function ：串口2初始函数
* @brief    ：使用波特率发生器
* @input    ：None 
* @output   ：None
* @retval   ：None
**/
void Uart_2_Init(void)
{

	AUXR &= 0xF7;		//波特率不倍速
	S2CON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x04;		//独立波特率发生器时钟为Fosc,即1T
    BRT = -(FOSC/32/UART_2_BAUD);  
	//BRT = 0xDC;		//设定独立波特率发生器重装值 波特率9600，系统频率：11.0592MHz
	AUXR |= 0x10;		//启动独立波特率发生器

    IE2 = 0x01;            //启动串口2中断
}

/**
* @Function ：串口2中断程序
* @brief    ：
* @input    ：None 
* @output   ：None
* @retval   ：None
**/
void Uart_2_Isr(void) interrupt 8 using 1
{
    if (S2RI)
    {
        S2RI = 0;     //Clear receive interrupt flag

    }
    if (S2TI)
    {
        S2TI = 0;     //Clear transmit interrupt flag

    }
}


/**
* @Function : 串口2发送数据函数
* @brief    ：
* @input    ：dat - 发送的数据 
* @output   ：None
* @retval   ：None
**/
void Uart_2_SendByte(uint8_t dat)
{
 	S2BUF = dat;
 	while(!S2TI);
     {
    	S2TI = 0;
     }
}

/**
* @Function : 串口2发送字符串函数
* @brief    ：
* @input    ：*s - 发送字符串的地址 
* @output   ：None
* @retval   ：None
**/
void Uart_2_SendString(uint8_t *s)
{
    while (*s)
    {
        Uart_2_SendByte(*s++);
    }
}






