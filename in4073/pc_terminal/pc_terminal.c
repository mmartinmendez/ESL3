#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#include "pc_rs232.h"
#include "pc_term.h"
#include "pc_message_helper.h"
#include "time_helper.h"
#include "../message/message.h"
#include "../message/crc.h"
#include "joystick.h"

/*----------------------------------------------------------------
 * main -- execute terminal
 *----------------------------------------------------------------
 */

int main(int argc, char **argv)
{
	// current joystick readings
	int	axis[6];
	int	button[12];

	// variables for sending/receiving messages
	char c;
	message_t send_buffer;
	message_t receive_buffer;
	bool is_escaped = false;
	uint8_t msg_index = 0;

	// time variables
	struct timespec t_now, t_message_expect, t_joystick;

	// mode variables
	uint8_t mode_requested = 0;
	uint8_t mode_received = 0;
	
	uint8_t retval = 0;

	// debug variables
	uint8_t counter = 0;

	memset(axis,0,sizeof(axis));
	memset(button,0,sizeof(button));

	term_puts("\nTerminal program - Embedded Real-Time Systems\n");

	init_joystick();
	term_initio();
	crc_init();
	rs232_open();

	term_puts("Type ^C to exit\n");

	// discard any incoming text
	while ((c = rs232_getchar_nb()) != -1)
		fputc(c,stderr);

	// init joystick time
  	clock_gettime(CLOCK_MONOTONIC, &t_joystick);

  	// start GUI thread


	// send & receive
	for (;;)
	{
  		clock_gettime(CLOCK_MONOTONIC, &t_now);

		if ((c = term_getchar_nb()) != -1)
		{
			if ((retval = select_message(c, &send_buffer)) != 0xFF)
			{
				mode_requested = retval;
				t_message_expect = add_time_millis(&t_now, 200);
			}
		}
		if ((c = rs232_getchar_nb()) != -1)
		{
			uint8_t message_len;

			debug_printf(c);

			message_len = parse_message(c, &msg_index, 
				&is_escaped, (uint8_t *) &receive_buffer, "PC");

			if (message_len > 0)
			{
				// we received an end-byte, now handle message
				retval = handle_message(&receive_buffer, message_len);
				if (retval != 0xFF)
				{
					mode_received = retval;
				} 		
			}	
		}

		// This means we have set a mode, but not received the mode update yet
		if (mode_requested != mode_received)
		{
			if (compare_time (&t_now, &t_message_expect))
			{
				// now the expected response is expired, resend request
				select_message(mode_requested + '0', &send_buffer);
				t_message_expect = add_time_millis(&t_now, 200);
			}
		}

		if (compare_time (&t_now, &t_joystick))
		{
			int8_t axis_small[6];

			axis_small[0] = axis[0] / 256;
			axis_small[1] = axis[1] / 256;
			axis_small[2] = axis[2] / 256;
			axis_small[3] = axis[3] / 256;

			read_joystick(axis, button);
			
			// update ui
			// update_gui(axis, button);

			printf("small values: %d | %d | %d | %d\n", axis_small[0], axis_small[1], axis_small[2], axis_small[3]);

			// note: convert int16 to int8
			send_buffer.data.input_data.roll = axis_small[0];
			send_buffer.data.input_data.pitch = axis_small[1];
			send_buffer.data.input_data.yaw = axis_small[2];
			send_buffer.data.input_data.lift = axis_small[3];

			debug_printf("Sending before build_and_send_message, counter: %d\n", counter++);

			build_and_send_message(MSG_INPUT_DATA, &send_buffer);

			t_joystick = add_time_millis(&t_now, 10);
		}
	}

	term_exitio();
	rs232_close();
	term_puts("\n<exit>\n");

	return 0;
}

