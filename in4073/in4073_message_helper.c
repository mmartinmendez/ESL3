#include <stdio.h>

#include "in4073_message_helper.h"
#include "message/message.h"
#include "message/crc.h"
#include "in4073.h"

void send_message(message_t * message, uint8_t message_len)
{
	uint8_t * message_ptr = (uint8_t *) message;

	putchar(START_BYTE);

	// send data over uart
	for (int i = 0; i < message_len; i++)
	{
		// check for special bytes, and add escaping where necessary
		if(*(message_ptr) == START_BYTE ||
			*(message_ptr) == END_BYTE ||
			*(message_ptr) == ESCAPE )
		{
			putchar(ESCAPE);
			putchar(*(message_ptr++) ^ 0x20);
		} 
		else
		{
			putchar(*(message_ptr++));
		}

	}

	putchar(END_BYTE);
	putchar('\n'); // add newline to flush send buffer
}

void send_terminate(message_t * send_buffer)
{
	uint8_t message_len;

	message_len = build_message(MSG_TERMINATE, NULL, 0, send_buffer);

	if (message_len > 0)
	{
		send_message(send_buffer, message_len);
	}
}

void send_mode_update(message_t * send_buffer, uint8_t mode)
{
	mode_update_t data;
	uint8_t message_len;

	data.mode = mode;
	message_len = build_message(MSG_MODE_UPDATE, (uint8_t *) &data, 
		sizeof(data), send_buffer);

	if (message_len > 0)
	{
		send_message(send_buffer, message_len);
	}
}

void send_p_values_update(message_t * send_buffer, uint8_t select)
{
	p_values_update_t data;
	uint8_t message_len;

	data.select = select;

	switch (select)
	{
		case P_YAW_CONTROL:
		{
			data.value = p_yaw_control;
			break;
		}
		case P1_PITCH_ROLL_CONTROL:
		{
			data.value = p1;
			break;
		}
		case P2_PITCH_ROLL_CONTROL:
		{
			data.value = p2;
			break;
		}
		default:
		{
			data.value = 0;
		}
	}

	message_len = build_message(MSG_P_VALUES_UPDATE, (uint8_t *) &data, 
		sizeof(data), send_buffer);

	if (message_len > 0)
	{
		send_message(send_buffer, message_len);
	}
}

void send_calibration_data(message_t * send_buffer, int16_t phi, int16_t theta, 
	int16_t psi, int16_t sp, int16_t sq, int16_t sr, int16_t sax, int16_t say, 
	int16_t saz)
{
	calibration_data_t data;
	uint8_t message_len;

 	data.phi = phi;
 	data.theta = theta;
 	data.psi = psi;
 	data.sp = sp;
 	data.sq = sq;
 	data.sr = sr;
 	data.sax = sax;
 	data.say = say;
 	data.saz = saz;

	message_len = build_message(MSG_CALIBRATION_DATA, (uint8_t *) &data, 
			sizeof(data), send_buffer);

	if (message_len > 0)
	{
		send_message(send_buffer, message_len);
	}
}

static void change_p_values(set_p_values_t * data)
{
	int8_t * selected_p_value;
	switch (data->select)
	{
		case P_YAW_CONTROL:
		{
			selected_p_value = &p_yaw_control;
			break;
		}
		case P1_PITCH_ROLL_CONTROL:
		{
			selected_p_value = &p1;
			break;
		}
		case P2_PITCH_ROLL_CONTROL:
		{
			selected_p_value = &p2;
			break;
		}
		default:
		{
			printf("No valid p value selected: %d", data->select);
			return;
		}
	}

	switch (data->mode)
	{
		case INCREMENT:
		{
			// prevent overflow
			if (*selected_p_value < 126)
			{
				*selected_p_value += data->value;
			}
			break;
		}
		case DECREMENT:
		{
			*selected_p_value -= data->value;
			if(*selected_p_value < 0)
			{
				*selected_p_value = 0; // we don't want negative p values	
			} 
			break;
		}
		case SET_ABSOLUTE_VALUE:
		{
			*selected_p_value = data->value;
			break;
		}
	}


}

// return mode set, or 0xFF
uint8_t handle_message(message_t * send_buffer, uint8_t * receive_buffer, 
	uint8_t buffer_len, bool * demo_done)
{
	message_t * message_ptr = (message_t *) receive_buffer;
	uint8_t retval = 0xFF;

	switch ((msg_type_e) message_ptr->message_type)
	{
		case MSG_SET_MODE:
		{
			set_mode_data_t * data = (set_mode_data_t*) &(message_ptr->data);
			printf("Received set mode command, mode: %d\n", data->mode);
			retval = data->mode;

			// Now send mode update back to the pc
			send_mode_update(send_buffer, data->mode);

			break;
		}
		case MSG_INPUT_DATA:
		{
			input_data_t * data = (input_data_t*) &(message_ptr->data);
			liftdata = data->lift;
			rolldata = data->roll;
			pitchdata = data->pitch;
			yawdata = data->yaw;

			break;
		}
		case MSG_SET_P_VALUES:
		{
			set_p_values_t * data = (set_p_values_t*) &(message_ptr->data);
			change_p_values(data);
			send_p_values_update(send_buffer, data->select);
			break;
		}
		default:
		{
			printf("Received unsupported msg_type: %d\n", message_ptr->message_type);
			break;
		}
	}

	return retval;
}