/*------------------------------------------------------------------
 *  message.h 
 *	
 *	This file describes the message protocol
 *------------------------------------------------------------------
 */

#define START_BYTE 	0x12
#define END_BYTE 	0x13
#define ESCAPE		0x7D

#ifndef MESSAGE_H__
#define MESSAGE_H__

typedef struct {
	uint8_t message_type;
	uint8_t crc;
	uint8_t data[4];
} standard_message_t;

#endif // MESSAGE_H__