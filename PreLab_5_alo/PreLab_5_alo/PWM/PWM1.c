/*
 * PWM1.c
 *
 * Created: 4/14/2026 12:07:56 PM
 *  Author: Julian Alonso
 */ 
#include "PWM1.h"

void initPWM1A(uint8_t invert, uint8_t modo, uint16_t prescaler)
{

	DDRB |= (1<<DDB1);
	
	TCCR1A = 0;
	TCCR1B = 0;
	
	if (invert)
	{
		TCCR1A |= (1<<COM1A1)|(1<<COM1A0); 
	}
	else
	{
		TCCR1A |= (1<<COM1A1); 
	}

	switch(modo)
	{
		case 0:
		TCCR1A |= (1<<WGM10); 
		break;
		case 1:
		TCCR1A |= (1<<WGM11); 
		break;
		case 2:
		TCCR1A |= (1<<WGM11)|(1<<WGM10); 
		break;
		case 3:
		TCCR1A |= (1<<WGM10); 
		TCCR1B |= (1<<WGM12);
		break;
		case 4:
		TCCR1A |= (1<<WGM11); 
		TCCR1B |= (1<<WGM12);
		break;
		case 5:
		TCCR1A |= (1<<WGM11)|(1<<WGM10); 
		TCCR1B |= (1<<WGM12);
		break;
		case 6:
		TCCR1B |= (1<<WGM13);
		break;
		case 7:
		TCCR1A |= (1<<WGM10);
		TCCR1B |= (1<<WGM13);
		break;
		case 8:
		TCCR1A |= (1<<WGM11)|(1<<WGM10);
		TCCR1B |= (1<<WGM13);
		break;
		case 9:
		TCCR1A |= (1<<WGM11)|(1<<WGM10);
		TCCR1B |= (1<<WGM12)|(1<<WGM13);
		break;
		case 10:
		TCCR1A |= (1<<WGM11);
		TCCR1B |= (1<<WGM12)|(1<<WGM13);
		break;
		case 11:
		TCCR1A |= (1<<WGM11)|(1<<WGM10);
		TCCR1B |= (1<<WGM12)|(1<<WGM13);
		break;
		default:
		TCCR1A |= (1<<WGM10); 
		break;
	}
	switch(prescaler)
	{
		case 1:
		TCCR1B |= (1<<CS10);
		break;
		case 8:
		TCCR1B |= (1<<CS11);
		break;
		case 64:
		TCCR1B |= (1<<CS11)|(1<<CS10);
		break;
		case 256:
		TCCR1B |= (1<<CS12);
		break;
		case 1024:
		TCCR1B |= (1<<CS12)|(1<<CS10);
		break;
		default:
		TCCR1B |= (1<<CS10);
		break;
	}
}

void initPWM1B(uint8_t invert, uint8_t modo, uint16_t prescaler)
{

	DDRB |= (1<<DDB2);
	
	TCCR1A &= ~((1<<COM1B1)|(1<<COM1B0)|(1<<WGM11)|(1<<WGM10));
	TCCR1B = 0;
	
	if (invert)
	{
		TCCR1A |= (1<<COM1B1)|(1<<COM1B0); 
	}
	else
	{
		TCCR1A |= (1<<COM1B1); 
	}

	switch(modo)
	{
		case 0:
		TCCR1A |= (1<<WGM10); 
		break;
		case 1:
		TCCR1A |= (1<<WGM11); 
		break;
		case 2:
		TCCR1A |= (1<<WGM11)|(1<<WGM10); 
		break;
		case 3:
		TCCR1A |= (1<<WGM10); 
		TCCR1B |= (1<<WGM12);
		break;
		case 4:
		TCCR1A |= (1<<WGM11); 
		TCCR1B |= (1<<WGM12);
		break;
		case 5:
		TCCR1A |= (1<<WGM11)|(1<<WGM10); 
		TCCR1B |= (1<<WGM12);
		break;
		case 6:
		TCCR1B |= (1<<WGM13);
		break;
		case 7:
		TCCR1A |= (1<<WGM10);
		TCCR1B |= (1<<WGM13);
		break;
		case 8:
		TCCR1A |= (1<<WGM11)|(1<<WGM10);
		TCCR1B |= (1<<WGM13);
		break;
		case 9:
		TCCR1A |= (1<<WGM11)|(1<<WGM10);
		TCCR1B |= (1<<WGM12)|(1<<WGM13);
		break;
		case 10:
		TCCR1A |= (1<<WGM11);
		TCCR1B |= (1<<WGM12)|(1<<WGM13);
		break;
		case 11:
		TCCR1A |= (1<<WGM11)|(1<<WGM10);
		TCCR1B |= (1<<WGM12)|(1<<WGM13);
		break;
		default:
		TCCR1A |= (1<<WGM10); 
		break;
	}
	switch(prescaler)
	{
		case 1:
		TCCR1B |= (1<<CS10);
		break;
		case 8:
		TCCR1B |= (1<<CS11);
		break;
		case 64:
		TCCR1B |= (1<<CS11)|(1<<CS10);
		break;
		case 256:
		TCCR1B |= (1<<CS12);
		break;
		case 1024:
		TCCR1B |= (1<<CS12)|(1<<CS10);
		break;
		default:
		TCCR1B |= (1<<CS10);
		break;
	}
}

void updateDutyCycle1A(uint32_t duty)
{
	OCR1A = duty;
}
void updateDutyCycle1B(uint32_t duty)
{
	OCR1B = duty;
}