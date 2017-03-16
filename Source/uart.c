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


/******************************模拟串口标志位/变量*******************************/

sbit RXB = P1^0;                        //define UART TX/RX port
sbit TXB = P1^1;

uint8_t TBUF,RBUF;                      //模拟串口发送/接收寄存器
uint8_t TDAT,RDAT;                      //模拟串口发送/接收数据变量
uint8_t TCNT,RCNT;                      //发送和接收检测 计数器(3倍速率检测)
uint8_t TBIT,RBIT;                      //发送和接收的数据计数器   
bit TING,RING;
bit TEND,REND;

uint8_t t, r;


uint8_t RX_1_buf[RX_1_BUF_SIZE];
uint8_t RX_2_buf[RX_2_BUF_SIZE];
uint8_t RX_vir_buf[RX_VIR_BUF_SIZE];

uint8_t RX_1_cnt;
uint8_t RX_2_cnt;
uint8_t RX_vir_cnt;



/**
* @Function ：模拟串口主函数
* @brief    ：接送/发送数据
* @input    ：None 
* @output   ：None
* @retval   ：None
**/
void Uart_vir_Main(void)
{
    if (REND)
    {
        REND = 0;
        //Uart_vir_RX_data_buf[r++ & 0x0f] = RBUF;
    }
    // if (TEND)
    // {
    //     if (t != r)
    //     {
    //         TEND = 0;
    //         TBUF = buf[t++ & 0x0f];
    //         TING = 1;
    //     }
    // }
}


/**
* @Function ：定时器0中断函数
* @brief    ：实现模拟串口收发数据过程
* @input    ：None 
* @output   ：None
* @retval   ：None
**/
void tm0(void) interrupt 1 using 1
{
    if (RING)
    {
        if (--RCNT == 0)
        {
            RCNT = 3;                   //reset send baudrate counter
            if (--RBIT == 0)
            {
                RBUF = RDAT;            //save the data to RBUF
                RING = 0;               //stop receive
                REND = 1;               //set receive completed flag
            }
            else
            {
                RDAT >>= 1;
                if (RXB) RDAT |= 0x80;  //shift RX data to RX buffer
            }
        }
    }
    else if (!RXB)
    {
        RING = 1;                       //set start receive flag
        RCNT = 4;                       //initial receive baudrate counter
        RBIT = 9;                       //initial receive bit number (8 data bits + 1 stop bit)
    }

    if (--TCNT == 0)
    {
        TCNT = 3;                       //reset send baudrate counter
        if (TING)                       //judge whether sending
        {
            if (TBIT == 0)
            {
                TXB = 0;                //send start bit
                TDAT = TBUF;            //load data from TBUF to TDAT
                TBIT = 9;               //initial send bit number (8 data bits + 1 stop bit)
            }
            else
            {
                TDAT >>= 1;             //shift data to CY
                if (--TBIT == 0)
                {
                    TXB = 1;
                    TING = 0;           //stop send
                    TEND = 1;           //set send completed flag
                }
                else
                {
                    TXB = CY;           //write CY to TX port
                }
            }
        }
    }
}


/**
* @Function ：模拟串口初始函数
* @brief    ：占用定时器0，波特率9600，系统频率：11.0592MHz
* @input    ：None 
* @output   ：None
* @retval   ：None
**/
void Uart_vir_Init(void)
{
    TMOD = 0x00;                        //timer0 in 16-bit auto reload mode
    AUXR = 0x80;                        //timer0 working at 1T mode
    TL0 = UART_VIR_BAUD;
    TH0 = UART_VIR_BAUD>>8;                      //initial timer0 and set reload value
    TR0 = 1;                            //tiemr0 start running
    ET0 = 1;                            //enable timer0 interrupt
    PT0 = 1;                            //improve timer0 interrupt priority
    //EA = 1;                             //open global interrupt switch

    TING = 0;
    RING = 0;
    TEND = 1;
    REND = 0;
    TCNT = 0;
    RCNT = 0;
}




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
* @Function ：串口1中断函数
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
* @Function ：串口2中断函数
* @brief    ：
* @input    ：None 
* @output   ：None
* @retval   ：None
**/
void Uart_2_Isr(void) interrupt 8 using 1
{
    if (S2RI)
    {
        S2RI = 0;
        RX_2_buf[RX_2_cnt] = S2BUF;
        RX_2_cnt++;
        if (RX_2_cnt > RX_2_BUF_SIZE) 
        {
            RX_2_cnt = 0;
        }

    }
    if (S2TI)
    {
        S2TI = 0;

    }
}

/**
* @Function ：读取串口2接收缓冲区数据
* @brief    ：
* @input    ： 
* @output   ：-*dat：存放数据地址
* @retval   ：返回flase：缓冲区未满；返回true：成功读取缓冲区数据
**/
bool Uart_2_ReadData(uint8_t *dat)
{
    uint8_t i; 

    if(RX_2_cnt < RX_2_BUF_SIZE)
    {
        return FLASE;
    }

    for (i = 0; i < RX_2_BUF_SIZE; i++)
    {
        *dat = RX_2_buf[i];
        dat++;
    }

    RX_2_cnt = 0;           //清零接收计数器   

    return TRUE;
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






