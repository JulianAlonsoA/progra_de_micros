#ifndef ADC_H_
#define ADC_H_

#include <stdint.h>

void ADC_Init(void);
uint16_t ADC_Read(uint8_t channel);
uint8_t ADC_MapToAngle(uint16_t adc_value);

#endif