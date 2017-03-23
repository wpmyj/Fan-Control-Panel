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

bool Uart_1_TX_Busy;
bool Uart_2_TX_Busy;
bool respons_flg = OFF;
bool send_respons = OFF;

// uint8_t RX_1_buf[RX_1_BUF_SIZE];
// uint8_t RX_2_buf[RX_2_BUF_SIZE];
// uint8_t RX_vir_buf[RX_VIR_BUF_SIZE];

// uint8_t RX_1_cnt;
// uint8_t RX_2_cnt;
// uint8_t RX_vir_cnt;

extern bool Status_Switch;       	//开机/关机状态位，ON:开机，OFF：关机
extern bool Status_RunModel;       	//手动/自动状态位，ATUO/MANUAL
extern bool Status_WindModel;      //内循环/外循环状态位，EXTER/INTER
extern bool Status_FilteSetTime;     //滤网时间设置状态位，ON：进入，OFF：退出
extern bool Status_FilteIco;			//滤网显示/关闭状态位，
extern uint8_t Status_Filte;      //滤网当前状态位


extern uint8_t Wind_Level;      		//风量大小变量，范围：1-3

extern uint8_t Filte_RunTime_Hour;		//滤网运行时间，小时
extern uint8_t Filte_RunTime_Day;		//滤网运行时间，天
extern uint8_t Filte_RunTime_Mon;		//滤网运行时间，月

extern uint16_t PM25_Value;
extern uint16_t CO2_Value;
extern uint8_t Temp_Ex_Value; 
extern uint8_t Temp_In_Value; 


/**
* @Function ：模拟串口主函数
* @brief    ：接收控制板数据
* @input    ：None 
* @output   ：None
* @retval   ：None
**/
void Uart_vir_Main(void)
{
    static uint8_t Uart_vir_cnt = 0;
    uint8_t temp;

    if (REND)
    {
        REND = 0;
        temp = RBUF;
        //Temp_Value = temp;
        if(Uart_vir_cnt > 1)
        {
            if(Uart_vir_cnt == 3)
            {
                if(temp == 0xAB)    //回应包
                {
                    respons_flg = ON;
                    return;
                }
                else
                {
                    send_respons = ON;
                }
            }
            else if(Uart_vir_cnt == 4)
            {
                Status_Switch = temp;
            }
            else if(Uart_vir_cnt == 5)
            {
                Status_RunModel = temp;             
            }
            else if(Uart_vir_cnt == 6)
            {
                Status_WindModel = temp;             
            }
            else if(Uart_vir_cnt == 7)
            {
                Status_Filte = temp;             
            }  
            else if(Uart_vir_cnt == 8)
            {
                Wind_Level = temp;             
            }            
            else if(Uart_vir_cnt == 13)
            {
                Temp_Ex_Value = temp;             
            }  
            else if(Uart_vir_cnt == 14)
            {
                Temp_In_Value = temp;
                Uart_vir_cnt = 0;    
                return;         
            }        
            Uart_vir_cnt++;     
        }

        if(Uart_vir_cnt == 1)
        {
            if(temp == 0x00)
            {
                Uart_vir_cnt++;
            }
            else
            {
                Uart_vir_cnt = 0;
            }
        }

        if(temp == 0xAA && Uart_vir_cnt == 0)
        {
            Uart_vir_cnt++;
        }



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
    TL0 = UART_VIR_BAUD;
    TH0 = UART_VIR_BAUD >> 8; 
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
        RCNT = 3;                       //initial receive baudrate counter
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
    TMOD |= 0x01;                        //timer0 in 16-bit auto reload mode
    AUXR |= 0x80;                        //timer0 working at 1T mode
    TL0 = UART_VIR_BAUD;
    TH0 = UART_VIR_BAUD>>8;                      //initial timer0 and set reload value
    TR0 = 1;                            //tiemr0 start running
    ET0 = 1;                            //enable timer0 interrupt
    IPH |= 0x02;
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
	//AUXR |= 0x40;		//定时器1时钟为Fosc,即1T
	//AUXR &= 0xFE;		//串口1选择定时器1为波特率发生器
	TMOD |= 0x20;		//设定定时器1为8位自动重装方式
    TH1 = TL1 = -(FOSC/12/32/UART_1_BAUD); 
	//TL1 = 0xE0;		//设定定时初值  波特率9600，系统频率：11.0592MHz
	//TH1 = 0xFE;		//设定定时初值
	ET1 = 0;		    //禁止定时器1中断
	TR1 = 1;		    //启动定时器1
    ES = 1;             //启动串口1中断
}

/**
* @Function ：串口1中断函数
* @brief    ：接收CO2数据
* @input    ：None 
* @output   ：None
* @retval   ：None
**/
void Uart_1_Isr(void) interrupt 4 using 1
{
    static uint8_t Uart_1_cnt = 0, value;
    uint8_t temp;

    if (RI)
    {
        RI = 0;             //Clear receive interrupt flag
        temp = SBUF;
        if(Uart_1_cnt > 1)
        {
            if(Uart_1_cnt == 2)
            {
                value = temp;
            }
            else if(Uart_1_cnt == 3)
            {
                CO2_Value = value * 256 + temp;
                Uart_1_cnt = 0;      
                return;          
            }
            Uart_1_cnt++;
        }

        if(Uart_1_cnt == 1)
        {
            if(temp == 0x86)
            {
                Uart_1_cnt++;
            }
            else
            {
                Uart_1_cnt = 0;
            }
        }

        if(temp == 0xFF && Uart_1_cnt == 0)
        {
            Uart_1_cnt++;
        }

    }
    if (TI)
    {
        TI = 0;             //Clear transmit interrupt flag
        Uart_1_TX_Busy = 0;
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
    while(Uart_1_TX_Busy);
    Uart_1_TX_Busy = 1;
 	SBUF = dat;
 	// while(!TI);
    // TI = 0;
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

	//AUXR &= 0xF7;		//波特率不倍速
	S2CON = 0x50;		//8位数据,可变波特率
	//AUXR |= 0x14;		//独立波特率发生器时钟为Fosc,即1T
    BRT = -(FOSC/32/UART_2_BAUD);  
	//BRT = 0xDC;		//设定独立波特率发生器重装值 波特率9600，系统频率：11.0592MHz
	AUXR |= 0x14;		//启动独立波特率发生器

    IE2 = 0x01;            //启动串口2中断
}

/**
* @Function ：串口2中断函数
* @brief    ：接收PM2.5数据
* @input    ：None 
* @output   ：None
* @retval   ：None
**/
void Uart_2_Isr(void) interrupt 8 using 1
{
    static uint8_t Uart_2_cnt = 0, value;
    uint8_t temp;

    if (S2CON & S2RI)
    {
        S2CON &= ~S2RI;;
        temp = S2BUF;
        if(Uart_2_cnt > 1)
        {
            if(Uart_2_cnt == 6)
            {
                value = temp;
            }
            else if(Uart_2_cnt == 7)
            {
                PM25_Value = value * 256 + temp;
                Uart_2_cnt = 0;   
                return;             
            }
            Uart_2_cnt++;
        }

        if(Uart_2_cnt == 1)
        {
            if(temp == 0x3D)
            {
                Uart_2_cnt++;
            }
            else
            {
                Uart_2_cnt = 0;
            }
        }

        if(temp == 0x32 && Uart_2_cnt == 0)
        {
            Uart_2_cnt++;
        }

    }
    if (S2CON & S2TI)
    {
        S2CON &= ~S2TI;
        Uart_2_TX_Busy = 0;
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
    while(Uart_2_TX_Busy);
    Uart_2_TX_Busy = 1;
 	S2BUF = dat;
 	// while(!S2TI);
    // S2TI = 0;
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






