/******************************************************************************
* @file    display.c
* @author  xxc
* @version V1.0
* @date    2017-03-17
* @brief   液晶显示
******************************************************************************
* @attention
*
*
*
*******************************************************************************/ 

#include "main.h"
#include "display.h"
#include "timer.h"


/******************************************************** 
功能描述:HT1621指令集
********************************************************/
#define BIAS    0x52    //0b1000 0101 0010   1/3duty 4com
#define SYSDIS  0X00    //0b1000 0000 0000   关振系统荡器和LCD偏压发生器
#define SYSEN   0X02    //0b1000 0000 0010   打开系统振荡器
#define LCDOFF  0X04    //0b1000 0000 0100   关LCD偏压
#define LCDON   0X06    //0b1000 0000 0110   打开LCD偏压
#define XTAL    0x28    //0b1000 0010 1000   外部接时钟
#define RC256   0X30    //0b1000 0011 0000   内部时钟
#define TONEON  0X12    //0b1000 0001 0010   打开声音输出
#define TONEOFF 0X10    //0b1000 0001 0000   关闭声音输出 
#define WDTDIS  0X0A    //0b1000 0000 1010   禁止看门狗
/******************************************************** 
功能描述:LCD+/变量/标志位/变量定义
********************************************************/
sbit HT1621_CS  = P2^0;   // - - HT1621使能引脚
sbit HT1621_WR  = P2^1;   // - - HT1621时钟引脚
sbit HT1621_DAT = P2^2;   // - - HT1621数据引脚


/* 功能描述:LCD清屏数组 */
uint8_t code Ht1621Tab[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

uint8_t code Ht1621_Tube_1[]={0x02,0x07,0x01,0x05,0x04,0x04,0x00,0x07,0x00,0x04};  //0 1 2 3 4 5 6 7 8 9
uint8_t code Ht1621_Tube_2[]={0x0F,0x06,0x0B,0x0F,0x06,0x0D,0x0D,0x07,0x0F,0x0F};  //0 1 2 3 4 5 6 7 8 9

uint8_t Ht1621_Tab[16] = {0x00};


/**
* @Function ：开机屏幕显示
* @brief    ：
* @input    ：None 
* @output   ：None
* @retval   ：None
**/
// void Display_Init(void)
// {
//     // Display_PM2_5_Ico();
//     // Display_CO2_Ico();
//     // Display_Fan_Ico();
    


// }

/**
* @Function ：清除屏幕显示
* @brief    ：写入Ht1621Tab数据组
* @input    ：None 
* @output   ：None
* @retval   ：None
**/
void Display_Clear(void)
{
    Ht1621WrAllData(0, Ht1621Tab, 16);           // - - 清除1621寄存器数据，实现清屏                       
}

/**
* @Function ：显示/隐藏外循环图标
* @brief    ：
* @input    ：-sw 显示操作，TRUE显示，FLASE隐藏
* @output   ：None
* @retval   ：None
**/
void Display_Exter_Ico(bool sw)
{
    if(sw)
    {
        Ht1621_Tab[7] |= 0x08;
    }
    else
    {
        Ht1621_Tab[7] |= 0x08;        
        Ht1621_Tab[7] ^= 0x08;
    }      
    Ht1621WrOneData(7, Ht1621_Tab[7]);          
}

/**
* @Function ：显示/隐藏内循环图标
* @brief    ：
* @input    ：-sw 显示操作，TRUE显示，FLASE隐藏
* @output   ：None
* @retval   ：None
**/
void Display_Inter_Ico(bool sw)
{
    if(sw)
    {
        Ht1621_Tab[6] |= 0x08;
    }
    else
    {
        Ht1621_Tab[6] |= 0x08;        
        Ht1621_Tab[6] ^= 0x08;
    }      
    Ht1621WrOneData(6, Ht1621_Tab[6]);                                  
}

/**
* @Function ：显示/隐藏手动模式图标
* @brief    ：
* @input    ：-sw 显示操作，TRUE显示，FLASE隐藏
* @output   ：None
* @retval   ：None
**/
void Display_Manual_Ico(bool sw)
{
    if(sw)
    {
        Ht1621_Tab[10] |= 0x08;
    }
    else
    {
        Ht1621_Tab[10] |= 0x08;        
        Ht1621_Tab[10] ^= 0x08;
    }   
    Ht1621WrOneData(10, Ht1621_Tab[10]);                         
}

/**
* @Function ：显示/隐藏自动模式图标
* @brief    ：
* @input    ：-sw 显示操作，TRUE显示，FLASE隐藏
* @output   ：None
* @retval   ：None
**/
void Display_Auto_Ico(bool sw)
{
    if(sw)
    {
        Ht1621_Tab[2] |= 0x08;
    }
    else
    {
        Ht1621_Tab[2] |= 0x08;
        Ht1621_Tab[2] ^= 0x08;
    }      
    Ht1621WrOneData(2, Ht1621_Tab[2]);             
}

/**
* @Function ：显示/隐藏滤网提示图标
* @brief    ：
* @input    ：-sw 显示操作，TRUE显示，FLASE隐藏
* @output   ：None
* @retval   ：None
**/
void Display_Filte_Ico(bool sw)
{
    if(sw)
    {
        Ht1621_Tab[14] |= 0x08;
    }
    else
    {
        Ht1621_Tab[14] |= 0x08;        
        Ht1621_Tab[14] ^= 0x08;
    }   
    Ht1621WrOneData(14, Ht1621_Tab[14]);        
}



/**
* @Function ：显示PM2.5图标
* @brief    ：
* @input    ：
* @output   ：None
* @retval   ：None
**/
void Display_PM2_5_Ico(bool sw)
{  
    if(sw)
    {
        Ht1621_Tab[0] |= 0x08;
    }
    else
    {
        Ht1621_Tab[0] |= 0x08;
        Ht1621_Tab[0] ^= 0x08;
    }   
    Ht1621WrOneData(0, Ht1621_Tab[0]);
}

/**
* @Function ：显示CO2图标
* @brief    ：
* @input    ：
* @output   ：None
* @retval   ：None
**/
void Display_CO2_Ico(bool sw)
{
    //Ht1621WrOneData(8, 0x08);  
    if(sw)
    {
        Ht1621_Tab[8] |= 0x08;
    }
    else
    {
        Ht1621_Tab[8] |= 0x08;
        Ht1621_Tab[8] ^= 0x08;
    }   
    Ht1621WrOneData(8, Ht1621_Tab[8]);            
}

/**
* @Function ：显示风叶图标
* @brief    ：
* @input    ：
* @output   ：None
* @retval   ：None
**/
void Display_Fan_Ico(uint8_t sw)
{
    if(sw)
    {
        Ht1621_Tab[6] |= 0x07;
    }
    else
    {
        Ht1621_Tab[6] |= 0x07;
        Ht1621_Tab[6] ^= 0x07;
    }   
    Ht1621WrOneData(6, Ht1621_Tab[6]);          
}

/**
* @Function ：显示风量等级
* @brief    ：
* @input    ：-level 风量等级。1为睡眠，2为和风，3为强风 
* @output   ：None
* @retval   ：None
**/
void Display_WindLevel(uint8_t level)
{
    switch(level)
    {
        case 1: 
            Ht1621_Tab[7] |= 0x07;
            Ht1621_Tab[7] ^= 0x03;
            Ht1621_Tab[6] |= 0x07;
            Ht1621_Tab[6] ^= 0x06;
        break;
        case 2: 
            Ht1621_Tab[7] |= 0x07;
            Ht1621_Tab[7] ^= 0x05;
            Ht1621_Tab[6] |= 0x07;
            Ht1621_Tab[6] ^= 0x04;
        break;
        case 3: 
            Ht1621_Tab[7] |= 0x07;
            Ht1621_Tab[7] ^= 0x06;
            Ht1621_Tab[6] |= 0x07;
        break;
    }
    Ht1621WrOneData(6, Ht1621_Tab[6]);              
    Ht1621WrOneData(7, Ht1621_Tab[7]);          
}

/**
* @Function ：显示外循环风量
* @brief    ：
* @input    ：-level 风量等级。1为睡眠，2为和风，3为强风 
* @output   ：None
* @retval   ：None
**/
// void Display_Ex_WindLevel(uint8_t level)
// {
//     // Ht1621WrOneData(6, Ht1621Tab6[level]);
//     // Ht1621WrOneData(7, Ht1621Tab7[level]);           
// }

/**
* @Function ：显示内循环风量
* @brief    ：
* @input    ：-level 风量等级。1为睡眠，2为和风，3为强风 
* @output   ：None
* @retval   ：None
**/
// void Display_In_WindLevel(uint8_t level)
// {
//     // Ht1621WrOneData(6, Ht1621Tab8[level]);
//     // Ht1621WrOneData(7, Ht1621Tab9[level]);           
// }


/**
* @Function ：关闭显示PM2.5数值
* @brief    ：
* @input    ：None 
* @output   ：None
* @retval   ：None
**/
void Display_PM2_5_clean(void)
{

    Ht1621_Tab[0] &= 0x80;
    Ht1621_Tab[1] = 0x00;
    Ht1621WrOneData(0, Ht1621_Tab[0]);          
    Ht1621WrOneData(1, Ht1621_Tab[1]);   

    Ht1621_Tab[2] &= 0x80;
    Ht1621_Tab[3] = 0x00;
    Ht1621WrOneData(2, Ht1621_Tab[2]);          
    Ht1621WrOneData(3, Ht1621_Tab[3]); 

    Ht1621_Tab[4] &= 0x80;
    Ht1621_Tab[5] = 0x00;
    Ht1621WrOneData(4, Ht1621_Tab[4]);          
    Ht1621WrOneData(5, Ht1621_Tab[5]); 
}

/**
* @Function ：显示PM2.5数值
* @brief    ：
* @input    ：-value 数值 
* @output   ：None
* @retval   ：None
**/
void Display_PM2_5_value(uint16_t value)
{
    uint8_t hun, ten, one;

    hun = value / 100 % 10;
    ten = value / 10 % 10;
    one = value % 10;

    // Ht1621_Tab[0] |= 0x07;
    // Ht1621_Tab[0] ^= Ht1621_Tube_1[1];
    // Ht1621_Tab[1] = Ht1621_Tube_2[1];
    // Ht1621WrOneData(0, 0x08);          
    // Ht1621WrOneData(1, Ht1621_Tab[1]); 

    Ht1621_Tab[0] |= 0x07;
    Ht1621_Tab[0] ^= Ht1621_Tube_1[hun];
    Ht1621_Tab[1] = Ht1621_Tube_2[hun];
    Ht1621WrOneData(0, Ht1621_Tab[0]);          
    Ht1621WrOneData(1, Ht1621_Tab[1]);   

    Ht1621_Tab[2] |= 0x07;
    Ht1621_Tab[2] ^= Ht1621_Tube_1[ten];
    Ht1621_Tab[3] = Ht1621_Tube_2[ten];
    Ht1621WrOneData(2, Ht1621_Tab[2]);          
    Ht1621WrOneData(3, Ht1621_Tab[3]); 

    Ht1621_Tab[4] |= 0x07;
    Ht1621_Tab[4] ^= Ht1621_Tube_1[one];
    Ht1621_Tab[5] = Ht1621_Tube_2[one];
    Ht1621WrOneData(4, Ht1621_Tab[4]);          
    Ht1621WrOneData(5, Ht1621_Tab[5]); 
}

/**
* @Function ：显示CO2数值
* @brief    ：
* @input    ：-value 数值 
* @output   ：None
* @retval   ：None
**/
void Display_CO2_value(uint16_t value)
{
    uint8_t ths, hun, ten, one;

    ths = value / 1000 % 10;
    hun = value / 100 % 10;
    ten = value / 10 % 10;
    one = value % 10;


    Ht1621_Tab[8] |= 0x07;
    Ht1621_Tab[8] ^= Ht1621_Tube_1[ths];
    Ht1621_Tab[9] = Ht1621_Tube_2[ths];
    Ht1621WrOneData(8, Ht1621_Tab[8]);          
    Ht1621WrOneData(9, Ht1621_Tab[9]);  

    Ht1621_Tab[10] |= 0x07;
    Ht1621_Tab[10] ^= Ht1621_Tube_1[hun];
    Ht1621_Tab[11] = Ht1621_Tube_2[hun];
    Ht1621WrOneData(10, Ht1621_Tab[10]);          
    Ht1621WrOneData(11, Ht1621_Tab[11]);   

    Ht1621_Tab[12] |= 0x07;
    Ht1621_Tab[12] ^= Ht1621_Tube_1[ten];
    Ht1621_Tab[13] = Ht1621_Tube_2[ten];
    Ht1621WrOneData(12, Ht1621_Tab[12]);          
    Ht1621WrOneData(13, Ht1621_Tab[13]); 

    Ht1621_Tab[14] |= 0x07;
    Ht1621_Tab[14] ^= Ht1621_Tube_1[one];
    Ht1621_Tab[15] = Ht1621_Tube_2[one];
    Ht1621WrOneData(14, Ht1621_Tab[14]);          
    Ht1621WrOneData(15, Ht1621_Tab[15]); 

}







/****************************************************************************************************************/

/**
* @Function ：HT1621初始化函数
* @brief    ：执行HT1621初始化
* @input    ：None 
* @output   ：None
* @retval   ：None
**/
void Ht1621_Init(void)
{
  HT1621_CS=1;			//芯片片选脚拉高，对数据操作无效
  HT1621_WR=1;			//芯写数据选脚拉高
  HT1621_DAT=1;			//芯数据脚选脚拉高
  Delay_ms(500);        //延时使LCD工作电压稳定
  Ht1621WrCmd(BIAS);	//打开芯片COM引脚1/3duty偏压
  Ht1621WrCmd(RC256);   //使用系统时钟源/内部振荡器
  Ht1621WrCmd(SYSDIS);  //关振系统荡器和LCD偏压发生器 
  Ht1621WrCmd(WDTDIS);  //禁止看门狗
  Ht1621WrCmd(SYSEN);   //打开系统振荡器 
  Ht1621WrCmd(LCDON);   //打开LCD偏压发生器 
}

/**
* @Function ：HT1621数据写入函数
* @brief    ：
* @input    ：Data - 写入的数据
*           ：cnt - 写入数据的长度 
* @output   ：None
* @retval   ：None
**/
void Ht1621WrData(uint8_t Data, uint8_t cnt)
{
    uint8_t i;
    for (i = 0; i < cnt; i++) 
    {
        HT1621_WR = 0;
        Delay_10us();
        HT1621_DAT = Data&0x80;
        Delay_10us();
        HT1621_WR = 1;
        Delay_10us();
        Data <<= 1;
    }
}

/**
* @Function ：HT1621命令写入函数
* @brief    ：
* @input    ：Cmd - 写入的命令值
* @output   ：None
* @retval   ：None
**/
void Ht1621WrCmd(uint8_t Cmd) 
{
    HT1621_CS = 0;
    Delay_10us();
    Ht1621WrData(0x80,4);              // - -写入命令标志100
    Ht1621WrData(Cmd,8);               // - - 写入命令数据
    HT1621_CS = 1;
    Delay_10us();
}

/**
* @Function ：HT1621在指定地址写入数据函数 
* @brief    ：因为HT1621的数据位4位，所以实际写入数据为参数的后4位
* @input    ：Addr - 写入数据的地址
*           ：Data - 写入的数据 
* @output   ：None
* @retval   ：None
**/
void Ht1621WrOneData(uint8_t Addr, uint8_t Data)
{
    HT1621_CS = 0;
    Ht1621WrData(0xa0,3);          // - - 写入数据标志101
    Ht1621WrData(Addr<<2,6);       // - - 写入地址数据
    Ht1621WrData(Data<<4,4);       // - - 写入数据
    HT1621_CS = 1;
    Delay_10us();
}

/**
* @Function ：HT1621连续写入方式函数
* @brief    ：HT1621的数据位4位，此处每次数据为8位，写入数据总数按8位计算
* @input    ：Addr - 写入的初始地址
*           ：*p - 为连续写入数据指针 
*           ：cnt - 写入数据总数
* @output   ：None
* @retval   ：None
**/
void Ht1621WrAllData(uint8_t Addr, uint8_t *p, uint8_t cnt)
{
    uint8_t i;
    HT1621_CS = 0;
    Ht1621WrData(0xa0,3);              // - - 写入数据标志101
    Ht1621WrData(Addr<<2,6);           // - - 写入地址数据
    for (i = 0; i < cnt; i++)
    {
        Ht1621WrData(*p,8);            // - - 写入数据
        p++;
    }
    HT1621_CS = 1;
    Delay_10us();
}








