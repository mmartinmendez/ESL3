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
	flash_chip_erase();
	ble_init();
	crc_init();

	// variables for input receive_buffer
	static uint8_t receive_buffer[sizeof(message_t)];
	static bool is_escaped = false;
	static uint8_t msg_index;

	uint8_t message_len = 0;
	uint8_t c;
	uint8_t retval = 0;
	uint8_t current_mode = 0;

	uint32_t counter = 0;
	demo_done = false;

	while (!demo_done)
	{

		if (rx_queue.count)
		{
			c = dequeue(&rx_queue);

			message_len = parse_message(c, &msg_index, 
				&is_escaped, receive_buffer, "DRONE");

			if (message_len > 0)
			{
				// we received an end-byte, now handle message
				retval = handle_message(&send_buffer, receive_buffer, 
					message_len, &demo_done); 		
				if (retval != 0xFF)
				{
					current_mode = retval;
				}

				get_dmp_data();
				run_filters_and_control(current_mode, bat_volt);
			}	
		} 

		if (check_sensor_int_flag()) 
		{
			get_dmp_data();
			run_filters_and_control(current_mode, bat_volt);
		}

		if (check_timer_flag()) 
		{
			if (counter++%20 == 0) nrf_gpio_pin_toggle(BLUE);

			adc_request_sample();
			read_baro();
			uint32_t time = get_time_us();
			if (get_time_us() - time > 100000)
			{
			// printf("%10ld | ", get_time_us());
			printf("Motor values: %3d %3d %3d %3d \n",ae[0],ae[1],ae[2],ae[3]);
			//printf("%6d %6d %6d | ", phi, theta, psi);
			printf("cal_sr:%6d sr: %6d | ", cal_sr, sr);
			// printf("%4d | %4ld | %6ld \n", bat_volt, temperature, pressure);
			}
			

			clear_timer_flag();
		}


	}	

	printf("\n\t Goodbye \n\n");
	nrf_delay_ms(100);

	NVIC_SystemReset();
}
