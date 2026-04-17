#include "pwm2_servo.h"
#include <avr/io.h>

// 1 tick son 64 micros

#define SERVO_MIN_COUNT 8U
#define SERVO_MAX_COUNT 31U

static uint8_t AngleToCounts(uint8_t angle);

void PWM2_Init(void)
{
	// D3
	DDRD |= (1 << DDD3);

	TCCR2A = 0;
	TCCR2B = 0;

	// Fast PWM
	TCCR2A |= (1 << COM2B1) | (1 << WGM21) | (1 << WGM20);

	// Prescaler 1024
	TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20);

	Servo2_SetAngle(90);//centrado
}

void Servo2_SetAngle(uint8_t angle)
{
	OCR2B = AngleToCounts(angle);//ajustar pulso
}

static uint8_t AngleToCounts(uint8_t angle)
{
	if (angle > 180) angle = 180;

	return SERVO_MIN_COUNT +
	(((uint32_t)(SERVO_MAX_COUNT - SERVO_MIN_COUNT) * angle) / 180U);//valor minimo y maximo
}