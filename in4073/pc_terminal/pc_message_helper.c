#include <stdio.h>

#include "pc_rs232.h"
#include "pc_message_helper.h"
#include "../message/message.h"
#include "../message/crc.h"
#include "joystick.h"

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

void build_and_send_message (uint8_t msg_type, message_t * send_buffer)
{
	uint8_t message_len, data_len;
	message_data_u data;

	data = send_buffer->data;

	data_len = sizeof(data);
	switch (msg_type)
	{
		case MSG_SET_MODE:
		{
			data_len = sizeof(data.set_mode_data);
			break;
		}
		case MSG_INPUT_DATA:
		{
			data_len = sizeof(data.input_data);
			break;
		}
		case MSG_TERMINATE:
		{
			data_len = 0;
			break;
		}
		default:
		{
			printf("No valid msg_type is being send: %d\n", msg_type);			
			return;
		}
	
	}

	message_len = build_message(msg_type, (uint8_t *) &data, data_len, 
		send_buffer);

	if (message_len > 0)
	{
		send_message(send_buffer, message_len);
	}
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

// returns mode requested or 0xFF if no mode is set
uint8_t select_message(uint8_t c, message_t * send_buffer)
{
	msg_type_e msg_type;
	uint8_t retval = 0xFF;

	switch(c)
	{
		case '0': // SAFE MODE
		case '1': // PANIC MODE
		case '3': // CALIBRATION MODE
		{
			msg_type = MSG_SET_MODE;
			send_buffer->data.set_mode_data.mode = c - '0';
			retval = c - '0';
			break;
		}
		case '2': // MANUAL MODE
		case '4': // YAW CONTROL MODE
		{
			// first check if joystick is in 'zero' position
			if (!is_joystick_zero())
			{
				printf("Set joystick into zero position\n");
				return retval;
			}

			msg_type = MSG_SET_MODE;
			send_buffer->data.set_mode_data.mode = c - '0';
			retval = c - '0';
			break;
		}
		case 'a': 
		{
			// lift up
			break;
		}
		case 'z': 
		{
			// lift down
			break;
		}
		case 37: // LEFT ARROW 
		{
			// roll up
			break;
		}
		case 39: // RIGHT ARROW 
		{
			// roll down
			break;
		}
		case 38: // UP ARROW 
		{
			// pitch down
			break;
		}
		case 40: // DOWN ARROW 
		{
			// pitch down
			break;
		}
		case 'q':
		{
			// yaw down
			break;
		}
		case 'w':
		{
			// yaw up
			break;
		}
		case 'u':
		{
			// yaw control P up
			break;
		}
		case 'j':
		{
			// yaw control P down
			break;
		}
		case 'i':
		{
			// roll/pitch control P1 up
			break;
		}
		case 'k':
		{
			// roll/pitch control P1 down
			break;
		}
		case 'o':
		{
			// roll/pitch control P2 up
			break;
		}
		case 'l':
		{
			// roll/pitch control P2 down
			break;
		}


		case 27: // escape character
		{
			msg_type = MSG_TERMINATE;
			break;
		}


		default:
		{
			printf("No valid input: %c\n", c);
			return retval;
		}
	}

	build_and_send_message(msg_type, send_buffer);


	return retval;
}