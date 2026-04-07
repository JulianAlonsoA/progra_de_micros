/*
 * main.c
 *
 * Created: 4/6/26
 * Author: Julian Alonso
 * Description: Contador de 8 bits
 */

/****************************************/
// Encabezado (Libraries)
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

/****************************************/
// Function prototypes
void setup(void);
uint8_t leer_boton(uint8_t pin);

/****************************************/
// Main Function
int main(void)
{
    uint8_t counter = 0;

    setup();
    PORTD = counter;

    while (1)
    {
        // -------------------------
        // incremento
        // -------------------------
        if (leer_boton(PB0))
        {
            counter++;
            PORTD = counter;

            // Esperar a que suelte el botón
            while (!(PINB & (1 << PB0)));
            _delay_ms(20);
        }

        // -------------------------
        // decremento
        // -------------------------
        if (leer_boton(PB1))
        {
            counter--;
            PORTD = counter;

            // Esperar a que suelte el botón
            while (!(PINB & (1 << PB1)));
            _delay_ms(20);
        }
    }
}

/****************************************/
// CONFIGURACIÓN INICIAL
void setup(void)
{
    // Desactivar USART 
    UCSR0B = 0x00;

    // PORTD como salidas
    DDRD = 0xFF;
    PORTD = 0x00;

    // PB0 y PB1 como entradas
    DDRB &= ~((1 << PB0) | (1 << PB1));
    PORTB |= (1 << PB0) | (1 << PB1);
}

/****************************************/
// Lee botones
uint8_t leer_boton(uint8_t pin)
{
    // Detectar si el botón fue presionado
    if (!(PINB & (1 << pin)))
    {
        _delay_ms(20);

        // Confirmar que sigue presionado
        if (!(PINB & (1 << pin)))
        {
            return 1;
        }
    }
    return 0;
}

/****************************************/
// Interrupt routines