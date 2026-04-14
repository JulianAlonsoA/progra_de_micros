/*
 * PWM1.h
 *
 * Created: 4/14/2026 12:08:24 PM
 *  Author: Julian Alonso
 */ 


#ifndef PWM1_H_
#define PWM1_H_

#include <avr/io.h>

#define no_invertido 0
#define invertido 1
#define phasePWM_8bit 0
#define phasePWM_9bit 1
#define phasePWM_10bit 2
#define fastPWM_8bit 3
#define fastPWM_9bit 4
#define fastPWM_10bit 5
#define PWM_phase_frequency_ICR1_bottom 6
#define PWM_phase_frequency_OCR1A_bottom 7
#define phasePWM_ICR1_top 8
#define phasePWM_OCR1A_top 9
#define fastPWM_ICR1_top 10
#define fastPWM_OCR1A_top 11




void initPWM1A(uint8_t invert, uint8_t modo, uint16_t prescaler);
void initPWM1B(uint8_t invert, uint8_t modo, uint16_t prescaler);

void updateDutyCycle1A(uint32_t duty);
void updateDutyCycle1B(uint32_t duty);






#endif /* PWM1_H_ */