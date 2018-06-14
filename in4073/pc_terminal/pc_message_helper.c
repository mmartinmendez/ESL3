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
		case MSG_SET_P_VALUES:
		{
			data_len = sizeof(data.set_p_values);
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

char* get_mode_name(uint8_t mode)
{
	char* mode_names[10] = 
	{	
		"SAFE MODE",			// 0
		"PANIC_MODE",			// 1
		"MANUAL_MODE",			// 2
		"CALIBRATION_MODE",		// 3
		"YAW_CONTROL_MODE",		// 4
		"FULL_CONTROL_MODE",		// 5
		"RAW_MODE",			// 6
		"HEIGHT_CONTROL_MODE",		// 7
		"WIRELESS_MODE",		// 8
		"TERMINATE_MODE"		// 9
	};

	if (mode < 10)
	{
		return mode_names[mode];
	}
	else
	{
		return "mode not found";
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
			printf("PC: Received mode update command, mode: %d --> %s\n",
				data->mode, get_mode_name(data->mode));
			retval = data->mode;
			break;
		}

		case MSG_TERMINATE:
		{
			retval = 27; // escape char
			break;
		}

		case MSG_P_VALUES_UPDATE:
		{
			p_values_update_t * data = (p_values_update_t *) &(buffer->data);
			printf("received p value update: ");
			switch (data->select)
			{
				case P_YAW_CONTROL:
				{
					printf("p yaw control");
					break;
				}
				case P1_PITCH_ROLL_CONTROL:
				{
					printf("p1");
					break;
				}
				case P2_PITCH_ROLL_CONTROL:
				{
					printf("p2");
					break;
				}
				default:
				{
					printf("p error");
				}
			}
			printf(", value: %d\n", data->value);
			break;
		}

		case MSG_CALIBRATION_DATA:
		{
			calibration_data_t * data = (calibration_data_t *) &(buffer->data);
			printf("Received calibration data: \n"
				"phi: %d, theta: %d, psi: %d\n"
				"sp: %d, sq: %d, sr: %d\n"
				"sax: %d, say: %d, saz: %d\n",
				data->phi, data->theta, data->psi,
				data->sp, data->sq, data->sr,
				data->sax, data->say, data->saz);
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
	uint8_t retval = 0xFF;

	switch(c)
	{
		case '0': // SAFE MODE
		case '1': // PANIC MODE
		case '3': // CALIBRATION MODE
		{
			send_buffer->data.set_mode_data.mode = c - '0';
			retval = c - '0';
			build_and_send_message(MSG_SET_MODE, send_buffer);
			break;
		}
		case '2': // MANUAL MODE
		case '4': // YAW CONTROL MODE
		case '5': // FULL CONTROL MODE
		{
			// first check if joystick is in 'zero' position
			if (!is_joystick_zero())
			{
				printf("Set joystick into zero position\n");
			}
			else
			{
				send_buffer->data.set_mode_data.mode = c - '0';
				retval = c - '0';
				build_and_send_message(MSG_SET_MODE, send_buffer);	
			}
			break;
		}
		case '9':
		case 27: // escape character
		{
			send_buffer->data.set_mode_data.mode = TERMINATE_MODE;
			retval = TERMINATE_MODE;
			build_and_send_message(MSG_SET_MODE, send_buffer);
			break;
		}
		case 'a': 
		{
			axis_offsets[3] += 1; // lift up
			break;
		}
		case 'z': 
		{
			axis_offsets[3] -= 1; // lift down
			break;
		}
		//TODO verify the arrow values with joystick
		case 's': // LEFT ARROW 
		{
			axis_offsets[0] += 1; // roll up
			break;
		}
		case 'x': // RIGHT ARROW 
		{
			axis_offsets[0] -= 1; // roll down
			break;
		}
		case 'd': // UP ARROW 
		{
			axis_offsets[1] -= 1; // pitch down
			break;
		}
		case 'c': // DOWN ARROW 
		{
			axis_offsets[1] += 1; // pitch up
			break;
		}
		case 'q':
		{
			axis_offsets[2] -= 1;// yaw down
			break;
		}
		case 'w':
		{
			axis_offsets[2] += 1;// yaw up
			break;
		}
		case 'u':
		{
			// yaw control P up
			send_buffer->data.set_p_values.select = P_YAW_CONTROL;
			send_buffer->data.set_p_values.mode = INCREMENT;
			send_buffer->data.set_p_values.value = 1;
			build_and_send_message(MSG_SET_P_VALUES, send_buffer);
			break;
		}
		case 'j':
		{
			// yaw control P down
			send_buffer->data.set_p_values.select = P_YAW_CONTROL;
			send_buffer->data.set_p_values.mode = DECREMENT;
			send_buffer->data.set_p_values.value = 1;
			build_and_send_message(MSG_SET_P_VALUES, send_buffer);
			break;
		}
		case 'i':
		{
			// roll/pitch control P1 up
			send_buffer->data.set_p_values.select = P1_PITCH_ROLL_CONTROL;
			send_buffer->data.set_p_values.mode = INCREMENT;
			send_buffer->data.set_p_values.value = 1;
			build_and_send_message(MSG_SET_P_VALUES, send_buffer);
			break;
		}
		case 'k':
		{
			// roll/pitch control P1 down
			send_buffer->data.set_p_values.select = P1_PITCH_ROLL_CONTROL;
			send_buffer->data.set_p_values.mode = DECREMENT;
			send_buffer->data.set_p_values.value = 1;
			build_and_send_message(MSG_SET_P_VALUES, send_buffer);
			break;
		}
		case 'o':
		{
			// roll/pitch control P2 up
			send_buffer->data.set_p_values.select = P2_PITCH_ROLL_CONTROL;
			send_buffer->data.set_p_values.mode = INCREMENT;
			send_buffer->data.set_p_values.value = 1;
			build_and_send_message(MSG_SET_P_VALUES, send_buffer);
			break;
		}
		case 'l':
		{
			// roll/pitch control P2 down
			send_buffer->data.set_p_values.select = P2_PITCH_ROLL_CONTROL;
			send_buffer->data.set_p_values.mode = DECREMENT;
			send_buffer->data.set_p_values.value = 1;
			build_and_send_message(MSG_SET_P_VALUES, send_buffer);
			break;
		}

		default:
		{
			printf("No valid input: %c\n", c);
			break;
		}
	}

	return retval;
}
