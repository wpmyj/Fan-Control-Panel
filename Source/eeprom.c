/******************************************************************************
* @file    eeprom.c
* @author  xxc
* @version V1.0
* @date    2017-03-18
* @brief   存储模块
******************************************************************************
* @attention
*
*
*
*******************************************************************************/ 

#include "main.h"
#include "eeprom.h"
#include "intrins.h"

sbit SDA = P2^5;    //24C02数据端口
sbit SCL = P2^6;    //24C02时钟端口

/**
* @Function ：延时子程序
* @brief    ：
* @input    ：None 
* @output   ：None
* @retval   ：None
**/
void delay(void)
{
	_nop_();
	_nop_();
}

/**
* @Function ：24c02开始信号
* @brief    ：
* @input    ：None 
* @output   ：None
* @retval   ：None
**/
void start(void)
{
	SDA=1;
	delay();
	SCL=1;
	delay();
	SDA=0;
	delay();
}

/**
* @Function ：24c02停止信号
* @brief    ：
* @input    ：None 
* @output   ：None
* @retval   ：None
**/
void stop(void)
{
	SDA=0;
	delay();
	SCL=1;
	delay();
	SDA=1;
	delay();
}

/**
* @Function ：24c02应答信号
* @brief    ：
* @input    ：None 
* @output   ：None
* @retval   ：None
**/
void respons(void)
{
	uint8_t i;
	SCL = 1;
	delay();
	while((SDA == 1)&&(i < 250))
	{
		i++;
	}
	SCL = 0;
	delay();
}

/**
* @Function ：24c02初始化信号
* @brief    ：
* @input    ：None 
* @output   ：None
* @retval   ：None
**/
void Eeprom_Init(void)
{
	SDA = 1;
	delay();
	SCL = 1;
	delay();
}

/**
* @Function ：24c02写入一个字节
* @brief    ：
* @input    ：写入的字节
* @output   ：None
* @retval   ：None
**/
void write_byte(uint8_t date)
{
	uint8_t i, temp;
	
	temp = date;
	for(i = 0; i < 8; i++)
	{
		temp = temp << 1;
		SCL = 0;
		delay();
		SDA = CY;
		delay();
		SCL = 1;
		delay(); 
	}
	SCL = 0;
	delay();
	SDA = 1;
	SCL = 1;
	delay();  
}

/**
* @Function ：24c02读出一个字节
* @brief    ：
* @input    ：None 
* @output   ：返回一个字节
* @retval   ：None
**/
uint8_t read_byte(void)
{
	uint8_t i, k;

	SCL = 0;
	delay(); 
	SDA = 1;
	delay(); 
	for(i = 0; i < 8; i++)
	{
		SCL = 1;
		delay(); 
		k = (k << 1) | SDA;
		SCL = 0;
		delay(); 
	}
	return k;
}


/**
* @Function ：24c02在指定地址写入数据
* @brief    ：
* @input    ：None 
* @output   ：None
* @retval   ：None
**/
void Eeprom_Write_Add(uint8_t address, uint8_t date)
{
	start();
	write_byte(0xae);
	respons();
	write_byte(address);
	respons();
	write_byte(date);
	respons();
	stop();
}

/**
* @Function ：24c02读取在指定地址数据
* @brief    ：
* @input    ：adress ：地址 
* @output   ：None
* @retval   ：数据
**/
uint8_t Eeprom_Read_Add(uint8_t adress)
{
	uint8_t date;

	start();
	write_byte(0xae);
	respons();
	write_byte(adress);
	respons();
	start();
	write_byte(0xaf);
	respons();
	date = read_byte();
	stop();
	return date;
}



