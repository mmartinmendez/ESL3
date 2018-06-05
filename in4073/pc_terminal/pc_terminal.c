#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <pthread.h>

#include "pc_rs232.h"
#include "pc_term.h"
#include "pc_message_helper.h"
#include "time_helper.h"
#include "../message/message.h"
#include "../message/crc.h"
#include "joystick.h"
#include "gui.h"

/*----------------------------------------------------------------
 * main -- execute terminal
 *----------------------------------------------------------------
 */

// #define USE_GUI

int main(int argc, char **argv)
{

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

	// other
	bool demo_done = false;
	bool offset_update = false;
	uint8_t retval = 0;

	memset(axis_small,0,sizeof(axis_small));
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

	#ifdef USE_GUI
  	// start GUI thread
  	pthread_t gui_thread;
  	
	if (pthread_create (&gui_thread, NULL, run_gui, (void *) argv))
	{
    	perror("ERROR creating jsfunc thread.");
	}
	#endif

	// send & receive
	while(!demo_done)
	{
  		clock_gettime(CLOCK_MONOTONIC, &t_now);

  		if (compare_time (&t_now, &t_joystick))
		{
			bool joystick_update = read_joystick(axis_small, button);
			
			// only send new data when a change is detected
			#ifdef USE_GUI
			update_gui(axis, button);
			#endif

			// add keyboard offset to joystick values + check for overflow
			int8_t axis_totals[4];
			for (int i = 0; i < 4; i++)
			{
				int8_t x = axis_small[i];
				int8_t y = axis_offsets[i];

				if ((y > 0) && (x > (127 - y)))
				{
					axis_totals[i] = 127; // overflow
				} 
				else if ((y < 0) && (x < (-127 - y)))
				{
				    axis_totals[i] = -127; // underflow
				}
				else
				{
				    axis_totals[i] = x + y;
				}
			}

			// send joystick values to DRONE
			send_buffer.data.input_data.roll = axis_totals[0];
			send_buffer.data.input_data.pitch = axis_totals[1];
			send_buffer.data.input_data.yaw = axis_totals[2];
			send_buffer.data.input_data.lift = axis_totals[3];

			build_and_send_message(MSG_INPUT_DATA, &send_buffer);
			if (joystick_update || offset_update)
			{
			#ifndef DONT_PRINT_JS_VALUES
				printf("small values: %d | %d | %d | %d\n", 
					axis_totals[0], axis_totals[1], 
					axis_totals[2], axis_totals[3]);
			#endif
			}

			// fire button is pressed, go to panic mode
			if ((button[0] > 0) && (mode_received != PANIC_MODE))
			{
				retval = select_message('1', &send_buffer);
				if (retval < 0x0F)
				{
					mode_requested = retval;
					t_message_expect = add_time_millis(&t_now, 200);
				}	
			}
			
			offset_update = false;
			t_joystick = add_time_millis(&t_now, 10);
		}

		if ((c = term_getchar_nb()) != -1)
		{
			retval = select_message(c, &send_buffer);
			if (retval < 0x0F)
			{
				mode_requested = retval;
				t_message_expect = add_time_millis(&t_now, 200);
			}
			else
			{
				// new offset is created, send new joystick values
				offset_update = true;
			}
		}

		if ((c = rs232_getchar_nb()) != -1)
		{
			uint8_t message_len;

			putchar(c);

			message_len = parse_message(c, &msg_index, 
				&is_escaped, (uint8_t *) &receive_buffer, "PC");

			if (message_len > 0)
			{
				// we received an end-byte, now handle message
				retval = handle_message(&receive_buffer, message_len);
				if (retval < 0x0F)
				{
					mode_received = retval;
				} 	
				else if (retval == 27)
				{
					demo_done = true;
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
	}

	term_exitio();
	rs232_close();
	term_puts("\n<exit>\n");

	return 0;
}

