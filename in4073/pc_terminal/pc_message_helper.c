#include <stdio.h>

#include "pc_rs232.h"
#include "pc_message_helper.h"
#include "../message/message.h"
#include "../message/crc.h"

void send_message(message_t * message, uint8_t message_len)
{
	uint8_t * message_ptr = (uint8_t *) message;

	rs232_putchar(START_BYTE);

	// send data over uart
	for (int i = 0; i < message_len; i++)
	{
		// check for special bytes, and add escaping where necessary
		if(*(message_ptr) == START_BYTE ||
			*(message_ptr) == END_BYTE ||
			*(message_ptr) == ESCAPE )
		{
			rs232_putchar(ESCAPE);
			rs232_putchar(*(message_ptr++) ^ 0x20);
		} 
		else
		{
			rs232_putchar(*(message_ptr++));
		}

	}

	rs232_putchar(END_BYTE);
}

// returns mode requested or 0xFF if no mode is set
uint8_t select_message(uint8_t c, message_t * send_buffer)
{
	msg_type_e msg_type;
	message_data_u data;
	uint8_t data_len = 0;
	uint8_t message_len = 0;
	uint8_t retval = 0xFF;

	switch(c)
	{
		case '0':
		case '1':
		case '2':
		{
			msg_type = MSG_SET_MODE;
			data.set_mode_data.mode = c - '0';
			retval = c - '0';
			data_len = sizeof(data.set_mode_data);
			break;
		}
		case 27: // escape character
		{
			msg_type = MSG_TERMINATE;
			data_len = 0;
			break;
		}
		case 'a':
		{
			msg_type = MSG_INPUT_DATA;
			data.input_data.lift = 10;
			data.input_data.roll = 20;
			data.input_data.pitch = 30;
			data.input_data.yaw = 40;
			data_len = sizeof(data.input_data);
			break;
		}
		default:
		{
			printf("No valid input: %c\n", c);
			return retval;
		}
	}

	message_len = build_message(msg_type, (uint8_t *) &data, data_len, 
		send_buffer);

	if (message_len > 0)
	{
		send_message(send_buffer, message_len);
	}

	return retval;
}


uint8_t handle_message(message_t * buffer, uint8_t buffer_len)
{
	uint8_t retval = 0xFF;

	switch ((msg_type_e) buffer->message_type)
	{
		case MSG_MODE_UPDATE:
		{
			mode_update_t * data = (mode_update_t*) &(buffer->data);
			printf("PC: Received mode update command, mode: %d\n", data->mode);
			retval = data->mode;
			break;
		}

		default:
		{
			printf("PC: Received unsupported msg_type: %d\n", 
				buffer->message_type);
			break;
		}
	}

	return retval;
}