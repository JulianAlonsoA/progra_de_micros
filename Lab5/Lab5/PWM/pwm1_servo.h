#ifndef PWM1_SERVO_H
#define PWM1_SERVO_H

#include <stdint.h>

//  Iniciamos el PWM1 usando el timer1
void PWM1_Init(void);

// Cambia el angulo del servo conectado a PWM1
void Servo1_SetAngle(uint8_t angle);

#endif