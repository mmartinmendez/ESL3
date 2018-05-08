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

void echo_message(uint8_t * message, uint8_t message_len)
{
	for(int i = 0; i < message_len; i++)
	{
		printf("%c", *(message++));
	}

	printf("\n");
}

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

void send_mode_update(uint8_t mode)
{
	mode_update_t data;
	uint8_t message_len;

	data.mode = mode;
	message_len = build_message(MSG_MODE_UPDATE, (uint8_t *) &data, 
		sizeof(data), &send_buffer);

	if (message_len > 0)
	{
		send_message(&send_buffer, message_len);
	}
}

void handle_message(uint8_t * buffer, uint8_t buffer_len)
{
	static uint8_t reply_counter = 0; // TODO remove this
	message_t * message_ptr = (message_t *) buffer;

	switch ((msg_type_e) message_ptr->message_type)
	{
		case MSG_SET_MODE:
		{
			set_mode_data_t * data = (set_mode_data_t*) &(message_ptr->data);
			printf("Received set mode command, mode: %d\n", data->mode);

			// TODO remove reply counter
			reply_counter++;
			if (reply_counter == 3) 
			{
				// Now send mode update back to the pc
				send_mode_update(data->mode);
				reply_counter = 0;
			}

			break;
		}
		case MSG_INPUT_DATA:
		{
			input_data_t * data = (input_data_t*) &(message_ptr->data);
			printf("Received input command, lift: %d, roll: %d, pitch: %d, yaw: %d\n", 
				data->lift, data->roll, data->pitch, data->yaw);
			break;
		}

		default:
		{
			printf("Received unsupported msg_type: %d\n", message_ptr->message_type);
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

	// variables for input buffer
	static uint8_t buffer[sizeof(message_t)];
	static bool is_escaped = false;
	static uint8_t msg_index;

	uint8_t message_len = 0;
	uint8_t c;

	uint32_t counter = 0;
	demo_done = false;

	while (!demo_done)
	{
		// if (rx_queue.count) process_key( dequeue(&rx_queue) );

		if (rx_queue.count)
		{
			c = dequeue(&rx_queue);
			printf("%X\n", c); // DEBUG PRINT - REMOVE LATER

			message_len = parse_message(c, &msg_index, 
				&is_escaped, buffer, "DRONE");

			if (message_len > 0)
			{
				// we received an end-byte, now handle message
				handle_message(buffer, message_len); 		
			}	
		} 

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
			input_data_t data;
			memset(&data,0,sizeof(data));
		
			run_filters_and_control(&data);
		}
	}	

	printf("\n\t Goodbye \n\n");
	nrf_delay_ms(100);

	NVIC_SystemReset();
}
