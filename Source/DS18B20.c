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

uint temper_change()
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

uchar read_byte()    //读一个字节
{
  uchar i,dat=0;
  for(i=0;i<8;i++)
  {
     DQ=0;  //先将总线拉低1us
     _nop_();
     DQ=1;  //然后释放总线
     _nop_();_nop_();
     _nop_();_nop_();
     if(DQ) dat=dat|0x80;   //每次读一位
     dat=dat>>1;       //从最低位开始读
     delay_us(10);   //读取完之后等待48us再接着读取下一个数
   }
   return dat;
}

void write_byte(uchar dat)   //写一个字节
{
   uchar i;
   for(i=0;i<8;i++)
   {
      DQ=0;  //每写入一位数据之前先把总线拉低1us
      _nop_();
     DQ=dat&0x01;    //取最低位写入
     delay_us(10);   //延时68us，持续时间最少60us
     DQ=1;   //然后释放总线
     dat=dat>>1;    //从低位开始写
   }
   delay_us(10);
}



void DS18B20_init()
{
       DQ=1;
       delay_us(1);     //稍作延时
       DQ=0;
       delay_us(80);    //延时480到960us
       DQ=1;
       i = 0;
       while(DQ)    //等待DS18B20拉低总线
       {
           delay_us(100);
           i++;
           if(i>5)//约等待>5MS
           {
               return 0;//初始化失败
           }    
       }
}

bit reset_and_getack()
{
   bit ack;

   DQ = 1;
   delay_20us(1);
   DQ = 0;
   delay_20us(60); //delay 610us
   DQ = 1;  //release bus
   delay_20us(6);  //delay 70us;
   ack = DQ;
   while(!DQ);  //wait ds18b20  release bus 
   delay_20us(50); // delay 510 us
   return ack;
} 

