#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <termios.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>
#include "joystick.h"
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>

GtkWidget *bar_power;
GtkWidget *bar_front;
GtkWidget *bar_back;
GtkWidget *bar_left;
GtkWidget *bar_right;
GtkWidget *bar_yaw_left;
GtkWidget *bar_yaw_right;

GtkWidget *label_1;
GtkWidget *label_2;
GtkWidget *label_3;
GtkWidget *label_4;
GtkWidget *label_5;
GtkWidget *label_6;
GtkWidget *label_7;
GtkWidget *label_8;
GtkWidget *label_9;
GtkWidget *label_10;
GtkWidget *label_11;
GtkWidget *label_12;

int	axis[6];
int	button[12];

unsigned int    mon_time_ms(void)
{
        unsigned int    ms;
        struct timeval  tv;
        struct timezone tz;

        gettimeofday(&tv, &tz);
        ms = 1000 * (tv.tv_sec % 65); // 65 sec wrap around
        ms = ms + tv.tv_usec / 1000;
        return ms;
}

void    mon_delay_ms(unsigned int ms)
{
        struct timespec req, rem;

        req.tv_sec = ms / 1000;
        req.tv_nsec = 1000000 * (ms % 1000);
        assert(nanosleep(&req,&rem) == 0);
}

#define JS_DEV	"/dev/input/js1"
//reading joystick values
void *jsfunc(void *para)
//void update_gui (int8_t axis[], int button[])
{

	static int buttoncheck_1 = 0;
	static int buttoncheck_2 = 0;
	static int buttoncheck_3 = 0;
	static int buttoncheck_4 = 0;
	static int buttoncheck_5 = 0;
	static int buttoncheck_6 = 0;
	static int buttoncheck_7 = 0;
	static int buttoncheck_8 = 0;
	static int buttoncheck_9 = 0;
	static int buttoncheck_10 = 0;
	static int buttoncheck_11 = 0;
	static int buttoncheck_12 = 0;
	float horizontal = 0;
	float vertical = 0;
	float power = 0;
	float yaw = 0;
	float maxvalue = 32767;
	float doublemaxvalue = 65534;

	int 		fd;
	struct js_event js;
	//unsigned int	t;

	if ((fd = open(JS_DEV, O_RDONLY)) < 0) 
		{
		perror("Please connect the joystick\n");
		exit(1);
		} 

	/* non-blocking mode
	 */
	fcntl(fd, F_SETFL, O_NONBLOCK);

	while (1) 
		{


		/* simulate work
		 */
		mon_delay_ms(20);
		//t = mon_time_ms();

		/* check up on JS
		 */
		while (read(fd, &js, sizeof(struct js_event)) == 
		       			sizeof(struct js_event))  
			{

			/* register data
			 */
			// fprintf(stderr,".");
			switch(js.type & ~JS_EVENT_INIT) 
				{
				case JS_EVENT_BUTTON:
					button[js.number] = js.value;
					break;
				case JS_EVENT_AXIS:
					axis[js.number] = js.value;
					break;
				}
			}
		if (errno != EAGAIN) 
			{
			perror("\njs: error reading (EAGAIN)");
			exit (1);
			}
/*
		printf("%i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i \n",t , axis[0],axis[1],axis[2],axis[3],axis[4],axis[5],button[0] ,button[1] ,button[2] ,button[3] ,button[4] ,button[5] ,button[6] ,button[7] ,button[8] ,button[9] ,button[10] ,button[11] );

*/
	if (button[0] != buttoncheck_1)
	{
		if (button[0] == 0)
		{
			gtk_label_set_text( GTK_LABEL(label_1),"  Button 1  \n      OFF");
			buttoncheck_1 = button[0] ;
		}
		else
		{
			gtk_label_set_text( GTK_LABEL(label_1),"  Button 1  \n      ON");
			buttoncheck_1 = button[0] ;
		}
	}

	if (button[1] != buttoncheck_2)
	{
		if (button[1] == 0)
		{
		gtk_label_set_text( GTK_LABEL(label_2),"  Button 2  \n      OFF");
		buttoncheck_2 = button[1] ;
		}
	else
		{
		gtk_label_set_text( GTK_LABEL(label_2),"  Button 2  \n      ON");
		buttoncheck_2 = button[1] ;
		}
	}

	if (button[2] != buttoncheck_3)
		{
		if (button[2] == 0)
			{
			gtk_label_set_text( GTK_LABEL(label_3),"  Button 3  \n      OFF");
			buttoncheck_3 = button[2] ;
			}
		else
			{
			gtk_label_set_text( GTK_LABEL(label_3),"  Button 3  \n      ON");
			buttoncheck_3 = button[2] ;
			}
		}

	if (button[3] != buttoncheck_4)
		{
		if (button[3] == 0)
			{
			gtk_label_set_text( GTK_LABEL(label_4),"  Button 4  \n      OFF");
			buttoncheck_4 = button[3] ;
			}
		else
			{
			gtk_label_set_text( GTK_LABEL(label_4),"  Button 4  \n      ON");
			buttoncheck_4 = button[3] ;
			}
		}

	if (button[4] != buttoncheck_5)
		{
		if (button[4] == 0)
			{
			gtk_label_set_text( GTK_LABEL(label_5),"  Button 5  \n      OFF");
			buttoncheck_5 = button[4] ;
			}
		else
			{
			gtk_label_set_text( GTK_LABEL(label_5),"  Button 5  \n      ON");
			buttoncheck_5 = button[4] ;
			}
		}

	if (button[5] != buttoncheck_6)
		{
		if (button[5] == 0)
			{
			gtk_label_set_text( GTK_LABEL(label_6),"  Button 6  \n      OFF");
			buttoncheck_6 = button[5] ;
			}
		else
			{
			gtk_label_set_text( GTK_LABEL(label_6),"  Button 6  \n      ON");
			buttoncheck_6 = button[5] ;
			}
		}

	if (button[6] != buttoncheck_7)
		{
		if (button[6] == 0)
			{
			gtk_label_set_text( GTK_LABEL(label_7),"  Button 7  \n      OFF");
			buttoncheck_7 = button[6] ;
			}
		else
			{
			gtk_label_set_text( GTK_LABEL(label_7),"  Button 7  \n      ON");
			buttoncheck_7 = button[6] ;
			}
		}

	if (button[7] != buttoncheck_8)
		{
		if (button[7] == 0)
			{
			gtk_label_set_text( GTK_LABEL(label_8),"  Button 8  \n      OFF");
			buttoncheck_8 = button[7] ;
			}
		else
			{
			gtk_label_set_text( GTK_LABEL(label_8),"  Button 8  \n      ON");
			buttoncheck_8 = button[7] ;
			}
		}

	if (button[8] != buttoncheck_9)
		{
		if (button[8] == 0)
			{
			gtk_label_set_text( GTK_LABEL(label_9),"  Button 9  \n      OFF");
			buttoncheck_9 = button[8] ;
			}
		else
			{
			gtk_label_set_text( GTK_LABEL(label_9),"  Button 9  \n      ON");
			buttoncheck_9 = button[8] ;
			}
		}

	if (button[9] != buttoncheck_10)
		{
		if (button[9] == 0)
			{
			gtk_label_set_text( GTK_LABEL(label_10),"  Button 10  \n      OFF");
			buttoncheck_10 = button[9] ;
			}
		else
			{
			gtk_label_set_text( GTK_LABEL(label_10),"  Button 10  \n      ON");
			buttoncheck_10 = button[9] ;
			}
		}

	if (button[10] != buttoncheck_11)
		{
		if (button[10] == 0)
			{
			gtk_label_set_text( GTK_LABEL(label_11),"  Button 11  \n      OFF");
			buttoncheck_11 = button[10] ;
			}
		else
			{
			gtk_label_set_text( GTK_LABEL(label_11),"  Button 11  \n      ON");
			buttoncheck_11 = button[10] ;
			}
		}

	if (button[11] != buttoncheck_12)
		{
		if (button[11] == 0)
			{
			gtk_label_set_text( GTK_LABEL(label_12),"  Button 12  \n      OFF");
			buttoncheck_12 = button[11] ;
			}
		else
			{
			gtk_label_set_text( GTK_LABEL(label_12),"  Button 12  \n      ON");
			buttoncheck_12 = button[11] ;
			}
		}

	horizontal = axis[0]/maxvalue;
	vertical = 0 - axis[1]/maxvalue;
	power = (32767-axis[3])/doublemaxvalue;
	yaw = axis[2]/maxvalue;

	if (horizontal <= 0)
		{
		gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (bar_left), 0 - horizontal);
		usleep(20000);
		gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (bar_right), 0);
		usleep(20000);
		}
	if (horizontal > 0)
		{
		gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (bar_left), 0);
		usleep(20000);
		gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (bar_right), horizontal);
		usleep(20000);
		}
	if (vertical <= 0)
		{
		gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (bar_back), 0 - vertical);
		usleep(20000);
		gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (bar_front), 0);
		usleep(20000);
		}
	if (vertical > 0)
		{
		gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (bar_back), 0);
		usleep(20000);
		gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (bar_front), vertical);
		usleep(20000);
		}
	if (yaw <= 0)
		{
		gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (bar_yaw_left), 0 - yaw);
		usleep(20000);
		gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (bar_yaw_right), 0);
		usleep(20000);
		}
	if (yaw > 0)
		{
		gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (bar_yaw_left), 0);
		usleep(20000);
		gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (bar_yaw_right), yaw);
		usleep(20000);
		}

	gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (bar_power), power);
	usleep(20000);
}
}


static gboolean delete_event(GtkWidget *widget, GdkEvent *event, gpointer data)
	{

    	g_print ("delete event occurred\n");
	gtk_main_quit ();

    	// Change TRUE to FALSE and the main window will be destroyed with
     	// a "delete-event". 

    	return TRUE;
	}

static void destroy(GtkWidget *widget, gpointer data)
	{
    	gtk_main_quit ();
	}


void *run_gui(void *para)
{
    	GtkWidget *window;
	GtkWidget *button;
	GtkWidget *grid;

	//initial values
	float front = 0;
	float back = 0;
	float left = 0;
	float right = 0;
	float yaw_left = 0.8;
	float yaw_right = 0.8;


	//pthread_t input_check;
	pthread_t js_check;

	//int xyz = 0;

	if(pthread_create (&js_check, NULL, jsfunc, NULL))
	{	
   		perror("ERROR creating jsfunc thread.");
	}

	
   	// gtk_init (&argc, &argv);
   	gtk_init (NULL, NULL);

	int vposition = 1;
	int hposition = 1;

	//create window
    	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);	
	gtk_window_set_title(GTK_WINDOW(window), "Window");
	gtk_widget_set_size_request (window, 500, 300);
	gtk_container_set_border_width(GTK_CONTAINER (window), 10);

	//connect signal of window manager with GTK
    	g_signal_connect (window, "delete-event", G_CALLBACK (delete_event), NULL);
	g_signal_connect (window, "destroy", G_CALLBACK (destroy), NULL);

	//create new grid
	grid = gtk_grid_new ();
	gtk_container_add (GTK_CONTAINER (window), grid);
	gtk_grid_set_row_spacing (GTK_GRID(grid), 0);
	gtk_grid_set_column_spacing (GTK_GRID(grid), 0);

	//create progress bar_front
	bar_front = gtk_progress_bar_new();
	gtk_orientable_set_orientation (GTK_ORIENTABLE (bar_front), GTK_ORIENTATION_VERTICAL);
	gtk_progress_bar_set_inverted (GTK_PROGRESS_BAR(bar_front), TRUE);
	//gtk_widget_set_size_request (bar_front, 50, 40);
	gtk_grid_attach (GTK_GRID (grid), bar_front, vposition + 1, hposition + 0, 1, 1);
	gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (bar_front), front);
	//gtk_progress_bar_set_text (GTK_PROGRESS_BAR (bar_front), "front");
	//gtk_progress_bar_set_show_text (GTK_PROGRESS_BAR (bar_front), TRUE);

	//create progress bar_back
	bar_back = gtk_progress_bar_new();
	gtk_orientable_set_orientation (GTK_ORIENTABLE (bar_back), GTK_ORIENTATION_VERTICAL);
	//gtk_widget_set_size_request (bar_back, 50, 40);
	gtk_grid_attach (GTK_GRID (grid), bar_back, vposition + 1, hposition + 2, 1, 1);
	gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (bar_back), back);

	//create progress bar_left
	bar_left = gtk_progress_bar_new();
	gtk_progress_bar_set_inverted (GTK_PROGRESS_BAR(bar_left), TRUE);
	//gtk_widget_set_size_request (bar_left, 50, 40);
	gtk_grid_attach (GTK_GRID (grid), bar_left, vposition + 0, hposition + 1, 1, 1);
	gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (bar_left), left);

	//create progress bar_right
	bar_right = gtk_progress_bar_new();
	//gtk_widget_set_size_request (bar_right, 50, 40);
	gtk_grid_attach (GTK_GRID (grid), bar_right, vposition + 2, hposition + 1, 1, 1);
	gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (bar_right), right);

	//create progress bar_power
	bar_power = gtk_progress_bar_new();
	gtk_orientable_set_orientation (GTK_ORIENTABLE (bar_power), GTK_ORIENTATION_VERTICAL);
	gtk_progress_bar_set_inverted (GTK_PROGRESS_BAR(bar_power), TRUE);
	//gtk_widget_set_size_request (bar_right, 50, 40);
	gtk_grid_attach (GTK_GRID (grid), bar_power, vposition + 3, hposition + 0, 1, 3);
	gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (bar_power), 0);

	//create progress bar_yaw_left
	bar_yaw_left = gtk_progress_bar_new();
	gtk_progress_bar_set_inverted (GTK_PROGRESS_BAR(bar_yaw_left), TRUE);
	//gtk_widget_set_size_request (bar_left, 50, 40);
	gtk_grid_attach (GTK_GRID (grid), bar_yaw_left, vposition + 0, hposition + 3, 1, 1);
	gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (bar_yaw_left), yaw_left);

	//create progress bar_yaw_right
	bar_yaw_right = gtk_progress_bar_new();
	//gtk_widget_set_size_request (bar_right, 50, 40);
	gtk_grid_attach (GTK_GRID (grid), bar_yaw_right, vposition + 2, hposition + 3, 1, 1);
	gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (bar_yaw_right), yaw_right);

	#if 0

	GtkWidget *bar_1;
	GtkWidget *bar_2;
	GtkWidget *bar_3;
	GtkWidget *bar_4;

	//create progress bar_1
	bar_1 = gtk_progress_bar_new();
	gtk_orientable_set_orientation (GTK_ORIENTABLE (bar_1), GTK_ORIENTATION_VERTICAL);
	gtk_progress_bar_set_inverted (GTK_PROGRESS_BAR(bar_1), TRUE);
	//gtk_widget_set_size_request (bar_front, 50, 40);
	gtk_grid_attach (GTK_GRID (grid), bar_1, vposition + 0, hposition + 5, 1, 1);
	gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (bar_1), 0.3);
	gtk_progress_bar_set_text (GTK_PROGRESS_BAR (bar_1), "Motor 1");
	gtk_progress_bar_set_show_text (GTK_PROGRESS_BAR (bar_1), TRUE);

	//create progress bar_2
	bar_2 = gtk_progress_bar_new();
	gtk_orientable_set_orientation (GTK_ORIENTABLE (bar_2), GTK_ORIENTATION_VERTICAL);
	gtk_progress_bar_set_inverted (GTK_PROGRESS_BAR(bar_2), TRUE);
	//gtk_widget_set_size_request (bar_front, 50, 40);
	gtk_grid_attach (GTK_GRID (grid), bar_2, vposition + 3, hposition + 5, 1, 1);
	gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (bar_2), 0.3);
	gtk_progress_bar_set_text (GTK_PROGRESS_BAR (bar_2), "Motor 2");
	gtk_progress_bar_set_show_text (GTK_PROGRESS_BAR (bar_2), TRUE);

	//create progress bar_3
	bar_3 = gtk_progress_bar_new();
	gtk_orientable_set_orientation (GTK_ORIENTABLE (bar_3), GTK_ORIENTATION_VERTICAL);
	gtk_progress_bar_set_inverted (GTK_PROGRESS_BAR(bar_3), TRUE);
	//gtk_widget_set_size_request (bar_front, 50, 40);
	gtk_grid_attach (GTK_GRID (grid), bar_3, vposition + 0, hposition + 6, 1, 1);
	gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (bar_3), 0.3);
	gtk_progress_bar_set_text (GTK_PROGRESS_BAR (bar_3), "Motor 3");
	gtk_progress_bar_set_show_text (GTK_PROGRESS_BAR (bar_3), TRUE);

	//create progress bar_4
	bar_4 = gtk_progress_bar_new();
	gtk_orientable_set_orientation (GTK_ORIENTABLE (bar_4), GTK_ORIENTATION_VERTICAL);
	gtk_progress_bar_set_inverted (GTK_PROGRESS_BAR(bar_4), TRUE);
	//gtk_widget_set_size_request (bar_front, 50, 40);
	gtk_grid_attach (GTK_GRID (grid), bar_4, vposition + 3, hposition + 6, 1, 1);
	gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (bar_4), 0.3);
	gtk_progress_bar_set_text (GTK_PROGRESS_BAR (bar_4), "Motor 4");
	gtk_progress_bar_set_show_text (GTK_PROGRESS_BAR (bar_4), TRUE);

	#endif

	//create button
  	button = gtk_button_new_with_label ("Quit");
  	g_signal_connect_swapped (button, "clicked", G_CALLBACK (gtk_widget_destroy), window);
	gtk_grid_attach (GTK_GRID (grid), button, vposition + 4, hposition + 2, 1, 1);

	label_1 = gtk_label_new ("  Button 1  \n      OFF");
	label_2 = gtk_label_new ("  Button 2  \n      OFF");
	label_3 = gtk_label_new ("  Button 3  \n      OFF");
	label_4 = gtk_label_new ("  Button 4  \n      OFF");
	label_5 = gtk_label_new ("  Button 5  \n      OFF");
	label_6 = gtk_label_new ("  Button 6  \n      OFF");
	label_7 = gtk_label_new ("  Button 7  \n      OFF");
	label_8 = gtk_label_new ("  Button 8  \n      OFF");
	label_9 = gtk_label_new ("  Button 9  \n      OFF");
	label_10 = gtk_label_new ("  Button 10  \n      OFF");
	label_11 = gtk_label_new ("  Button 11  \n      OFF");
	label_12 = gtk_label_new ("  Button 12  \n     OFF");

	gtk_grid_attach (GTK_GRID (grid), label_1, vposition + 5, hposition + 0, 1, 1);
	gtk_grid_attach (GTK_GRID (grid), label_2, vposition + 5, hposition + 1, 1, 1);
	gtk_grid_attach (GTK_GRID (grid), label_3, vposition + 5, hposition + 2, 1, 1);
	gtk_grid_attach (GTK_GRID (grid), label_4, vposition + 5, hposition + 3, 1, 1);
	gtk_grid_attach (GTK_GRID (grid), label_5, vposition + 6, hposition + 0, 1, 1);
	gtk_grid_attach (GTK_GRID (grid), label_6, vposition + 6, hposition + 1, 1, 1);
	gtk_grid_attach (GTK_GRID (grid), label_7, vposition + 6, hposition + 2, 1, 1);
	gtk_grid_attach (GTK_GRID (grid), label_8, vposition + 6, hposition + 3, 1, 1);
	gtk_grid_attach (GTK_GRID (grid), label_9, vposition + 7, hposition + 0, 1, 1);
	gtk_grid_attach (GTK_GRID (grid), label_10, vposition + 7, hposition + 1, 1, 1);
	gtk_grid_attach (GTK_GRID (grid), label_11, vposition + 7, hposition + 2, 1, 1);
	gtk_grid_attach (GTK_GRID (grid), label_12, vposition + 7, hposition + 3, 1, 1);

	//show everything in window
	gtk_widget_show_all (window);
    
    	gtk_main ();
	
    	return 0;
	}





