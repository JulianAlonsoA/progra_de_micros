/*
 * main.c
 *
 * Created: 4/21/2026
 * Author: Julian Alonso
 * Description:
 * Prelab Laboratorio 6 - UART
 * Parte 1: Enviar un caracter desde el microcontrolador hacia la terminal.
 * Parte 2: Recibir un caracter desde la terminal y mostrarlo en el puerto B.
 */

/****************************************/
// Encabezado (Libraries y constantes)
#define F_CPU 16000000UL
#define BAUD 9600
#define UBRR_VALUE ((F_CPU / 16 / BAUD) - 1)

#include <avr/io.h>

/****************************************/
// Prototipos de funciones
void UART_init(void);
void UART_tx(char data);
char UART_rx(void);
void delay_simple(void);

/****************************************/
// Función para inicializar UART
void UART_init(void)
{
	// Cargar baud rate en los registros UBRR0H y UBRR0L
	UBRR0H = (uint8_t)(UBRR_VALUE >> 8);
	UBRR0L = (uint8_t)(UBRR_VALUE);

	// Habilitar transmisión (TX) y recepción (RX)
	UCSR0B = (1 << TXEN0) | (1 << RXEN0);

	// Configurar formato: 8 bits de datos, 1 bit de stop, sin paridad
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

/****************************************/
// Función para transmitir un caracter por UART
void UART_tx(char data)
{
	// Espera hasta que el buffer de transmisión esté vacío
	while (!(UCSR0A & (1 << UDRE0)));

	// Carga el dato a enviar en el registro UDR0
	UDR0 = data;
}

/****************************************/
// Función para recibir un caracter por UART
char UART_rx(void)
{
	// Espera hasta que llegue un dato recibido
	while (!(UCSR0A & (1 << RXC0)));

	// Retorna el dato recibido
	return UDR0;
}

/****************************************/
// Delay simple por software
void delay_simple(void)
{
	for (volatile long i = 0; i < 500000; i++);
}

/****************************************/
// Función principal
int main(void)
{
	// Inicializa UART
	UART_init();

	// Configura todo PORTB como salida para mostrar el dato recibido
	DDRB = 0xFF;
	PORTB = 0x00;

	/****************************************/
	// Loop infinito
	while (1)
	{
		// Parte 1:
		// Envía el caracter 'A' hacia la terminal
		UART_tx('A');

		// Pequeño retardo para no saturar la terminal
		delay_simple();

		// Parte 2:
		// Espera un caracter recibido desde la terminal
		// y lo muestra directamente en el puerto B
		PORTB = UART_rx();
	}
}