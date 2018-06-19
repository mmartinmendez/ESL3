#include <stdio.h>
#include <stdint.h>
#include "pc_term.h"
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
uint8_t select_message(uint8_t k, message_t * send_buffer)
{
	uint8_t retval = 0xFF;

	switch(k)
	{
		case KEY_0: // SAFE MODE
		{
			send_buffer->data.set_mode_data.mode = 0;
			retval = 0;
			build_and_send_message(MSG_SET_MODE, send_buffer);
			break;
		}
		case KEY_1: // PANIC MODE
		{
			send_buffer->data.set_mode_data.mode = 1;
			retval = 1;
			build_and_send_message(MSG_SET_MODE, send_buffer);
			break;
		}
		case KEY_3: // CALIBRATION MODE
		{
			send_buffer->data.set_mode_data.mode = 3;
			retval = 3;
			build_and_send_message(MSG_SET_MODE, send_buffer);
			break;
		}
		case KEY_2: // MANUAL MODE
		{
			// first check if joystick is in 'zero' position
			if (!is_joystick_zero())
			{
				printf("Set joystick into zero position\n");
			}
			else
			{
				send_buffer->data.set_mode_data.mode = 2;
				retval = 2;
				build_and_send_message(MSG_SET_MODE, send_buffer);
			}
			break;
		}
		case KEY_4: // YAW CONTROL MODE
		{
			// first check if joystick is in 'zero' position
			if (!is_joystick_zero())
			{
				printf("Set joystick into zero position\n");
			}
			else
			{
				send_buffer->data.set_mode_data.mode = 2;
				retval = 2;
				build_and_send_message(MSG_SET_MODE, send_buffer);
			}
			break;
		}
		case KEY_5: // FULL CONTROL MODE
		{
			// first check if joystick is in 'zero' position
			if (!is_joystick_zero())
			{
				printf("Set joystick into zero position\n");
			}
			else
			{
				send_buffer->data.set_mode_data.mode = 5;
				retval = 5;
				build_and_send_message(MSG_SET_MODE, send_buffer);
			}
			break;
		}
		case KEY_9:
		case KEY_ESCAPE: // escape character
		{
			send_buffer->data.set_mode_data.mode = TERMINATE_MODE;
			retval = TERMINATE_MODE;
			build_and_send_message(MSG_SET_MODE, send_buffer);
			break;
		}
		case KEY_A:
		{
			axis_offsets[3] += 1; // lift up
			break;
		}
		case KEY_Z:
		{
			axis_offsets[3] -= 1; // lift down
			break;
		}
		//TODO verify the arrow values with joystick
		case KEY_LEFT: // LEFT ARROW
		{
			axis_offsets[0] += 1; // roll up
			break;
		}
		case KEY_RIGHT: // RIGHT ARROW
		{
			axis_offsets[0] -= 1; // roll down
			break;
		}
		case KEY_UP: // UP ARROW
		{
			axis_offsets[1] -= 1; // pitch down
			break;
		}
		case KEY_DOWN: // DOWN ARROW
		{
			axis_offsets[1] += 1; // pitch up
			break;
		}
		case KEY_Q:
		{
			axis_offsets[2] -= 1;// yaw down
			break;
		}
		case KEY_W:
		{
			axis_offsets[2] += 1;// yaw up
			break;
		}
		case KEY_U:
		{
			// yaw control P up
			send_buffer->data.set_p_values.select = P_YAW_CONTROL;
			send_buffer->data.set_p_values.mode = INCREMENT;
			send_buffer->data.set_p_values.value = 1;
			build_and_send_message(MSG_SET_P_VALUES, send_buffer);
			break;
		}
		case KEY_J:
		{
			// yaw control P down
			send_buffer->data.set_p_values.select = P_YAW_CONTROL;
			send_buffer->data.set_p_values.mode = DECREMENT;
			send_buffer->data.set_p_values.value = 1;
			build_and_send_message(MSG_SET_P_VALUES, send_buffer);
			break;
		}
		case KEY_I:
		{
			// roll/pitch control P1 up
			send_buffer->data.set_p_values.select = P1_PITCH_ROLL_CONTROL;
			send_buffer->data.set_p_values.mode = INCREMENT;
			send_buffer->data.set_p_values.value = 1;
			build_and_send_message(MSG_SET_P_VALUES, send_buffer);
			break;
		}
		case KEY_K:
		{
			// roll/pitch control P1 down
			send_buffer->data.set_p_values.select = P1_PITCH_ROLL_CONTROL;
			send_buffer->data.set_p_values.mode = DECREMENT;
			send_buffer->data.set_p_values.value = 1;
			build_and_send_message(MSG_SET_P_VALUES, send_buffer);
			break;
		}
		case KEY_O:
		{
			// roll/pitch control P2 up
			send_buffer->data.set_p_values.select = P2_PITCH_ROLL_CONTROL;
			send_buffer->data.set_p_values.mode = INCREMENT;
			send_buffer->data.set_p_values.value = 1;
			build_and_send_message(MSG_SET_P_VALUES, send_buffer);
			break;
		}
		case KEY_L:
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
			printf("No valid input: %c\n", k);
			break;
		}
	}

	return retval;
}
