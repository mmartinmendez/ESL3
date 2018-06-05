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
#include <assert.h>

//#define MANUAL_MODE

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

	message_t send_buffer;

	// variables for input receive_buffer
	static uint8_t receive_buffer[sizeof(message_t)];
	static bool is_escaped = false;
	static uint8_t msg_index;

	uint8_t message_len = 0;
	uint8_t c;
	uint8_t retval = 0;
	
	bool demo_done = false;

	#if 0
	uint8_t data[15] = {0};
	uint32_t free_pointer = 0x000000;
	uint8_t buttons_123 = 0 ; //+ button_1*100 + button_2*10 + button_3*1 ;
	uint8_t buttons_456 = 0 ; //+ button_4*100 + button_5*10 + button_6*1 ;
	uint8_t buttons_789 = 0 ; //+ button_7*100 + button_8*10 + button_9*1 ;
	uint8_t buttons_101112 = 0 ; //+ button_10*100 + button_11*10 + button_12*1 ;
	uint8_t front_percent = 0;
	uint8_t side_percent = 0;
	uint8_t yaw_percent = 0;
	uint8_t power_percent = 0;
	uint8_t baro_percent = 0;
	uint8_t roll_x_percent = 0;
	uint8_t roll_y_percent = 0;
	uint8_t motor_1_percent = 0;
	uint8_t	motor_2_percent = 0; 
	uint8_t	motor_3_percent = 0;
	uint8_t	motor_4_percent = 0; 
	#endif

	uint32_t counter = 0;
	uint32_t bat_volt_counter = 0;
	uint32_t time_last_msg = get_time_us();

	adc_request_sample(); // request first battery sample

	// give p values initial value
	p_yaw_control = 0; // desired 15
	p1 = 0; // desired 3
	p2 = 0; // desired 15

	while (!demo_done)
	{

		if (check_sensor_int_flag()) 
		{
			get_dmp_data();
			run_filters_and_control(&send_buffer, bat_volt, &demo_done);
		}

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
				if (retval < 0x0F)
				{
					current_mode = retval;
				}

				time_last_msg = get_time_us();
			}	
		} 

		if (check_timer_flag()) 
		{
			if (counter++%20 == 0) 
			{
				nrf_gpio_pin_toggle(BLUE);
				
				if (bat_volt_counter++%8 == 0)
				{
					adc_request_sample();

					// not sure which outputs the correct battery level
					printf("bat_volt: %dV\n", bat_volt*1*3/255*2); 

					// TODO replace with correct bat_volt value
					if ((bat_volt*1*3/255*2) < 11)
					{
						current_mode = PANIC_MODE;
						printf("Battery value too low, go to panic mode\n");
					}
				}

				// read_baro();
			}

			// note to also check that time_last_char is smaller than now
			//if ((time_last_msg < now) &&
			//	((now - time_last_msg) > 2000000))
			uint32_t now = get_time_us();
			if ((now - time_last_msg) > 1000000)
			{
				// we did not receive any char for over 2 seconds -> panic
				// current_mode = PANIC_MODE;
				printf("We did not receive any char for over 2 seconds, "
					"go to panic mode (disabled now). now: %lu, "
					"time_last_msg: %lu\n", 
					now, time_last_msg);

				// TODO fix this
			}

			// printf("%10ld | ", get_time_us());
			//printf("Motor values: %3d %3d %3d %3d \n",ae[0],ae[1],ae[2],ae[3]);
			// printf("%6d %6d %6d | ", phi, theta, psi);
			//printf("%6d %6d %6d | ", sp, sq, sr);
			// printf("%4d | %4ld | %6ld \n", bat_volt, temperature, pressure);

			clear_timer_flag();
		}
	}	

	send_terminate(&send_buffer);
	printf("\n\t Goodbye \n\n");

	nrf_delay_ms(100);

	NVIC_SystemReset();
}
