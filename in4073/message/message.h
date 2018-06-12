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

#define debug_printf(...) //printf(__VA_ARGS__) 
#define ENABLE_4073_PRINT 

#pragma pack(1)

typedef enum 
{
	MSG_SET_MODE = 0x01,		// PC -> DRONE
	MSG_MODE_UPDATE,			// DRONE -> PC

	MSG_INPUT_DATA,				// PC -> DRONE

	MSG_SET_P_VALUES,			// PC-> DRONE
	MSG_P_VALUES_UPDATE,		// DRONE -> PC

	MSG_CALIBRATION_DATA,		// DRONE -> PC
	MSG_MOTOR_DATA,				// DRONE -> PC
	MSG_BATTERY_DATA,			// DRONE -> PC
	MSG_BAROMETER_DATA,			// DRONE -> PC

	MSG_TERMINATE,				// DRONE -> PC
	MSG_LOGGING_DATA 			// DRONE -> PC

} msg_type_e;

typedef enum 
{
	SAFE_MODE,				// 0
	PANIC_MODE,				// 1
	MANUAL_MODE,			// 2
	CALIBRATION_MODE,		// 3
	YAW_CONTROL_MODE,		// 4
	FULL_CONTROL_MODE,		// 5
	RAW_MODE,				// 6
	HEIGHT_CONTROL_MODE,	// 7
	WIRELESS_MODE,			// 8
	TERMINATE_MODE			// 9
} mode_type_e;

typedef enum 
{
	P_YAW_CONTROL = 0x01,
	P1_PITCH_ROLL_CONTROL,
	P2_PITCH_ROLL_CONTROL
} p_value_select_e;

typedef enum 
{
	INCREMENT = 0x01,
	DECREMENT,
	SET_ABSOLUTE_VALUE
} p_update_mode_e;

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
	int8_t lift;
	int8_t roll;
	int8_t pitch;
	int8_t yaw;
} input_data_t; // PC -> Drone

typedef struct 
{
	uint8_t select;
	uint8_t mode;
	uint8_t value;
} set_p_values_t;

typedef struct 
{
	uint8_t select;
	uint8_t value;
} p_values_update_t;

typedef struct 
{
	int16_t phi;
	int16_t theta;
	int16_t psi;
	int16_t sp;
	int16_t sq;
	int16_t sr;
	int16_t sax;
	int16_t say;
	int16_t saz;
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
	set_p_values_t set_p_values;
	p_values_update_t p_values_update;
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
	bool * is_escaped, uint8_t * buffer, char* source);


#endif // MESSAGE_H__