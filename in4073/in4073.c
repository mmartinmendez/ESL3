/*------------------------------------------------------------------
 *  in4073.c -- test QR engines and sensors
 *
 *  reads ae[0-3] uart rx queue
 *  (q,w,e,r increment, a,s,d,f decrement)
 *
 *  prints timestamp, ae[0-3], sensors to uart tx queue
 *
 *  I. Protonotarios
 *  Embedded Software Lab
 *
 *  June 2016
 *------------------------------------------------------------------
 */

#include "in4073.h"

/*------------------------------------------------------------------
 * process_key -- process command keys
 *------------------------------------------------------------------
 */
void process_key(uint8_t c)
{
	switch (c)
	{
		case 'q':
			ae[0] += 10;
			break;
		case 'a':
			ae[0] -= 10;
			if (ae[0] < 0) ae[0] = 0;
			break;
		case 'w':
			ae[1] += 10;
			break;
		case 's':
			ae[1] -= 10;
			if (ae[1] < 0) ae[1] = 0;
			break;
		case 'e':
			ae[2] += 10;
			break;
		case 'd':
			ae[2] -= 10;
			if (ae[2] < 0) ae[2] = 0;
			break;
		case 'r':
			ae[3] += 10;
			break;
		case 'f':
			ae[3] -= 10;
			if (ae[3] < 0) ae[3] = 0;
			break;
		case 27:
			demo_done = true;
			break;
		default:
			nrf_gpio_pin_toggle(RED);
	}
}

void echo_key(uint8_t c)
{
	printf("%X", c);
}

void echo_message(uint8_t * message, uint8_t message_len)
{
	for(int i = 0; i < message_len; i++)
	{
		printf("%c", *(message++));
	}

	printf("\n");
}

void handle_message(uint8_t * buffer, uint8_t buffer_len)
{

	standard_message_t * message_ptr = (standard_message_t *) buffer;
	uint16_t crc_received, crc_calculated;

	echo_message((uint8_t *) &buffer, buffer_len);
	printf("buffer_len: %d, sizeof(message): %d\n", buffer_len, sizeof(standard_message_t));

	// if we receive more bytes than we expect, return
	if(buffer_len > sizeof(standard_message_t))
		return;

	// store the received crc
	crc_received = message_ptr->crc;
	
	// calculte the crc
	message_ptr->crc = 0;
	crc_calculated = crc_fast((unsigned char const*) message_ptr, sizeof(standard_message_t));

	printf("message type: 0x%X, crc_received: 0x%X, crc_calculated: 0x%X, data: %s\n", 
		message_ptr->message_type, crc_received, crc_calculated, message_ptr->data);
}

void parse_message(uint8_t c)
{
	static uint8_t msg_index = 0;
	static bool is_escaped = false;
	static uint8_t buffer[30];



	// TODO remove this - debug print received char
	printf("%X\n", c);

	// check for escaped bytes, discard escape bytes
	if (is_escaped == false && c == ESCAPE)
	{
		is_escaped = true;
		return;
	}

	// if this byte is escaped, reXOR it
	if (is_escaped == true)
	{
		c ^= 0x20;
		is_escaped = false;
	}

	// handle the incomming char
	switch (c)
	{
		case START_BYTE:
		{
			msg_index = 0; 
			break;
		}
		case END_BYTE:
		{
			handle_message(buffer, msg_index);
			msg_index = 0;
			break;
		}
		default:
		{
			if(msg_index >= 30)
			{
				msg_index = 0;
				return;
			}

			buffer[msg_index++] = c;
			break;
		}
	}
}

/*------------------------------------------------------------------
 * main -- everything you need is here :)
 *------------------------------------------------------------------
 */
int main(void)
{
	uart_init();
	gpio_init();
	timers_init();
	adc_init();
	twi_init();
	imu_init(true, 100);	
	baro_init();
	spi_flash_init();
	ble_init();
	crc_init();

	uint32_t counter = 0;
	demo_done = false;

	while (!demo_done)
	{
		// if (rx_queue.count) process_key( dequeue(&rx_queue) );

		if (rx_queue.count) parse_message( dequeue(&rx_queue) );

		if (check_timer_flag()) 
		{
			if (counter++%20 == 0) nrf_gpio_pin_toggle(BLUE);

			adc_request_sample();
			read_baro();

			// printf("%10ld | ", get_time_us());
			// printf("%3d %3d %3d %3d | ",ae[0],ae[1],ae[2],ae[3]);
			// printf("%6d %6d %6d | ", phi, theta, psi);
			// printf("%6d %6d %6d | ", sp, sq, sr);
			// printf("%4d | %4ld | %6ld \n", bat_volt, temperature, pressure);

			clear_timer_flag();
		}

		if (check_sensor_int_flag()) 
		{
			get_dmp_data();
			run_filters_and_control();
		}
	}	

	printf("\n\t Goodbye \n\n");
	nrf_delay_ms(100);

	NVIC_SystemReset();
}
