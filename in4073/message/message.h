/*------------------------------------------------------------------
 *  message.h 
 *	
 *	This file describes the message protocol
 *------------------------------------------------------------------
 */

#ifndef MESSAGE_H__
#define MESSAGE_H__

#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define START_BYTE 	0x12
#define END_BYTE 	0x13
#define ESCAPE		0x7D



#pragma pack(1)

typedef enum 
{
	set_mode = 0x01,		// PC -> Drone
	mode_update,			// Drone -> PC
	input_data,				// PC -> Drone

	calibration_data,		// Drone -> PC
	motor_data,				// Drone -> PC
	battery_data,			// Drone -> PC
	barometer_data,			// Drone -> PC

	logging_data 			// Drone -> PC

} msg_type_e;

typedef struct 
{
	uint8_t mode;
} set_mode_data_t; // PC -> Drone

typedef struct 
{
	uint8_t mode;
} mode_update_t; // Drone -> PC

typedef struct 
{
	uint16_t lift;
	uint16_t roll;
	uint16_t pitch;
	uint16_t yaw;
} input_data_t; // PC -> Drone

typedef struct 
{
	int16_t phi;
	int16_t theta;
	int16_t psi;
} calibration_data_t; // Drone -> PC

typedef struct 
{
	int16_t motor_rpm[4];
} motor_data_t; // Drone -> PC

typedef struct 
{
	uint16_t battery_voltage;
} battery_data_t; // Drone -> PC

typedef struct 
{
	int32_t pressure;
} barometer_data_t; // Drone -> PC

typedef struct  // maybe leave this out of the protocol
{
	uint32_t time_us;
	uint16_t lift;
	uint16_t roll;
	uint16_t pitch;
	uint16_t yaw;
	int16_t	ae[4];
	int16_t phi;
	int16_t theta;
	int16_t psi;
	int16_t sp;
	int16_t sq; 
	int16_t sr;
	uint16_t bat_volt;
	int32_t temperature;
	int32_t pressure;
} logging_data_t; // Drone -> PC

typedef union {
	set_mode_data_t set_mode_data;
	mode_update_t mode_update;
	input_data_t input_data;
	calibration_data_t calibration_data;
	motor_data_t motor_data;
	battery_data_t battery_data;
	barometer_data_t barometer_data;
	logging_data_t logging_data;
} message_data_u;

typedef struct {
	uint8_t message_type;
	uint16_t crc;
	message_data_u data;
} message_t;

uint8_t build_message(uint8_t message_type, uint8_t* message_data, 
				   	  uint8_t data_len, message_t * message);
uint8_t parse_message(uint8_t c, uint8_t * msg_index, 
				   bool * is_escaped, uint8_t * buffer);


#endif // MESSAGE_H__