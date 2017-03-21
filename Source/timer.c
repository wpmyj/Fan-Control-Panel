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
#include "timer.h"
#include "intrins.h"

#define T100Hz  (FOSC / 12 / 100)

bool Ico_Flash_flg = ON;
bool Send_Status_flg = ON;
bool Update_Sensor_flg = ON;
bool Auto_Timer_flg = OFF;


uint8_t Time_Base_1S, Time_Base_1M;
uint8_t Time_Base_Model_1M;
uint8_t cnt;
uint16_t value;

/**
* @Function ：定时器中断子程序
* @brief    ：
* @input    ：None 
* @output   ：None
* @retval   ：None
**/
void PCA_isr() interrupt 7 using 1
{
    CCF0 = 0;                       //Clear interrupt flag
    CCAP0L = value;
    CCAP0H = value >> 8;            //Update compare value
    value += T100Hz;
    if (cnt-- == 0)
    {
        cnt = 100;                  //Count 100 times
        Time_Base_1S++;
        if(Time_Base_1S > 60)
        {
            Time_Base_1S = 0;
            Time_Base_1M++;
            if(Auto_Timer_flg == ON)
            {
                Time_Base_Model_1M++;
            }

            if(Update_Sensor_flg == ON)
            {
                Update_Sensor_flg = OFF;
            }
        }

        if(Ico_Flash_flg == ON)
        {
            Ico_Flash_flg = OFF;
        }
        if(Send_Status_flg == ON)
        {
            Send_Status_flg = OFF;
        }
    }
}

/**
* @Function ：定时器初始化
* @brief    ：
* @input    ：None 
* @output   ：None
* @retval   ：None
**/
void Timer_Init(void)
{
    CCON = 0;                       //Initial PCA control register
                                    //PCA timer stop running
                                    //Clear CF flag
                                    //Clear all module interrupt flag
    CL = 0;                         //Reset PCA base timer
    CH = 0;
    CMOD = 0x00;                    //Set PCA timer clock source as Fosc/12
                                    //Disable PCA timer overflow interrupt
    value = T100Hz;
    CCAP0L = value;
    CCAP0H = value >> 8;            //Initial PCA module-0
    value += T100Hz;
    CCAPM0 = 0x49;                  //PCA module-0 work in 16-bit timer mode and enable PCA interrupt

    CR = 1;                         //PCA timer start run
}


/**
* @Function ：us延时子程序
* @brief    ：
* @input    ：xus：xus * 10 延时微秒 
* @output   ：None
* @retval   ：None
**/
void Delay_x_10us(uint16_t xus)
{ 
    uint8_t i;
    uint16_t u16;
    
	for(u16 = 0; u16 < xus; u16++)
	{
        _nop_();
        _nop_();
        _nop_();
        i = 24;
        while (--i);
    }

}

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
    uint16_t u16;
    uint8_t i, j;

	for(u16 = 0; u16 < xms; u16++)
	{
        //_nop_();
        i = 11;
        j = 190;
        do
        {
            while (--j);
        } while (--i);
    }
	// uint16_t i,j;
	// for(i=0;i<xms;i++)
	// 	for(j=0;j<110;j++);
}