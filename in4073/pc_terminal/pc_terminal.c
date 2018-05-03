#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#include "pc_rs232.h"
#include "pc_term.h"
#include "pc_message_helper.h"
#include "time_helper.h"
#include "../message/message.h"
#include "../message/crc.h"

/*----------------------------------------------------------------
 * main -- execute terminal
 *----------------------------------------------------------------
 */

int main(int argc, char **argv)
{
	char	c;

	message_t send_buffer;
	// variables for receiving messages
	message_t receive_buffer;
	bool is_escaped = false;
	uint8_t msg_index = 0;
	struct timespec t_now, t_expect;
	uint8_t mode_requested = 0;
	uint8_t mode_received = 0;
	uint8_t retval;

	term_puts("\nTerminal program - Embedded Real-Time Systems\n");

	term_initio();
	crc_init();
	rs232_open();

	term_puts("Type ^C to exit\n");

	/* discard any incoming text
	 */
	while ((c = rs232_getchar_nb()) != -1)
		fputc(c,stderr);

	/* send & receive
	 */
	for (;;)
	{
  		clock_gettime(CLOCK_MONOTONIC, &t_now);

		if ((c = term_getchar_nb()) != -1)
		{
			if ((retval = select_message(c, &send_buffer)) != 0xFF)
			{
				mode_requested = retval;
				t_expect = add_time_millis(&t_now, 200);
			}
		}
		if ((c = rs232_getchar_nb()) != -1)
		{
			uint8_t message_len;

			#ifdef ENABLE_4073_PRINT
			term_putchar(c);
			#endif

			message_len = parse_message(c, &msg_index, 
				&is_escaped, (uint8_t *) &receive_buffer, "PC");

			if (message_len > 0)
			{
				// we received an end-byte, now handle message
				handle_message(&receive_buffer, message_len); 		
			}	
		}

		// This means we have set a mode, but not received the mode update yet
		if (mode_requested != mode_received)
		{
			if (compare_time (&t_now, &t_expect))
			{
				// now the expected response is expired, resend request
				select_message(mode_requested + '0', &send_buffer);
				t_expect = add_time_millis(&t_now, 200);
			}
		}
	}

	term_exitio();
	rs232_close();
	term_puts("\n<exit>\n");

	return 0;
}

