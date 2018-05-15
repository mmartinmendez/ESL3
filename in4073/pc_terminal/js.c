
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "joystick.h"

static int fd;
static struct js_event js;


#define JS_DEV	"/dev/input/js1"

void init_joystick ()
{
	if ((fd = open(JS_DEV, O_RDONLY)) < 0) {
		perror("jstest");
		exit(1);
	}

	/* non-blocking mode
	 */
	fcntl(fd, F_SETFL, O_NONBLOCK);
}

void read_joystick (int8_t axis_small[], int button[])
{
	
	/* check up on JS
	 */
	while (read(fd, &js, sizeof(struct js_event)) == 
	       			sizeof(struct js_event))  {

		/* register data
		 */
		// fprintf(stderr,".");
		switch(js.type & ~JS_EVENT_INIT) {
			case JS_EVENT_BUTTON:
				button[js.number] = js.value;
				break;
			case JS_EVENT_AXIS:
				axis_small[js.number] = js.value / 256;
				if (js.number == 3) axis_small[3] = - axis_small[3]; // invert lift
				break;
		}
	}

	// axis_small[0] = axis[0] / 256;
	// axis_small[1] = axis[1] / 256;
	// axis_small[2] = axis[2] / 256;
	// axis_small[3] = -axis[3] / 256; // invert lift

	if (errno != EAGAIN) {
		perror("\njs: error reading (EAGAIN)");
		exit (1);
	}

	#if 0
	unsigned int i;
	printf("\n");
	printf("%5d   ",t);
	for (i = 0; i < 6; i++) {
		printf("%6d ",axis[i]);
	}
	printf(" |  ");
	for (i = 0; i < 12; i++) {
		printf("%d ",button[i]);
	}
	#endif
}

bool is_joystick_zero()
{
	return (
		(axis_small[0] == 0) &&
		(axis_small[1] == 0) && 
		(axis_small[2] == 0) &&
		(axis_small[3] == -127));
}
