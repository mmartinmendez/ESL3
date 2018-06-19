// Author: K. Flere
// Data logger

#include <spi_flash.c>

uint32_t free_pointer = 0x000000;
uint8_t data[15] = {0};

/*max value for uint8_t is 255
uint8_t buttons_123 = 0 + button_1*100 + button_2*10 + button_3*1 ;
uint8_t buttons_456 = 0 + button_4*100 + button_5*10 + button_6*1 ;
uint8_t buttons_789 = 0 + button_7*100 + button_8*10 + button_9*1 ;
uint8_t buttons_101112 = 0 + button_10*100 + button_11*10 + button_12*1 ;
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
*/

void log_data()
{

	data[0] = buttons_123;
	data[1] = buttons_456;
	data[2] = buttons_789;
	data[3] = buttons_101112;	
	data[4] = front_percent;
	data[5] = side_percent;
	data[6] = yaw_percent;
	data[7] = power_percent;
	data[8] = baro_percent;
	data[9] = roll_x_percent;
	data[10] = roll_y_percent;
	data[11] = motor_1_percent;
	data[12] = motor_2_percent;
	data[13] = motor_3_percent;
	data[14] = motor_4_percent;

	if(flash_write_bytes(free_pointer, data, 15) == true)
		{
		//printf("\nData logged to flash\n");
		}

	free_pointer = free_pointer + sizeof(data) + 1;

	//flash chip full so erase flash completely
	if (0x01FFFF <= free_pointer)
		{
		assert(flash_chip_erase() == true);
		free_pointer = 0x000000;
		printf("\nFlash erased\n");
		}

}

