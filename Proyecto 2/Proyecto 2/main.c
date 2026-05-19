/*
 * main.c
 *
 * Proyecto 2 - Brazo robótico con MODOS
 * Modo Manual + EEPROM + Adafruit/UART
 */

/****************************************/
// Encabezado (Libraries)
#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

#include "PWM/pwm_servo.h"
#include "ADC/adc.h"
#include "EEPROM/eeprom_manager.h"
#include "UART/uart.h"

/****************************************/
// Pines
#define BTN_MODO        PD7   // D7
#define BTN_GUARDAR     PD2   // D2
#define BTN_REPRODUCIR  PD4   // D4
#define LED_STATUS      PB5   // D13

/****************************************/
// Modos
#define MODO_MANUAL     0
#define MODO_EEPROM     1
#define MODO_ADAFRUIT   2

/****************************************/
// Variables globales
uint8_t modo_actual = MODO_MANUAL;

uint8_t angulos_actuales[4] = {90, 90, 90, 90};

uint8_t indice_pose = 0;
uint8_t indice_repro = 0;

/****************************************/
// Function prototypes
void setup(void);

void modo_manual_adc(void);
void modo_adafruit_uart(void);

void guardar_pose_siguiente(void);
void reproducir_pose_siguiente(void);

void indicar_modo(void);

uint8_t boton_presionado(uint8_t pin);

void enviar_feedback(uint8_t servo, uint8_t angle);

/****************************************/
// Main Function
int main(void)
{
	setup();

	while (1)
	{
		// Cambiar modo
		if (boton_presionado(BTN_MODO))
		{
			modo_actual++;

			if (modo_actual > MODO_ADAFRUIT)
			{
				modo_actual = MODO_MANUAL;
			}

			indicar_modo();
		}

		switch (modo_actual)
		{
			/********************************/
			// MODO MANUAL
			case MODO_MANUAL:

				modo_manual_adc();

			break;

			/********************************/
			// MODO EEPROM
			case MODO_EEPROM:

				if (boton_presionado(BTN_GUARDAR))
				{
					guardar_pose_siguiente();
				}

				if (boton_presionado(BTN_REPRODUCIR))
				{
					reproducir_pose_siguiente();
				}

			break;

			/********************************/
			// MODO ADAFRUIT / UART
			case MODO_ADAFRUIT:

				modo_adafruit_uart();

			break;
		}

		if (modo_actual != MODO_ADAFRUIT)
		{
			_delay_ms(5);
		}
	}
}

/****************************************/
// NON-Interrupt subroutines

void setup(void)
{
	Servo_Init();
	ADC_Init();
	UART_Init();

	// Botones como entrada
	DDRD &= ~((1 << BTN_MODO) |
			  (1 << BTN_GUARDAR) |
			  (1 << BTN_REPRODUCIR));

	// Pull-up interno
	PORTD |= (1 << BTN_MODO) |
			 (1 << BTN_GUARDAR) |
			 (1 << BTN_REPRODUCIR);

	// LED D13 como salida
	DDRB |= (1 << LED_STATUS);

	PORTB &= ~(1 << LED_STATUS);

	// Posición inicial
	Servo_SetAngle(1, 90);
	Servo_SetAngle(2, 90);
	Servo_SetAngle(3, 90);
	Servo_SetAngle(4, 90);

	indicar_modo();
}

/****************************************/
// MODO MANUAL
void modo_manual_adc(void)
{
	static uint16_t adc_filtrado[4] = {512, 512, 512, 512};
	static uint8_t last_angle[4] = {90, 90, 90, 90};

	uint16_t adc_actual;
	uint8_t angle;

	for (uint8_t i = 0; i < 4; i++)
	{
		adc_actual = ADC_Read(i);

		// Filtro suave
		adc_filtrado[i] =
		((adc_filtrado[i] * 7) + adc_actual) / 8;

		angle = ADC_MapToAngle(adc_filtrado[i]);

		// Deadband
		if ((angle > last_angle[i] + 3) ||
			(angle + 3 < last_angle[i]))
		{
			Servo_SetAngle(i + 1, angle);

			last_angle[i] = angle;

			angulos_actuales[i] = angle;
		}
	}
}

/****************************************/
// MODO ADAFRUIT / UART
void modo_adafruit_uart(void)
{
	static char buffer[12];
	static uint8_t index = 0;

	char c;

	while (UART_Available())
	{
		c = UART_ReceiveChar();

		if (c == '\n' || c == '\r')
		{
			buffer[index] = '\0';

			if (index >= 4 && buffer[0] == 'S' && buffer[2] == ':')
			{
				uint8_t servo = buffer[1] - '0';
				uint16_t angle = 0;

				for (uint8_t i = 3; buffer[i] != '\0'; i++)
				{
					if (buffer[i] >= '0' && buffer[i] <= '9')
					{
						angle = (angle * 10) + (buffer[i] - '0');
					}
				}

				if (angle > 180)
				{
					angle = 180;
				}

				if (servo >= 1 && servo <= 4)
				{
					Servo_SetAngle(servo, (uint8_t)angle);
					angulos_actuales[servo - 1] = (uint8_t)angle;
					enviar_feedback(servo, (uint8_t)angle);
				}
			}

			index = 0;
		}
		else
		{
			if (index < sizeof(buffer) - 1)
			{
				buffer[index] = c;
				index++;
			}
			else
			{
				index = 0;
			}
		}
	}
}

/****************************************/
// EEPROM
void guardar_pose_siguiente(void)
{
	EEPROM_SavePose(
		indice_pose,
		angulos_actuales[0],
		angulos_actuales[1],
		angulos_actuales[2],
		angulos_actuales[3]
	);

	indice_pose++;

	if (indice_pose >= 4)
	{
		indice_pose = 0;
	}

	// Feedback LED
	PORTB |= (1 << LED_STATUS);
	_delay_ms(100);
	PORTB &= ~(1 << LED_STATUS);
}

void reproducir_pose_siguiente(void)
{
	uint8_t a1;
	uint8_t a2;
	uint8_t a3;
	uint8_t a4;

	EEPROM_ReadPose(
		indice_repro,
		&a1,
		&a2,
		&a3,
		&a4
	);

	Servo_SetAngle(1, a1);
	Servo_SetAngle(2, a2);
	Servo_SetAngle(3, a3);
	Servo_SetAngle(4, a4);

	angulos_actuales[0] = a1;
	angulos_actuales[1] = a2;
	angulos_actuales[2] = a3;
	angulos_actuales[3] = a4;

	indice_repro++;

	if (indice_repro >= 4)
	{
		indice_repro = 0;
	}
}

/****************************************/
// Feedback UART
void enviar_feedback(uint8_t servo, uint8_t angle)
{
	UART_SendChar('R');

	UART_SendNumber(servo);

	UART_SendChar(':');

	UART_SendNumber(angle);

	UART_SendText("\n");
}

/****************************************/
// Indicar modo con LED
void indicar_modo(void)
{
	uint8_t veces = modo_actual + 1;

	for (uint8_t i = 0; i < veces; i++)
	{
		PORTB |= (1 << LED_STATUS);

		_delay_ms(200);

		PORTB &= ~(1 << LED_STATUS);

		_delay_ms(200);
	}
}

/****************************************/
// Leer botón
uint8_t boton_presionado(uint8_t pin)
{
	if (!(PIND & (1 << pin)))
	{
		_delay_ms(20);

		if (!(PIND & (1 << pin)))
		{
			while (!(PIND & (1 << pin)))
			{
			}

			return 1;
		}
	}

	return 0;
}

/****************************************/
// Interrupt routines