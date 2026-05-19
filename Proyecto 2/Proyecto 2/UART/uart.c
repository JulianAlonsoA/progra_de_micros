/*
 * uart.c
 *
 * Description:
 * Librería UART para comunicación con Python.
 */

/****************************************/
// Encabezado
#define F_CPU 16000000UL
#define BAUD 9600
#define UBRR_VALUE ((F_CPU / 16 / BAUD) - 1)

#include "uart.h"
#include <avr/io.h>
#include <stdint.h>

/****************************************/
// Inicializar UART
void UART_Init(void)
{
	UBRR0H = (uint8_t)(UBRR_VALUE >> 8);
	UBRR0L = (uint8_t)(UBRR_VALUE);

	UCSR0B = (1 << RXEN0) | (1 << TXEN0);

	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

/****************************************/
// Verificar si hay dato disponible
uint8_t UART_Available(void)
{
	return (UCSR0A & (1 << RXC0));
}

/****************************************/
// Enviar caracter
void UART_SendChar(char data)
{
	while (!(UCSR0A & (1 << UDRE0)))
	{
	}

	UDR0 = data;
}

/****************************************/
// Recibir caracter
char UART_ReceiveChar(void)
{
	while (!(UCSR0A & (1 << RXC0)))
	{
	}

	return UDR0;
}

/****************************************/
// Enviar texto
void UART_SendText(char *text)
{
	while (*text)
	{
		UART_SendChar(*text);
		text++;
	}
}

/****************************************/
// Enviar número 0-255
void UART_SendNumber(uint8_t number)
{
	char buffer[4];
	uint8_t i = 0;

	if (number == 0)
	{
		UART_SendChar('0');
		return;
	}

	while (number > 0)
	{
		buffer[i] = (number % 10) + '0';
		number /= 10;
		i++;
	}

	while (i > 0)
	{
		i--;
		UART_SendChar(buffer[i]);
	}
}