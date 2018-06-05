#include "message.h"
#include "crc.h"

// returns length of message build
uint8_t build_message(uint8_t message_type, uint8_t* message_data, 
				      uint8_t data_len, message_t * message)
{
	// initialze message struct to zero
	memset(message, 0, sizeof(message_t));

	// fill in message data members
	message->message_type = message_type;

	if (data_len > sizeof(message->data))
		return 0;
	
	memcpy((void*) &(message->data), message_data, data_len);

	data_len =  data_len + 3; // correct for crc and msg type size

	message->crc = 0; 
	message->crc = crc_fast((unsigned char const*) message, data_len);

	return data_len;
}

// return true if crc checks out, return false if not
bool validate_message(uint8_t * buffer, uint8_t buffer_len, char * source)
{

	message_t * message_ptr = (message_t *) buffer;
	uint16_t crc_received, crc_calculated;

	// if we receive more bytes than we expect, return
	if(buffer_len > sizeof(message_t))
		return false;

	// store the received crc
	crc_received = message_ptr->crc;
	
	// calculte the crc
	message_ptr->crc = 0;
	crc_calculated = crc_fast((unsigned char const*) message_ptr, buffer_len);

	return (crc_received == crc_calculated);
}

// Returns true if end byte is received, false if not
uint8_t parse_message(uint8_t c, uint8_t * msg_index, bool * is_escaped, 
					  uint8_t * buffer, char* source)
{
	uint8_t message_len = 0;

	// check for escaped bytes, discard escape bytes
	if (*is_escaped == false && c == ESCAPE)
	{
		*is_escaped = true;
		return 0;
	}

	// if this byte is escaped, reXOR it
	if (*is_escaped == true)
	{
		c ^= 0x20;
		*is_escaped = false;
	}

	// handle the incomming char
	switch (c)
	{
		case START_BYTE:
		{
			*msg_index = 0; 
			return 0;
		}
		case END_BYTE:
		{
			message_len = *msg_index;
			*msg_index = 0;
			
			if (validate_message(buffer, message_len, source))
			{
				return message_len;
			}
			
			return 0;

		}
		default:
		{
			if (*msg_index >= sizeof(message_t))
			{
				*msg_index = 0;
				return 0;
			}

			buffer[(*msg_index)++] = c;
			return 0;
		}
	}

	// should not get here
	return 0;
}

