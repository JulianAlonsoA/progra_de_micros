#ifndef PWM3_MANUAL_H
#define PWM3_MANUAL_H

#include <stdint.h>

void ManualPWM_Init(void);
void ManualPWM_SetDuty(uint8_t duty_percent);

#endif