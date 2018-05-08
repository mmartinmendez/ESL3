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

void run_filters_and_control(input_data_t * data, uint8_t current_mode)
{
	// fancy stuff here
	// control loops and/or filters

	// ae[0] = xxx, ae[1] = yyy etc etc

	//manual mode
	

	int16_t liftdata = data->lift;
	int16_t rolldata = data->roll;
	int16_t pitchdata = data->pitch;
	int16_t yawdata = data->yaw;

	// TO-DO mapping from input to usable values
	// i.e rolldata from -50 -> 50,  [-50, 0] roll left, [0, 50] roll right
	// i.e pitchdata from -50 -> 50,  [-50, 0] pitch back, [0, 50] pitch forward
	// i.e yawdata from -50 -> 50,  [-50, 0] yaw left, [0, 50] yaw right 
	// all normalized based on liftdata

	ae[0] = liftdata + pitchdata - yawdata;
	if (ae[0] < 0) ae[0] = 0;
	ae[1] = liftdata - rolldata + yawdata;
	if (ae[1] < 0) ae[1] = 0;
	ae[2] = liftdata - pitchdata - yawdata;
	if (ae[2] < 0) ae[2] = 0;
	ae[3] = liftdata + rolldata + yawdata;
	if (ae[3] < 0) ae[3] = 0;

	update_motors();
}
