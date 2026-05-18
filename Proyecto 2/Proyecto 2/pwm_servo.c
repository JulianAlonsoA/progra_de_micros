/*
 * pwm_servo.c
 *
 * Description:
 * Librería para controlar 4 servos usando Timer1 como base de tiempo.
 * Cada servo recibe una señal PWM independiente de 20 ms.
 */

/****************************************/
// Encabezado
#define F_CPU 16000000UL

#include "pwm_servo.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

/****************************************/
// Constantes PWM para servos
#define SERVO_MIN_US      1000U
#define SERVO_MAX_US      2000U
#define SERVO_FRAME_US    20000U
#define SERVO_TICK_US     10U

#define SERVO_MIN_TICKS   (SERVO_MIN_US / SERVO_TICK_US)
#define SERVO_MAX_TICKS   (SERVO_MAX_US / SERVO_TICK_US)
#define SERVO_FRAME_TICKS (SERVO_FRAME_US / SERVO_TICK_US)

/****************************************/
// Variables globales
volatile uint16_t servo_ticks[4] = {
	SERVO_MIN_TICKS,
	SERVO_MIN_TICKS,
	SERVO_MIN_TICKS,
	SERVO_MIN_TICKS
};

volatile uint16_t servo_counter = 0;

/****************************************/
// Inicialización de servos
void Servo_Init(void)
{
	// Servo 1: D9 = PB1
	// Servo 2: D10 = PB2
	// Servo 4: D11 = PB3
	DDRB |= (1 << DDB1) | (1 << DDB2) | (1 << DDB3);

	// Servo 3: D3 = PD3
	DDRD |= (1 << DDD3);

	// Iniciar servos en bajo
	PORTB &= ~((1 << PORTB1) | (1 << PORTB2) | (1 << PORTB3));
	PORTD &= ~(1 << PORTD3);

	// Configurar Timer1 en modo CTC
	TCCR1A = 0;
	TCCR1B = 0;

	// 20 ticks = 10 us
	OCR1A = 19;

	TCCR1B |= (1 << WGM12);  // Modo CTC
	TCCR1B |= (1 << CS11);   // Prescaler 8

	// Habilitar interrupción por comparación A
	TIMSK1 |= (1 << OCIE1A);

	sei();
}

/****************************************/
// Cambiar ángulo de un servo
void Servo_SetAngle(uint8_t servo, uint8_t angle)
{
	uint16_t pulse_ticks;

	if (angle > 180)
	{
		angle = 180;
	}

	// Convertir ángulo 0-180 a pulso 1000-2000 us
	pulse_ticks = SERVO_MIN_TICKS + (((uint32_t)angle * (SERVO_MAX_TICKS - SERVO_MIN_TICKS)) / 180);

	if (servo >= 1 && servo <= 4)
	{
		uint8_t sreg = SREG;
		cli();

		servo_ticks[servo - 1] = pulse_ticks;

		SREG = sreg;
	}
}

/****************************************/
// Cambiar pulso manualmente en microsegundos
void Servo_SetPulse(uint8_t servo, uint16_t pulse_us)
{
	uint16_t pulse_ticks;

	if (pulse_us < SERVO_MIN_US)
	{
		pulse_us = SERVO_MIN_US;
	}

	if (pulse_us > SERVO_MAX_US)
	{
		pulse_us = SERVO_MAX_US;
	}

	pulse_ticks = pulse_us / SERVO_TICK_US;

	if (servo >= 1 && servo <= 4)
	{
		uint8_t sreg = SREG;
		cli();

		servo_ticks[servo - 1] = pulse_ticks;

		SREG = sreg;
	}
}

/****************************************/
// Interrupción Timer1
ISR(TIMER1_COMPA_vect)
{
	// Inicio del frame de 20 ms
	if (servo_counter == 0)
	{
		PORTB |= (1 << PORTB1); // Servo 1
		PORTB |= (1 << PORTB2); // Servo 2
		PORTD |= (1 << PORTD3); // Servo 3
		PORTB |= (1 << PORTB3); // Servo 4
	}

	// Apagar cada servo cuando alcanza su pulso
	if (servo_counter >= servo_ticks[0])
	{
		PORTB &= ~(1 << PORTB1);
	}

	if (servo_counter >= servo_ticks[1])
	{
		PORTB &= ~(1 << PORTB2);
	}

	if (servo_counter >= servo_ticks[2])
	{
		PORTD &= ~(1 << PORTD3);
	}

	if (servo_counter >= servo_ticks[3])
	{
		PORTB &= ~(1 << PORTB3);
	}

	servo_counter++;

	// Reiniciar frame cada 20 ms
	if (servo_counter >= SERVO_FRAME_TICKS)
	{
		servo_counter = 0;
	}
}