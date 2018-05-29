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

//Kalman parameters
int p = 0;
int p_b = 0;
int P2PHI = 268;
int C1_p = 1000;
int C2_p = 1000000;
int P1_p = 0;
int P2_p = 0;
int roll_setpoint = 0;

int t = 0;
int t_b = 0;
int P2THETA = 268;
int C1_t = 1000;
int C2_t = 1000000;
int P1_t = 0;
int P2_t = 0;
int pitch_setpoint = 0;

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

				printf("Panic mode is done, go to safe mode\n");
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

			if (liftdata == -127) {
				ae[0] =0;
				ae[1] =0;
				ae[2] =0;
				ae[3] =0;

				break;
			} 

			int lift_setpoint  = (liftdata + 127 * 2) * 2;

			int P = p_yaw_control; //should be adjustable by keyboard

			// compensate for calibration error
            int real_sr = sr - cal_sr;

            // TODO convert joystick yaw to rate (for now leave at 0)
            int rate_setpoint = 0; // this is determined by yaw rate of joystick

            int Eps = rate_setpoint - real_sr ; //can we use psi for this, or is it sr? 

            // cap Esp at +-7000
            if (Eps > 2000)
            {
            	Eps = 2000;	
            } 
            else if (Eps < -2000)
            {
            	Eps = -2000;
            }	

            // scale eps
            Eps = Eps / 250; // TODO replace this with fixed point

			ae[0] = lift_setpoint + P * Eps;
			ae[1] = lift_setpoint - P * Eps;
			ae[2] = lift_setpoint + P * Eps;
			ae[3] = lift_setpoint - P * Eps;

			for (int i = 0; i < 4; i++)
			{
				if (ae[i] < MIN_RPM) ae[i] = MIN_RPM;
				else if(ae[i] > MAX_RPM) ae[i] = MAX_RPM;
			}

			#if 0
			static int debug_print_counter = 0; //TODO remove this later

			if (debug_print_counter++%4 == 0)
			{
				printf("Motor values: %3d %3d %3d %3d |",ae[0],ae[1],ae[2],ae[3]);
				printf("Eps: %6d | real_sr: %6d| sr: %6d | cal_sr: %6d \n", Eps, real_sr, sr, cal_sr);
			}
			#endif

		break;
		}

		case FULL_CONTROL_MODE:
		{

		// Kalman for p, phi
		p = sp - p_b
		phi = phi + p * P2PHI
		phi = phi - (phi - say) / C1_p
		p_b = p_b + (phi - say) / C2_p
		// Use p, phi in P controller
		K_s_p = P1_p * (roll_setpoint - phi) - P2_p * p


		// Kalman for t, theta
		t = sq - t_b
		theta = theta + t * P2THETA
		theta = theta - (theta - sax) / C1_t
		t_b = t_b + (theta - sax) / C2_t
		// Use p, phi in P controller
		K_s_t = P1_t * (pitch_setpoint - theta) - P2_t * t



		break;
		}

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
