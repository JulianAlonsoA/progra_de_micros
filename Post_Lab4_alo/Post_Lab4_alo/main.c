/*
 * main.c
 *
 * Created: 4/6/26
 * Author: Julian Alonso
 * Description: Contador de 8 bits + ADC + alarma
 */

/**************/
// Encabezado (Libraries)
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <avr/interrupt.h>

/**************/
// Function prototypes
void setup(void);
void adc_init(void);

void mostrar_segmentos(uint8_t valor_hex);
void apagar_displays(void);
void multiplexar_displays(uint8_t valor);

uint8_t boton_presionado_flanco(uint8_t pin, uint8_t *estado_anterior);

/**************/
// Variable global para guardar el ADC
volatile uint8_t adc_val = 0;

/**************/
// Tabla de 7 segmentos
const uint8_t tabla_7seg[16] = {
	0x3F, // 0
	0x06, // 1
	0x5B, // 2
	0x4F, // 3
	0x66, // 4
	0x6D, // 5
	0x7D, // 6
	0x07, // 7
	0x7F, // 8
	0x6F, // 9
	0x77, // A
	0x7C, // b
	0x39, // C
	0x5E, // d
	0x79, // E
	0x71  // F
};

/**************/
// Main Function
int main(void)
{
	uint8_t contador = 0;

	// Como hay pull-up, sin presionar = 1, presionado = 0
	uint8_t estado_anterior_pb0 = 1;
	uint8_t estado_anterior_pb1 = 1;

	setup();
	adc_init();

	sei(); // Habilita interrupciones globales
	ADCSRA |= (1 << ADSC); // Empieza la primera conversion ADC

	PORTD = contador;

	while (1)
	{
		// -------------------------
		// Lectura NO bloqueante de botones
		// -------------------------
		if (boton_presionado_flanco(PB0, &estado_anterior_pb0))
		{
			contador++;
			PORTD = contador;
		}

		if (boton_presionado_flanco(PB1, &estado_anterior_pb1))
		{
			contador--;
			PORTD = contador;
		}

		// -------------------------
		// Comparacion para alarma
		// Si ADC >= contador, enciende LED
		// -------------------------
		if (adc_val >= contador)
		{
			PORTC |= (1 << PC0);
		}
		else
		{
			PORTC &= ~(1 << PC0);
		}

		// -------------------------
		// Mostrar el valor del ADC en displays
		// -------------------------
		multiplexar_displays(adc_val);
	}
}

/**************/
// CONFIGURACIÓN INICIAL
void setup(void)
{
	// Desactivar USART
	UCSR0B = 0x00;

	// PORTD como salidas (8 leds del contador)
	DDRD = 0xFF;
	PORTD = 0x00;

	// PB0 y PB1 como entradas con pull-up
	DDRB &= ~((1 << PB0) | (1 << PB1));
	PORTB |= (1 << PB0) | (1 << PB1);

	// PB2 - PB5 como salidas (segmentos)
	DDRB |= (1 << PB2) | (1 << PB3) | (1 << PB4) | (1 << PB5);
	PORTB &= ~((1 << PB2) | (1 << PB3) | (1 << PB4) | (1 << PB5));

	// PC0 como salida para alarma
	DDRC |= (1 << PC0);
	PORTC &= ~(1 << PC0);

	// PC1 - PC5 como salidas
	DDRC |= (1 << PC1) | (1 << PC2) | (1 << PC3) | (1 << PC4) | (1 << PC5);
	PORTC &= ~((1 << PC1) | (1 << PC2) | (1 << PC3) | (1 << PC4) | (1 << PC5));

	// ADC6 es entrada analógica solamente

	apagar_displays();
}

/**************/
// Detecta una sola pulsación por flanco, sin bloquear
uint8_t boton_presionado_flanco(uint8_t pin, uint8_t *estado_anterior)
{
	uint8_t estado_actual;

	// con pull-up:
	// 1 = suelto
	// 0 = presionado
	estado_actual = (PINB & (1 << pin)) ? 1 : 0;

	// Detectar transición de 1 -> 0
	if ((*estado_anterior == 1) && (estado_actual == 0))
	{
		_delay_ms(15); // debounce corto

		estado_actual = (PINB & (1 << pin)) ? 1 : 0;

		if (estado_actual == 0)
		{
			*estado_anterior = 0;
			return 1;
		}
	}

	*estado_anterior = estado_actual;
	return 0;
}

/**************/
// Convertidor ADC con interrupciones
void adc_init(void)
{
	ADMUX = 0;
	ADMUX |= (1 << REFS0);   // referencia AVcc
	ADMUX |= (1 << ADLAR);   // left adjust para leer ADCH

	// Selecciona ADC6
	ADMUX |= (1 << MUX1) | (1 << MUX2);

	ADCSRA = 0;
	ADCSRA |= (1 << ADEN);   // habilita ADC
	ADCSRA |= (1 << ADIE);   // interrupción ADC
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // prescaler 128
}

/**************/
// ISR del ADC
ISR(ADC_vect)
{
	adc_val = ADCH; // Guarda los 8 bits más significativos
	ADCSRA |= (1 << ADSC); // Empieza otra conversión
}

/**************/
void apagar_displays(void)
{
	PORTC |= (1 << PC4) | (1 << PC5);
}

/**************/
// Mostrar segmentos
void mostrar_segmentos(uint8_t valor_hex)
{
	uint8_t patron = tabla_7seg[valor_hex];

	// Apagar segmentos primero
	PORTB &= ~((1 << PB2) | (1 << PB3) | (1 << PB4) | (1 << PB5));
	PORTC &= ~((1 << PC1) | (1 << PC2) | (1 << PC3));

	if (patron & (1 << 0)) PORTB |= (1 << PB2);
	if (patron & (1 << 1)) PORTB |= (1 << PB3);
	if (patron & (1 << 2)) PORTB |= (1 << PB4);
	if (patron & (1 << 3)) PORTB |= (1 << PB5);
	if (patron & (1 << 4)) PORTC |= (1 << PC1);
	if (patron & (1 << 5)) PORTC |= (1 << PC2);
	if (patron & (1 << 6)) PORTC |= (1 << PC3);
}

/**************/
// Multiplexado
void multiplexar_displays(uint8_t valor)
{
	uint8_t alto = (valor >> 4) & 0x0F;
	uint8_t bajo = valor & 0x0F;

	// Display izquierdo
	apagar_displays();
	mostrar_segmentos(alto);
	PORTC &= ~(1 << PC4);
	_delay_ms(2);

	// Display derecho
	apagar_displays();
	mostrar_segmentos(bajo);
	PORTC &= ~(1 << PC5);
	_delay_ms(2);

	apagar_displays();
}