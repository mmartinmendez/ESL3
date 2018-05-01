#include "message.h"
#include "crc.h"

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

// Returns true if end byte is received, false if not
uint8_t parse_message(uint8_t c, uint8_t * msg_index, 
				   bool * is_escaped, uint8_t * buffer)
{
	uint8_t message_len = 0;

	// TODO remove this - debug print received char
	printf("%X\n", c);

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
			return message_len;
		}
		default:
		{
			if(*msg_index >= sizeof(message_t))
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

