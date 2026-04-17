#ifndef PWM2_SERVO_H
#define PWM2_SERVO_H

#include <stdint.h>

// Iniciamos el PWM2 usando el timer2
void PWM2_Init(void);

// Camia el angulo del servo conectado a PWM2
void Servo2_SetAngle(uint8_t angle);

#endif