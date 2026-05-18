/*
 * adc.c
 *
 * Description:
 * Librería para leer 4 potenciómetros usando ADC.
 */

/****************************************/
// Encabezado
#define F_CPU 16000000UL

#include "adc.h"
#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

/****************************************/
// Inicialización ADC
void ADC_Init(void)
{
	// Referencia AVcc = 5V
	ADMUX = (1 << REFS0);

	// Desactivar entradas digitales en A0-A3 para reducir ruido
	DIDR0 = (1 << ADC0D) |
	(1 << ADC1D) |
	(1 << ADC2D) |
	(1 << ADC3D);

	// Habilitar ADC, prescaler 128
	ADCSRA = (1 << ADEN) |
	(1 << ADPS2) |
	(1 << ADPS1) |
	(1 << ADPS0);
}

/****************************************/
// Leer canal ADC
uint16_t ADC_Read(uint8_t channel)
{
	channel &= 0x07;

	// Seleccionar canal manteniendo AVcc
	ADMUX = (1 << REFS0) | channel;

	// Pequeña espera para que el mux se estabilice
	_delay_us(10);

	// Primera lectura dummy
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC))
	{
	}

	// Segunda lectura real
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC))
	{
	}

	return ADC;
}

/****************************************/
// Convertir ADC 0-1023 a ángulo 0-180
uint8_t ADC_MapToAngle(uint16_t adc_value)
{
	uint8_t angle;

	if (adc_value > 1023)
	{
		adc_value = 1023;
	}

	angle = (uint8_t)(((uint32_t)adc_value * 180) / 1023);

	return angle;
}