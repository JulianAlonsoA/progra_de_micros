#include "pwm3_manual.h"
#include <avr/io.h>
#include <avr/interrupt.h>

#define MANUAL_PWM_LED_DDR   DDRD
#define MANUAL_PWM_LED_PORT  PORTD
#define MANUAL_PWM_LED_PIN   PD5

volatile uint8_t pwm_counter = 0;// volatile (interrupciones)
volatile uint8_t pwm_duty = 0;

void ManualPWM_Init(void)
{
	MANUAL_PWM_LED_DDR |= (1 << MANUAL_PWM_LED_PIN);// led salida
	MANUAL_PWM_LED_PORT &= ~(1 << MANUAL_PWM_LED_PIN);

	TCCR0A = 0;//limpiar timer0
	TCCR0B = 0;

	TCCR0A |= (1 << WGM01);//modo CTC
	TCCR0B |= (1 << CS01) | (1 << CS00);//prescaler 64

	OCR0A = 24;// cada 25 ticks interrupcion

	TIMSK0 |= (1 << OCIE0A);

	sei();
}

void ManualPWM_SetDuty(uint8_t duty_percent) //limitar al 100%
{
	if (duty_percent > 100)
	{
		duty_percent = 100;
	}

	pwm_duty = duty_percent;//duty cycle que define el brillo
}

ISR(TIMER0_COMPA_vect)// automatico
{
	pwm_counter++;//contar de 0 a 99

	if (pwm_counter >= 100)//reiniciar
	{
		pwm_counter = 0;//empieza ciclo

		if (pwm_duty > 0)//si el duty cycle es mayor que 0 enciendo el led
		{
			MANUAL_PWM_LED_PORT |= (1 << MANUAL_PWM_LED_PIN);
		}
		else // si el duty cycle es 0 siempre esta apgada mi led
		{
			MANUAL_PWM_LED_PORT &= ~(1 << MANUAL_PWM_LED_PIN);
		}
	}

	if (pwm_counter >= pwm_duty)//cuando el contador alcanza el duty se apaga
	{
		MANUAL_PWM_LED_PORT &= ~(1 << MANUAL_PWM_LED_PIN);
	}
}