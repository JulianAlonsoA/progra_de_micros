#define F_CPU 16000000UL

#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

#include "PWM/pwm1_servo.h"
#include "PWM/pwm2_servo.h"
#include "PWM/pwm3_manual.h"

// PROTOTIPOS DE FUNCIONES
static void ADC_Init(void);
static uint16_t ADC_Read(uint8_t channel);
static uint8_t ADC_Angle(uint16_t adc_value);
static uint8_t ADC_LED(uint16_t adc_value);

int main(void)
{
	uint16_t pot_servo_a; //variables valores de los potenciometros (0-1023)
	uint16_t pot_servo_b;
	uint16_t pot_led_pwm;

	ADC_Init();
	PWM1_Init();//servo 1
	PWM2_Init();//servo2
	ManualPWM_Init();//LED

	while (1)
	{
		pot_servo_a = ADC_Read(4);//Leer a4 y convertir valor a angulo
		Servo1_SetAngle(ADC_Angle(pot_servo_a));
		_delay_ms(5);//estabilidad cambio

		pot_servo_b = ADC_Read(5);//otro servo
		Servo2_SetAngle(ADC_Angle(pot_servo_b));
		_delay_ms(5);

		pot_led_pwm = ADC_Read(6);//Leer a6 y convertir a porcentaje para el brillo
		ManualPWM_SetDuty(ADC_LED(pot_led_pwm));
		_delay_ms(5);
	}
}

static void ADC_Init(void)
{
	ADMUX = 0;// limpiar registros que pin analogico lee
	ADCSRA = 0;

	ADMUX |= (1 << REFS0);// referencia seteada como 5V

	ADCSRA |= (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);// encender adc con prescaler de 128
}

static uint16_t ADC_Read(uint8_t channel)
{
	channel &= 0x07;//solo valores de 0 a 7 para el rango del ADC

	ADMUX = (ADMUX & 0xF0) | channel;// borrar canal anterior y poner el nuevo (a4,5 o 6)

	ADCSRA |= (1 << ADSC);

	while (ADCSRA & (1 << ADSC));

	return ADC;// leer y esperar para un valor entre 0 y 1023
}

static uint8_t ADC_Angle(uint16_t adc_value)
{
	return (uint8_t)(((uint32_t)adc_value * 180U) / 1023U);// valor del ADC a angulo de 0 a 180
}

static uint8_t ADC_LED(uint16_t adc_value)// valor del ADC a porcentaje del PWM
{
	return (uint8_t)(((uint32_t)adc_value * 100U) / 1023U);
}