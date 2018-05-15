/*------------------------------------------------------------------
 *  control.c -- here you can implement your control algorithm
 *		 and any motor clipping or whatever else
 *		 remember! motor input =  0-1000 : 125-250 us (OneShot125)
 *
 *  I. Protonotarios
 *  Embedded Software Lab
 *
 *  July 2016
 *------------------------------------------------------------------
 */

#include "in4073.h"

void update_motors(void)
{					
	motor[0] = ae[0];
	motor[1] = ae[1];
	motor[2] = ae[2];
	motor[3] = ae[3];
}

void run_filters_and_control(input_data_t * data, uint8_t current_mode, uint16_t bat_volt)
{

	int16_t cal_phi, cal_theta, cal_psi, cal_sp, cal_sq, cal_sr, cal_sax, cal_say, cal_saz;

	// fancy stuff here
	// control loops and/or filters

	// ae[0] = xxx, ae[1] = yyy etc etc

	if(bat_volt < 10.5){
		current_mode = PANIC_MODE;
	}

	switch (current_mode)
	{
		case SAFE_MODE:
		{
			ae[0] =0;
			ae[1] =0;
			ae[2] =0;
			ae[3] =0;

		break;
		}	

		case PANIC_MODE:
		{
			ae[0] = 200;
			ae[1] = 200;
			ae[2] = 200;
			ae[3] = 200;
			while(ae[0] > 0 && ae[1] > 0 && ae[2] > 0 && ae[3] > 0 )
			{
				ae[0] -= 20;
					if (ae[0] < 0) ae[0] = 0;
				ae[1] -= 20;
					if (ae[1] < 0) ae[1] = 0;
				ae[2] -= 20; 
					if (ae[2] < 0) ae[2] = 0;
				ae[3] -= 20;
					if (ae[3] < 0) ae[3] = 0;
			}	
		break;
		}

		case MANUAL_MODE: 
		{
			//manual mode
			int8_t liftdata = data->lift;
			int8_t rolldata = data->roll;
			int8_t pitchdata = data->pitch;
			int8_t yawdata = data->yaw;
			int factor = 1;

			// TO-DO mapping from input to usable values
			// i.e rolldata from -50 -> 50,  [-50, 0] roll left, [0, 50] roll right
			// i.e pitchdata from -50 -> 50,  [-50, 0] pitch back, [0, 50] pitch forward
			// i.e yawdata from -50 -> 50,  [-50, 0] yaw left, [0, 50] yaw right 
			// all normalized based on liftdata

			ae[0] = 500 + floor(factor *(liftdata + pitchdata - yawdata));
			if (ae[0] < 0) ae[0] = 0;
			ae[1] = 500 + floor(factor *(liftdata - rolldata + yawdata));
			if (ae[1] < 0) ae[1] = 0;
			ae[2] = 500 + floor(factor *(liftdata - pitchdata - yawdata));
			if (ae[2] < 0) ae[2] = 0;
			ae[3] = 500 + floor(factor *(liftdata + rolldata + yawdata));
			if (ae[3] < 0) ae[3] = 0;
		
		break;
		}

		case CALIBRATION_MODE:
		{
			cal_phi = phi;
			cal_theta = theta;
			cal_psi = psi;
			cal_sp = sp; 
			cal_sq = sq;
			cal_sr = sr;
			cal_sax = sax;
			cal_say = say;
			cal_saz = saz;

			send_calibration_data(&send_buffer, cal_phi,cal_theta,cal_psi,
				cal_sp, cal_sq, cal_sr, cal_sax, cal_say, cal_saz);
		break;
		}

		case YAW_CONTROL_MODE:
		{
			int8_t liftdata = data->lift;
			int8_t yawdata = data->yaw;

			ae[0] = liftdata;
			ae[1] = liftdata;
			ae[2] = liftdata;
			ae[3] = liftdata;
			int control_factor = 1; //not sure if these values are anything near correct
			while(1){ 
				if(yawdata - sr > 10){ //not sure if these values are anything near correct
					ae[0] = ae[0] - control_factor * ae[0];
					ae[1] = ae[1] + control_factor * ae[1]; 
					ae[2] = ae[2] - control_factor * ae[2];
					ae[3] = ae[3] + control_factor * ae[3]; 

				}

				if(yawdata - sr < -10){ //not sure if these values are anything near correct
					ae[0] = ae[0] + control_factor * ae[0];
					ae[1] = ae[1] - control_factor * ae[1]; 
					ae[2] = ae[2] + control_factor * ae[2];
					ae[3] = ae[3] - control_factor * ae[3]; 
				}
				if (yawdata - sr > -10 &&  yawdata - sr < 10){ //not sure if these values are anything near correct
					ae[0] = ae[0];
					ae[1] = ae[1];
					ae[2] = ae[2];
					ae[3] = ae[3];
					break; 
				}
			}
		break;
		}

		case FULL_CONTROL_MODE:
		break;

		case RAW_MODE:
		break;

		case HEIGHT_CONTROL_MODE:
		break;

		case WIRELESS_MODE:
		break;

		default:
		printf("Not a correct mode");
	}

	update_motors();
}
