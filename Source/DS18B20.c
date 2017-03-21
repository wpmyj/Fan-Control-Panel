/******************************************************************************
* @file    ds18b20.c
* @author  xxc
* @version V1.0
* @date    2017-03-21
* @brief   DS18B20传感器函数
******************************************************************************
* @attention
*
*
*
*******************************************************************************/ 

#include "main.h"


sbit DQ = P2^1;


uint16_t temper_change()
{
    uint temper;
    float tp;
    temper=temper_read();
    if(temper<0)    //考虑负温度的情况
    {
       temper=temper-1;
       temp=~temp;
       tp=temper*0.0625;  //16位温度转换成10进制的温度
       temper=tp*100+0.5;   //留两个小数点，并四舍五入
    }
    else
    {
    tp=temper*0.0625;  //16位温度转换成10进制的温度
    temper=tp*100+0.5;  //留两个小数点，并四舍五入
    }
    return temper;
}

uint read_temper ()
{    
   uchar a,b;         
   uint t=0;
   DS18B20_init();       
   delay_us(15);
   write_byte(0xcc);   //跳过ROM操作命令
   write_byte(0x44);     //发送启动温度转换命令
   DS18B20_init();       
   delay_us(15);
   write_byte(0xcc);    //跳过ROM操作命令
   write_byte(0xbe);      //发送读温度寄存器命令
   a=read_byte();    //先读低八位
   b=read_byte();      //再读高八位
   t=b;        
   t<<=8;      //左移八位
   t=t|a;      //t为16位的数，使高八位为b的值，低八位为a的值  
   return t;    //返回温度值
}


uint8_t DS18B20_Read_Byte(void)    //读一个字节
{
    uint8_t i, dat = 0;

    for(i = 0; i < 8; i++)
    {
        DQ = 0;           //先将总线拉低1us
        _nop_();
        DQ = 1;             //然后释放总线
        _nop_();_nop_();
        _nop_();_nop_();
        if(DQ)
        {
            dat=dat|0x80;   //每次读一位
        }
        dat=dat>>1;       //从最低位开始读
        delay_us(10);   //读取完之后等待48us再接着读取下一个数
    }
    return dat;
}

/**
* @Function ：DS18B20写入一个字节
* @brief    ：
* @input    ：dat：写入的数据
* @output   ：None
* @retval   ：None
**/
void DS18B20_Write_Byte(uint8_t dat)   //写一个字节
{
    uint8_t i;
    for(i = 0; i < 8; i++)
    {
        DQ = 0;  //每写入一位数据之前先把总线拉低1us
        _nop_();
        DQ = dat & 0x01;    //取最低位写入
        Delay_x_10us(7);    //延时70us，持续时间最少60us
        DQ = 1;   //然后释放总线
        dat = dat >> 1;    //从低位开始写
    }
    Delay_10us();
}


/**
* @Function ：DS18B20初始化程序
* @brief    ：
* @input    ：None
* @output   ：None
* @retval   ：是否初始化成功
**/
bool DS18B20_init(void)
{
       DQ = 1;
       Delay_10us();     //稍作延时
       DQ = 0;
       Delay_x_10us(80);    //延时80*10-800us,在480到960us之间
       DQ = 1;
       i = 0;
       while(DQ)    //等待DS18B20拉低总线
       {
           Delay_x_10us(100);
           i++;
           if(i > 5)            //约等待>5MS
           {
               return FLASE;    //初始化失败
           }    
       }
       return TRUE;
}

