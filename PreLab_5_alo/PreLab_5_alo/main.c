/*
 * PreLab 5.c
 *
 * Created: 14/04/2026
 * Author: Julián Alonso
 * Description: ADC6 controla la posición de un servo con PWM
 */

/****************************************/
// Encabezado (Libraries)
#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include "PWM/PWM1.h"

/****************************************/
// Variables globales
volatile uint8_t valor_ADC = 0;   // Valor ADC en 8 bits

/****************************************/
// Prototipos de funciones
void initADC6(void);

/****************************************/
// Función principal
int main(void)
{
	cli();          // Deshabilita interrupciones
	initADC6();     // Inicializa ADC6
	
	// Inicializa PWM1A para servo
	initPWM1A(no_invertido, fastPWM_ICR1_top, 8);
	ICR1 = 39999;   // Periodo de 20 ms aprox.
	OCR1A = 3000;   // Posición inicial
	
	sei();          // Habilita interrupciones

	while (1)
	{
		// Escala ADC a rango útil del servo
		updateDutyCycle1A(800 + ((uint32_t)valor_ADC * 4200) / 255);
	}
}

/****************************************/
// Subrutinas sin interrupción
void initADC6(void)
{
	ADMUX = 0;
	// Referencia AVcc, ajuste a la izquierda, canal ADC6
	ADMUX |= (1 << REFS0) | (1 << ADLAR) | (1 << MUX1) | (1 << MUX2);

	ADCSRA = 0;
	// Habilita ADC, interrupción, prescaler y primera conversión
	ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADPS1) | (1 << ADPS0) | (1 << ADSC);
}

/****************************************/
// Rutina de interrupción
ISR(ADC_vect)
{
	valor_ADC = ADCH;      // Guarda valor ADC
	ADCSRA |= (1 << ADSC); // Inicia nueva conversión
}