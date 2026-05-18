#ifndef UART_H_
#define UART_H_

#include <stdint.h>

void UART_Init(void);
void UART_SendChar(char data);
void UART_SendText(char *text);
void UART_SendNumber(uint8_t number);
char UART_ReceiveChar(void);
uint8_t UART_Available(void);

#endif