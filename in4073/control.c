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

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

void update_motors(void)
{
	motor[0] = MIN(ae[0],1000);
	motor[1] = MIN(ae[1],1000);
	motor[2] = MIN(ae[2],1000);
	motor[3] = MIN(ae[3],1000);
}

void run_filters_and_control(message_t * send_buffer, uint16_t bat_volt, bool * demo_done)
{
	static bool in_panic_mode = false;
	static bool in_calibration_mode = false;
	static bool exit_in_safe_mode = false;

	static int setpoint = 0;

	static int debug_print_counter = 0; //TODO remove this later
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

			if (exit_in_safe_mode)
			{
				*demo_done = true;
			}
			break;
		}

		case PANIC_MODE:
		{
			if (!in_panic_mode)
			{
				// create setpoint
				setpoint = (ae[0] + ae[1] + ae[2] + ae [3]) / 4;

				in_panic_mode = true;
			}

			for (uint8_t i = 0; i < 4; i++)
			{
				if (ae[i] < setpoint)
 				{
 				 	ae[i] += MIN(setpoint - ae[i], PANIC_MODE_STEP_SIZE);
 				} else if (ae[i] > setpoint){
 					ae[i] -= MIN(ae[i] - setpoint, PANIC_MODE_STEP_SIZE);
 				}
			}
			
			setpoint = MAX(0, setpoint - PANIC_MODE_STEP_SIZE);


			if (ae[0] == 0 && ae[1] == 0 &&	ae[2] == 0 && ae[3] == 0)
			{
				// we are done with the panic mode, go to safe mode now
				in_panic_mode = false;
				current_mode = SAFE_MODE;
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
			if (!in_calibration_mode)
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

				send_calibration_data(send_buffer, cal_phi,cal_theta,cal_psi,
					cal_sp, cal_sq, cal_sr, cal_sax, cal_say, cal_saz);

				in_calibration_mode = true;
			}

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

			
			int P = p_yaw_control; //should be adjustable by keyboard

			// compensate for calibration error
            int real_sr = sr - cal_sr;

            // TODO convert joystick yaw to rate (for now leave at 0)
            int rate_setpoint = 0; // this is determined by yaw rate of joystick

            int Eps = rate_setpoint - real_sr ; //can we use psi for this, or is it sr? 

			if((Eps < -100) || (Eps > 100)){ // do not change on small eps
				ae[0] = ae[0] - P * Eps;
				ae[1] = ae[1] + P * Eps;
				ae[2] = ae[2] - P * Eps;
				ae[3] = ae[3] + P * Eps;
			} 

			if (debug_print_counter++%4 == 0)
			{
				printf("Motor values: %3d %3d %3d %3d |",ae[0],ae[1],ae[2],ae[3]);
				printf("Eps: %6d | real_sr: %6d| sr: %6d | cal_sr: %6d \n", Eps, real_sr, sr, cal_sr);
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

		case TERMINATE_MODE:
		{
			// Termination sequence: terminate->panic->safe->exit
			current_mode = PANIC_MODE;
			exit_in_safe_mode = true;
			break;
		}


		default:
		printf("Not a correct mode");
	}

	// update in_state booleans
	if(current_mode != PANIC_MODE) in_panic_mode = false;
	if(current_mode != CALIBRATION_MODE) in_calibration_mode = false;

	update_motors();
}
