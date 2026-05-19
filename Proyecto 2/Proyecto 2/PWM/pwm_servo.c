/*
 * pwm_servo.c
 *
 * Description:
 * Librería PWM para control de 4 servos usando Timer1.
 * Servo 1 -> D9  / PB1
 * Servo 2 -> D10 / PB2
 * Servo 3 -> D11 / PB3
 * Servo 4 -> D5  / PD5
 */

/****************************************/
// Encabezado (Libraries)
#define F_CPU 16000000UL

#include "pwm_servo.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

/****************************************/
// Definiciones
#define SERVO_TICK_US      50U
#define SERVO_FRAME_US     20000U
#define SERVO_FRAME_TICKS  (SERVO_FRAME_US / SERVO_TICK_US)

/****************************************/
// Calibración individual de servos
const uint16_t servo_min_us[4] =
{
	600,   // Servo 1 - D9 sube y baja el brazo
	600,   // Servo 2 - D10 extensor del brazo
	1900,   // Servo 3 - D11 pinza
	600    // Servo 4 - D5 base giratoria
};

const uint16_t servo_max_us[4] =
{
	1600,  // Servo 1 - D9
	2000,  // Servo 2 - D10
	2400,  // Servo 3 - D11
	2400   // Servo 4 - D5
};

/****************************************/
// Variables globales
volatile uint16_t servo_ticks[4] =
{
	30,
	30,
	30,
	30
};

volatile uint16_t timer1_counter = 0;

/****************************************/
// Inicializar servos
void Servo_Init(void)
{
	// Servo 1: D9  = PB1
	// Servo 2: D10 = PB2
	// Servo 3: D11 = PB3
	DDRB |= (1 << DDB1);
	DDRB |= (1 << DDB2);
	DDRB |= (1 << DDB3);

	// Servo 4: D5 = PD5
	DDRD |= (1 << DDD5);

	// Todos LOW inicialmente
	PORTB &= ~(1 << PORTB1);
	PORTB &= ~(1 << PORTB2);
	PORTB &= ~(1 << PORTB3);
	PORTD &= ~(1 << PORTD5);

	/****************************************/
	// Timer1 CTC
	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1 = 0;

	// Modo CTC
	TCCR1B |= (1 << WGM12);

	// Prescaler 8
	TCCR1B |= (1 << CS11);

	// 50 us:
	// F_CPU = 16 MHz
	// Timer tick = 0.5 us
	// 50 us = 100 ticks
	OCR1A = 99;

	// Habilitar interrupción Compare A
	TIMSK1 |= (1 << OCIE1A);

	sei();
}

/****************************************/
// Set angle
void Servo_SetAngle(uint8_t servo, uint8_t angle)
{
	uint16_t pulse_us;
	uint16_t ticks;
	uint8_t index;

	if (servo < 1 || servo > 4)
	{
		return;
	}

	if (angle > 180)
	{
		angle = 180;
	}

	index = servo - 1;

	// Conversión de ángulo a pulso calibrado
	pulse_us =
	servo_min_us[index] +
	(((uint32_t)angle *
	(servo_max_us[index] - servo_min_us[index])) / 180);

	ticks = pulse_us / SERVO_TICK_US;

	uint8_t sreg = SREG;
	cli();

	servo_ticks[index] = ticks;

	SREG = sreg;
}

/****************************************/
// Set pulse
void Servo_SetPulse(uint8_t servo, uint16_t pulse_us)
{
	uint16_t ticks;
	uint8_t index;

	if (servo < 1 || servo > 4)
	{
		return;
	}

	index = servo - 1;

	if (pulse_us < servo_min_us[index])
	{
		pulse_us = servo_min_us[index];
	}

	if (pulse_us > servo_max_us[index])
	{
		pulse_us = servo_max_us[index];
	}

	ticks = pulse_us / SERVO_TICK_US;

	uint8_t sreg = SREG;
	cli();

	servo_ticks[index] = ticks;

	SREG = sreg;
}

/****************************************/
// Timer1 interrupt
ISR(TIMER1_COMPA_vect)
{
	// Inicio del frame de 20 ms
	if (timer1_counter == 0)
	{
		PORTB |= (1 << PORTB1); // Servo 1 - D9
		PORTB |= (1 << PORTB2); // Servo 2 - D10
		PORTB |= (1 << PORTB3); // Servo 3 - D11
		PORTD |= (1 << PORTD5); // Servo 4 - D5
	}

	// Servo 1
	if (timer1_counter >= servo_ticks[0])
	{
		PORTB &= ~(1 << PORTB1);
	}

	// Servo 2
	if (timer1_counter >= servo_ticks[1])
	{
		PORTB &= ~(1 << PORTB2);
	}

	// Servo 3
	if (timer1_counter >= servo_ticks[2])
	{
		PORTB &= ~(1 << PORTB3);
	}

	// Servo 4
	if (timer1_counter >= servo_ticks[3])
	{
		PORTD &= ~(1 << PORTD5);
	}

	timer1_counter++;

	// Reiniciar frame cada 20 ms
	if (timer1_counter >= SERVO_FRAME_TICKS)
	{
		timer1_counter = 0;
	}
}