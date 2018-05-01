/*------------------------------------------------------------
 * Simple pc terminal in C
 *
 * Arjan J.C. van Gemund (+ mods by Ioannis Protonotarios)
 *
 * read more: http://mirror.datenwolf.net/serial/
 *------------------------------------------------------------
 */

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <inttypes.h>
#include "../message/message.h"
#include "../message/crc.h"

/*------------------------------------------------------------
 * console I/O
 *------------------------------------------------------------
 */
struct termios 	savetty;

void	term_initio()
{
	struct termios tty;

	tcgetattr(0, &savetty);
	tcgetattr(0, &tty);

	tty.c_lflag &= ~(ECHO|ECHONL|ICANON|IEXTEN);
	tty.c_cc[VTIME] = 0;
	tty.c_cc[VMIN] = 0;

	tcsetattr(0, TCSADRAIN, &tty);
}

void	term_exitio()
{
	tcsetattr(0, TCSADRAIN, &savetty);
}

void	term_puts(char *s)
{
	fprintf(stderr,"%s",s);
}

void	term_putchar(char c)
{
	putc(c,stderr);
}

int	term_getchar_nb()
{
        static unsigned char 	line [2];

        if (read(0,line,1)) // note: destructive read
        		return (int) line[0];

        return -1;
}

int	term_getchar()
{
        int    c;

        while ((c = term_getchar_nb()) == -1)
                ;
        return c;
}

/*------------------------------------------------------------
 * Serial I/O
 * 8 bits, 1 stopbit, no parity,
 * 115,200 baud
 *------------------------------------------------------------
 */
#include <termios.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>

int serial_device = 0;
int fd_RS232;

void rs232_open(void)
{
  	char 		*name;
  	int 		result;
  	struct termios	tty;

       	fd_RS232 = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY);  // Hardcode your serial port here, or request it as an argument at runtime

	assert(fd_RS232>=0);

  	result = isatty(fd_RS232);
  	assert(result == 1);

  	name = ttyname(fd_RS232);
  	assert(name != 0);

  	result = tcgetattr(fd_RS232, &tty);
	assert(result == 0);

	tty.c_iflag = IGNBRK; /* ignore break condition */
	tty.c_oflag = 0;
	tty.c_lflag = 0;

	tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; /* 8 bits-per-character */
	tty.c_cflag |= CLOCAL | CREAD; /* Ignore model status + read input */

	cfsetospeed(&tty, B115200);
	cfsetispeed(&tty, B115200);

	tty.c_cc[VMIN]  = 0;
	tty.c_cc[VTIME] = 1; // added timeout

	tty.c_iflag &= ~(IXON|IXOFF|IXANY);

	result = tcsetattr (fd_RS232, TCSANOW, &tty); /* non-canonical */

	tcflush(fd_RS232, TCIOFLUSH); /* flush I/O buffer */
}


void 	rs232_close(void)
{
  	int 	result;

  	result = close(fd_RS232);
  	assert (result==0);
}


int	rs232_getchar_nb()
{
	int 		result;
	unsigned char 	c;

	result = read(fd_RS232, &c, 1);

	if (result == 0)
		return -1;

	else
	{
		assert(result == 1);
		return (int) c;
	}
}


int 	rs232_getchar()
{
	int 	c;

	while ((c = rs232_getchar_nb()) == -1)
		;
	return c;
}


int 	rs232_putchar(char c)
{
	int result;

	do {
		result = (int) write(fd_RS232, &c, 1);
	} while (result == 0);

	assert(result == 1);
	return result;
}

uint8_t pc_build_message(uint8_t message_type, uint8_t* data, uint8_t data_len, message_t * message)
{
	memset(message, 0, sizeof(message_t));

	// fill in message data members
	message->message_type = message_type;

	if (data_len > sizeof(message->data))
		return data_len - sizeof(message->data);
	
	memcpy((void*) &(message->data), data, data_len);

	data_len =  data_len + 3; // correct for crc and msg type size

	message->crc = 0; 
	message->crc = crc_fast((unsigned char const*) message, data_len);


	return 0;
}

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

void select_message(uint8_t c, message_t * send_buffer)
{
	msg_type_e msg_type;
	message_data_u data;
	uint8_t data_len = 0;
	uint8_t message_len = 0;

	switch(c)
	{
		case '1':
		{
			msg_type = set_mode;
			data.set_mode_data.mode = 0;
			data_len = sizeof(data.set_mode_data);
			break;
		}
		case '2':
		{
			msg_type = input_data;
			data.input_data.lift = 10;
			data.input_data.roll = 20;
			data.input_data.pitch = 30;
			data.input_data.yaw = 40;
			data_len = sizeof(data.input_data);
			break;
		}

		default:
		{
			printf("No valid input\n");
			return;
		}
	}

	printf("data_len: %d\n", data_len);

	message_len = build_message(msg_type, (uint8_t *) &data, data_len, send_buffer);

	if (message_len > 0)
	{
		send_message(send_buffer, message_len);
	}
}

/*----------------------------------------------------------------
 * main -- execute terminal
 *----------------------------------------------------------------
 */

int main(int argc, char **argv)
{
	char	c;

	message_t send_buffer;

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
		if ((c = term_getchar_nb()) != -1)
		{
			select_message(c, &send_buffer);
			//rs232_putchar(c);
		}

		if ((c = rs232_getchar_nb()) != -1)
			term_putchar(c);

	}

	term_exitio();
	rs232_close();
	term_puts("\n<exit>\n");

	return 0;
}

