#ifndef PWM_SERVO_H_
#define PWM_SERVO_H_

#include <stdint.h>

void Servo_Init(void);
void Servo_SetAngle(uint8_t servo, uint8_t angle);
void Servo_SetPulse(uint8_t servo, uint16_t pulse_us);

#endif
