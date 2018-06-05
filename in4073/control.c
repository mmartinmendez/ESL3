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

			if (!in_calibration_mode) 
      		{
				cal_phi = phi;		//roll x angle
				cal_theta = theta;	//pitch y angle
				cal_psi = psi;		//yaw z angle
				cal_sp = sp;		//x velocity
				cal_sq = sq;		//y velocity
				cal_sr = sr;		//z velocity
				cal_sax = sax;		//x accelleration
				cal_say = say;		//y accelleration
				cal_saz = saz;		//z accelleration

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
			int P = p_yaw_control;

			// compensate for calibration error
            int real_sr = sr - cal_sr;

            // cap Esp at +-2000
            if (real_sr > 2000)
            {
            	real_sr = 2000;	
            } 
            else if (real_sr < -2000)
            {
            	real_sr = -2000;
            }	

 			// this is determined by yaw rate of joystick
            int rate_setpoint = yawdata * 20;
            int Eps = rate_setpoint - real_sr ;

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
				printf("Eps: %6d | real_sr: %6d| sr: %6d | cal_sr: %6d | rate_setpoint: %6d \n", 
					Eps, real_sr, sr, cal_sr, rate_setpoint);
			}
			#endif

		break;
		}


		case FULL_CONTROL_MODE:
		{

			if (liftdata == -127) {
				ae[0] =0;
				ae[1] =0;
				ae[2] =0;
				ae[3] =0;

				break;
			}
			
			int lift_setpoint  = (liftdata + 127 * 2) * 2;

			// compensate for calibration error
            int real_sax = sax - cal_sax;
            //int real_say = say - cal_say;
            int real_sp = sp - cal_sp;
            //int real_sq = sq - cal_sq;

            int roll_s = 0; // TODO implement joystick mapping
            //int pitch_s = 0; // TODO implement joystick mapping

			int K_s_roll = p1 * (roll_s - real_sax) - p2 * real_sp;
			// int K_s_pitch = p1 * (pitch_s - real_say) - p2 * real_sq;

			static int debug_print_counter = 0; //TODO remove this later

			if (debug_print_counter++%4 == 0)
			{
				printf("Motor values: %3d %3d %3d %3d |",ae[0],ae[1],ae[2],ae[3]);
				printf("K_s_roll: %6d | real_sax: %6d| real_sp: %6d\n", 
					K_s_roll, real_sax, real_sp);
			}

			//To Do Scale K_s values
			// K_s_roll = K_s_roll / 250;
			// K_s_pitch = K_s_pitch / 250;
			
			//ae[0] = lift_setpoint + K_s_pitch; //+ P * Eps;
			ae[1] = lift_setpoint - K_s_roll;  //- P * Eps;
			//ae[2] = lift_setpoint - K_s_pitch; //+ P * Eps;
			ae[3] = lift_setpoint + K_s_roll;  //- P * Eps;

					
			for (int i = 0; i < 4; i++)
			{
				if (ae[i] < MIN_RPM) ae[i] = MIN_RPM;
				else if(ae[i] > MAX_RPM) ae[i] = MAX_RPM;
			}


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
