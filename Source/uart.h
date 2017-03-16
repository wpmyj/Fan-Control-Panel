#ifndef __UART_H_
#define __UART_H_



#define UART_1_BAUD 9600    //串口1波特率
#define UART_2_BAUD 9600    //串口2波特率
#define UART_VIR_BAUD (FOSC / 9600 / 3)    //模拟串口波特率

#define RX_1_BUF_SIZE 32
#define RX_2_BUF_SIZE 32
#define RX_VIR_BUF_SIZE 32

void Uart_vir_Main(void);
void Uart_vir_Init(void);


void Uart_1_Init(void);
void Uart_1_SendByte(uint8_t dat);
void Uart_1_SendString(uint8_t *s);

void Uart_2_Init(void);
void Uart_2_SendByte(uint8_t dat);
void Uart_2_SendString(uint8_t *s);

#endif