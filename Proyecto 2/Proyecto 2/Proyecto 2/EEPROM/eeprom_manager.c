/*
 * eeprom_manager.c
 *
 * Description:
 * Librería para guardar y leer posiciones del brazo robótico en EEPROM.
 */

/****************************************/
// Encabezado
#include "eeprom_manager.h"
#include <avr/eeprom.h>
#include <stdint.h>

/****************************************/
// Constantes
#define POSE_SIZE 4

/****************************************/
// Variables EEPROM
uint8_t EEMEM eeprom_poses[10][POSE_SIZE];

/****************************************/
// Guardar una pose
void EEPROM_SavePose(uint8_t pose_index, uint8_t angle1, uint8_t angle2, uint8_t angle3, uint8_t angle4)
{
	if (pose_index >= 10)
	{
		return;
	}

	eeprom_update_byte(&eeprom_poses[pose_index][0], angle1);
	eeprom_update_byte(&eeprom_poses[pose_index][1], angle2);
	eeprom_update_byte(&eeprom_poses[pose_index][2], angle3);
	eeprom_update_byte(&eeprom_poses[pose_index][3], angle4);
}

/****************************************/
// Leer una pose
void EEPROM_ReadPose(uint8_t pose_index, uint8_t *angle1, uint8_t *angle2, uint8_t *angle3, uint8_t *angle4)
{
	if (pose_index >= 10)
	{
		return;
	}

	*angle1 = eeprom_read_byte(&eeprom_poses[pose_index][0]);
	*angle2 = eeprom_read_byte(&eeprom_poses[pose_index][1]);
	*angle3 = eeprom_read_byte(&eeprom_poses[pose_index][2]);
	*angle4 = eeprom_read_byte(&eeprom_poses[pose_index][3]);
}