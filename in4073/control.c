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

	// if(bat_volt < 10.5){
	// 	current_mode = PANIC_MODE;
	// }

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
			ae[0] = 450; //tweak value to smallest hover value
			ae[1] = 450;
			ae[2] = 450;
			ae[3] = 450;
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

			int16_t updated[4];

			// TO-DO mapping from input to usable values
			// i.e rolldata from -50 -> 50,  [-50, 0] roll left, [0, 50] roll right
			// i.e pitchdata from -50 -> 50,  [-50, 0] pitch back, [0, 50] pitch forward
			// i.e yawdata from -50 -> 50,  [-50, 0] yaw left, [0, 50] yaw right
			// all normalized based on liftdata

			if(liftdata == -127 && pitchdata == 0 && rolldata == 0 && yawdata == 0) {
				ae[0] =0;
				ae[1] =0;
				ae[2] =0;
				ae[3] =0;
			}
			else {
				updated[0] = (liftdata + pitchdata - yawdata + 127 * 2) * 2;
				updated[1] = (liftdata - rolldata + yawdata + 127 * 2) * 2;
				updated[2] = (liftdata - pitchdata - yawdata + 127 * 2) * 2;
				updated[3] = (liftdata + rolldata + yawdata + 127 * 2) * 2;

				ae[0] = (ae[0] > updated[0]) ? ae[0]+10 : updated[0];
				ae[1] = (ae[1] > updated[1]) ? ae[1]+10 : updated[1];
				ae[2] = (ae[2] > updated[2]) ? ae[2]+10 : updated[2];
				ae[3] = (ae[3] > updated[3]) ? ae[3]+10 : updated[3];

				if (ae[0] < 254) ae[0] = 254;
				else if(ae[0] > 750) ae[0] = 750;
				if (ae[1] < 254) ae[1] = 254;
				else if(ae[1] > 750) ae[1] = 750;
				if (ae[2] < 254) ae[2] = 254;
				else if(ae[2] > 750) ae[2] = 750;
				if (ae[3] < 254) ae[3] = 254;
				else if(ae[3] > 750) ae[3] = 750;
			}

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
