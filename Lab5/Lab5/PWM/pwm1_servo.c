#include "pwm1_servo.h"
#include <avr/io.h>

//1 tick del timer son 0.5 micros

#define SERVO_MIN_US 500U
#define SERVO_MAX_US 2500U
#define SERVO_TOP    39999U

static uint16_t AngleToTicks(uint8_t angle);

void PWM1_Init(void)
{
	// D9 ? OC1A
	DDRB |= (1 << DDB1);// pin d9 salida servo

	TCCR1A = 0;
	TCCR1B = 0;//limpiar timer1

	// Fast PWM, TOP = ICR1
	TCCR1A |= (1 << COM1A1) | (1 << WGM11);// activar salida PWM en modo Fast con top en ICR1
	TCCR1B |= (1 << WGM13) | (1 << WGM12) | (1 << CS11); // prescaler 8

	ICR1 = SERVO_TOP;// con 16mhz y prescaler 8 40000 ticks son 20ms

	Servo1_SetAngle(90);
}

void Servo1_SetAngle(uint8_t angle)
{
	OCR1A = AngleToTicks(angle);// el valor de OCR1A define el ancho de pulso alto (posicion)
}

static uint16_t AngleToTicks(uint8_t angle)
{
	if (angle > 180) angle = 180;

	uint16_t pulse = SERVO_MIN_US +
	(((uint32_t)(SERVO_MAX_US - SERVO_MIN_US) * angle) / 180U);// convertir 0 a 500µs y 180 a 2500

	return pulse * 2;
}