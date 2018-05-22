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
#include "math.h"

void update_motors(void)
{
	// motor[0] = ae[0];
	// motor[1] = ae[1];
	// motor[2] = ae[2];
	// motor[3] = ae[3];
}

void run_filters_and_control(uint8_t current_mode, uint16_t bat_volt)
{

	// int16_t cal_phi, cal_theta, cal_psi, cal_sp, cal_sq, cal_sr, cal_sax, cal_say, cal_saz;

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
			if(ae[0] > 254 &&  ae[1] > 254 && ae[2] > 254 && ae[3] > 254){

				while(ae[0] != 450 && ae[1] != 450 && ae[2] != 450 && ae[3] != 450){
	 				 if (ae[0] < 450)
	 				 {
	 				 	ae[0] += 1;
	 				 } else {
	 				 	ae[0] -= 1;
	 				 }
	 				 if (ae[1] < 450)
	 				 {
	 				 	ae[1] += 1;
	 				 } else {
	 				 	ae[1] -= 1;
	 				 }
	 				 if (ae[2] < 450)
	 				 {
	 				 	ae[2] += 1;
	 				 } else {
	 				 	ae[2] -= 1;
	 				 }
	 				 if (ae[3] < 450)
	 				 {
	 				 	ae[3] += 1;
	 				 } else {
	 				 	ae[3] -= 1;
	 				 }
				}

				ae[0] = 450;
				ae[1] = 450;
				ae[2] = 450;
				ae[3] = 450;
			}

			while(ae[0] > 0 && ae[1] > 0 && ae[2] > 0 && ae[3] > 0 )
			{
				ae[0] -= 1;
					if (ae[0] < 0) ae[0] = 0;
				ae[1] -= 1;
					if (ae[1] < 0) ae[1] = 0;
				ae[2] -= 1;
					if (ae[2] < 0) ae[2] = 0;
				ae[3] -= 1;
					if (ae[3] < 0) ae[3] = 0;
			}
	
		break;
		}

		case MANUAL_MODE:
		{
			//manual mode

			int16_t updated[4];

			if(liftdata == -127) {
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

				// ae[0] = (ae[0] > updated[0]) ? ae[0]+10 : updated[0];
				// ae[1] = (ae[1] > updated[1]) ? ae[1]+10 : updated[1];
				// ae[2] = (ae[2] > updated[2]) ? ae[2]+10 : updated[2];
				// ae[3] = (ae[3] > updated[3]) ? ae[3]+10 : updated[3];

				ae[0] = updated[0];
				ae[1] = updated[1];
				ae[2] = updated[2];
				ae[3] = updated[3];

				// Bounds check
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

				ae[0] = (liftdata + 127 * 2) * 2; //adjust to correct values from python script
				ae[1] = (liftdata + 127 * 2) * 2; //adjust to correct values from python script
				ae[2] = (liftdata + 127 * 2) * 2; //adjust to correct values from python script
				ae[3] = (liftdata + 127 * 2) * 2; //adjust to correct values from python script

				if (ae[0] < 254) ae[0] = 254;
				else if(ae[0] > 450) ae[0] = 450;
				if (ae[1] < 254) ae[1] = 254;
				else if(ae[1] > 450) ae[1] = 450;
				if (ae[2] < 254) ae[2] = 254;
				else if(ae[2] > 450) ae[2] = 450;
				if (ae[3] < 254) ae[3] = 254;
				else if(ae[3] > 450) ae[3] = 450;

			
			int control_factor = 10; //should be adjustable by keyboard

            int Desired_Yaw_Angle = floor(yawdata/180); // in degrees
            Desired_Yaw_Angle =  floor(Desired_Yaw_Angle/180 * M_PI); // in rad
            int real_sr = cal_sr - sr;
            int Eps = Desired_Yaw_Angle - real_sr ; //can we use psi for this, or is it sr? 

			while(1){

				if(Eps > 5){ //not sure if these values are anything near correct
					ae[0] = ae[0] - floor(1/control_factor * ae[0]);
					ae[1] = ae[1] + floor(1/control_factor * ae[1]);
					ae[2] = ae[2] - floor(1/control_factor * ae[2]);
					ae[3] = ae[3] + floor(1/control_factor * ae[3]);
				} 
				if(Eps < -5){ //not sure if these values are anything near correct
					ae[0] = ae[0] + floor(1/control_factor * ae[0]);
					ae[1] = ae[1] - floor(1/control_factor * ae[1]);
					ae[2] = ae[2] + floor(1/control_factor * ae[2]);
					ae[3] = ae[3] - floor(1/control_factor * ae[3]);
				}
				if (Eps > -5 &&  Eps < 5){ //not sure if these values are anything near correct
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

	//update_motors();
}
