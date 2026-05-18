#ifndef EEPROM_MANAGER_H_
#define EEPROM_MANAGER_H_

#include <stdint.h>

void EEPROM_SavePose(uint8_t pose_index, uint8_t angle1, uint8_t angle2, uint8_t angle3, uint8_t angle4);
void EEPROM_ReadPose(uint8_t pose_index, uint8_t *angle1, uint8_t *angle2, uint8_t *angle3, uint8_t *angle4);

#endif