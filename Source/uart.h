#ifndef __UART_H_
#define __UART_H_




#define UART_1_BAUD 9600    //串口1波特率
#define UART_2_BAUD 9600    //串口2波特率


void Uart_1_Init(void);
void Uart_1_SendByte(uint8_t dat);
void Uart_1_SendString(uint8_t *s);

void Uart_2_Init(void);
void Uart_2_SendByte(uint8_t dat);
void Uart_2_SendString(uint8_t *s);

#endif
