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

void send_mode_update(uint8_t mode)
{

}

void handle_message(uint8_t * buffer, uint8_t buffer_len)
{

	message_t * message_ptr = (message_t *) buffer;
	uint16_t crc_received, crc_calculated;

	echo_message((uint8_t *) &buffer, buffer_len);
	printf("buffer_len: %d ", buffer_len);

	// if we receive more bytes than we expect, return
	if(buffer_len > sizeof(message_t))
		return;

	// store the received crc
	crc_received = message_ptr->crc;
	
	// calculte the crc
	message_ptr->crc = 0;
	crc_calculated = crc_fast((unsigned char const*) message_ptr, buffer_len);

	printf("message type: 0x%X, crc_received: 0x%X, crc_calculated: 0x%X\n", 
		message_ptr->message_type, crc_received, crc_calculated);

	switch ((msg_type_e) message_ptr->message_type)
	{
		case set_mode:
		{
			set_mode_data_t * data = (set_mode_data_t*) &(message_ptr->data);
			printf("Received set mode command, mode: %d\n", data->mode);

			// Now send mode update back to the pc
			//send_mode_update(data->mode);
			break;
		}
		case input_data:
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

	uint32_t counter = 0;
	demo_done = false;

	while (!demo_done)
	{
		// if (rx_queue.count) process_key( dequeue(&rx_queue) );

		if (rx_queue.count)
		{
			message_len = parse_message(dequeue(&rx_queue), &msg_index, 
				&is_escaped, buffer);

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
			run_filters_and_control();
		}
	}	

	printf("\n\t Goodbye \n\n");
	nrf_delay_ms(100);

	NVIC_SystemReset();
}
