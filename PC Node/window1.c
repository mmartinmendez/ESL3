#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <termios.h>
#include <stdio.h>

#define SIDE_CENTER_VALUE 1000
#define SIDE_MAX_OFFSET 1000

#define FRONT_CENTER_VALUE 1000
#define FRONT_MAX_OFFSET 1000

#define YAW_CENTER_VALUE 1000
#define YAW_MAX_OFFSET 1000

#define POWER_CENTER_VALUE 1000
#define POWER_MAX_OFFSET 1000

GtkWidget *bar_power;
GtkWidget *bar_front;
GtkWidget *bar_back;
GtkWidget *bar_left;
GtkWidget *bar_right;

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

//code for reading character without enter ************************************************************************************

static struct termios old, new;

/* Initialize new terminal i/o settings */
void initTermios(int echo) 
{
  tcgetattr(0, &old); /* grab old terminal i/o settings */
  new = old; /* make new settings same as old settings */
  new.c_lflag &= ~ICANON; /* disable buffered i/o */
  if (echo) {
      new.c_lflag |= ECHO; /* set echo mode */
  } else {
      new.c_lflag &= ~ECHO; /* set no echo mode */
  }
  tcsetattr(0, TCSANOW, &new); /* use these new terminal i/o settings now */
}

/* Restore old terminal i/o settings */
void resetTermios(void) 
{
  tcsetattr(0, TCSANOW, &old);
}

/* Read 1 character - echo defines echo mode */
char getch_(int echo) 
{
  char ch;
  initTermios(echo);
  ch = getchar();
  resetTermios();
  return ch;
}

/* Read 1 character without echo */
char getch(void) 
{
  return getch_(0);
}

/* Read 1 character with echo */
char getche(void) 
{
  return getch_(1);
}


//**********************************************************************************


void *func(void *param)
{
char scanner[50] = {0};
char buffer[1] = {0};
float horizontal = 0;
float vertical = 0;
float power = 0;
float maxvalue = 32767;
float doublemaxvalue = 65534;
int jsfront = 0;
int jsside = 0;
int jsyaw = 0;
int jspower = 0;
int jsthumb_1 = 0;
int jsthumb_2 = 0;
int button_1 = 0;
int button_2 = 0;
int button_3 = 0;
int button_4 = 0;
int button_5 = 0;
int button_6 = 0;
int button_7 = 0;
int button_8 = 0;
int button_9 = 0;
int button_10 = 0;
int button_11 = 0;
int button_12 = 0;

int buttoncheck_1 = 0;
int buttoncheck_2 = 0;
int buttoncheck_3 = 0;
int buttoncheck_4 = 0;
int buttoncheck_5 = 0;
int buttoncheck_6 = 0;
int buttoncheck_7 = 0;
int buttoncheck_8 = 0;
int buttoncheck_9 = 0;
int buttoncheck_10 = 0;
int buttoncheck_11 = 0;
int buttoncheck_12 = 0;


//char **arguments = (char**)param;

while (1)
{
fgets(scanner, sizeof(scanner) , stdin);
sscanf(scanner , "%*i %i %i %i %i %i %i %*s %i %i %i %i %i %i %i %i %i %i %i %i \n", &jsside, &jsfront, &jsyaw, &jspower, &jsthumb_1, &jsthumb_2, &button_1, &button_2, &button_3, &button_4, &button_5, &button_6, &button_7, &button_8, &button_9, &button_10, &button_11, &button_12 );

//buffer[0] = getch();
//buffer[0] = getchar();

printf("%i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i \n", jsside, jsfront, jsyaw, jspower, jsthumb_1, jsthumb_2, button_1, button_2, button_3, button_4, button_5, button_6, button_7, button_8, button_9, button_10, button_11, button_12 );


if (button_1 != buttoncheck_1)
	{
	if (button_1 == 0)
		{
		gtk_label_set_text( GTK_LABEL(label_1),"  Button 1  \n      OFF");
		buttoncheck_1 = button_1 ;
		}
	else
		{
		gtk_label_set_text( GTK_LABEL(label_1),"  Button 1  \n      ON");
		buttoncheck_1 = button_1 ;
		}
	}

if (button_2 != buttoncheck_2)
	{
	if (button_2 == 0)
		{
		gtk_label_set_text( GTK_LABEL(label_2),"  Button 2  \n      OFF");
		buttoncheck_2 = button_2 ;
		}
	else
		{
		gtk_label_set_text( GTK_LABEL(label_2),"  Button 2  \n      ON");
		buttoncheck_2 = button_2 ;
		}
	}

if (button_3 != buttoncheck_3)
	{
	if (button_3 == 0)
		{
		gtk_label_set_text( GTK_LABEL(label_3),"  Button 3  \n      OFF");
		buttoncheck_3 = button_3 ;
		}
	else
		{
		gtk_label_set_text( GTK_LABEL(label_3),"  Button 3  \n      ON");
		buttoncheck_3 = button_3 ;
		}
	}

if (button_4 != buttoncheck_4)
	{
	if (button_4 == 0)
		{
		gtk_label_set_text( GTK_LABEL(label_4),"  Button 4  \n      OFF");
		buttoncheck_4 = button_4 ;
		}
	else
		{
		gtk_label_set_text( GTK_LABEL(label_4),"  Button 4  \n      ON");
		buttoncheck_4 = button_4 ;
		}
	}

if (button_5 != buttoncheck_5)
	{
	if (button_5 == 0)
		{
		gtk_label_set_text( GTK_LABEL(label_5),"  Button 5  \n      OFF");
		buttoncheck_5 = button_5 ;
		}
	else
		{
		gtk_label_set_text( GTK_LABEL(label_5),"  Button 5  \n      ON");
		buttoncheck_5 = button_5 ;
		}
	}

if (button_6 != buttoncheck_6)
	{
	if (button_6 == 0)
		{
		gtk_label_set_text( GTK_LABEL(label_6),"  Button 6  \n      OFF");
		buttoncheck_6 = button_6 ;
		}
	else
		{
		gtk_label_set_text( GTK_LABEL(label_6),"  Button 6  \n      ON");
		buttoncheck_6 = button_6 ;
		}
	}

if (button_7 != buttoncheck_7)
	{
	if (button_7 == 0)
		{
		gtk_label_set_text( GTK_LABEL(label_7),"  Button 7  \n      OFF");
		buttoncheck_7 = button_7 ;
		}
	else
		{
		gtk_label_set_text( GTK_LABEL(label_7),"  Button 7  \n      ON");
		buttoncheck_7 = button_7 ;
		}
	}

if (button_8 != buttoncheck_8)
	{
	if (button_8 == 0)
		{
		gtk_label_set_text( GTK_LABEL(label_8),"  Button 8  \n      OFF");
		buttoncheck_8 = button_8 ;
		}
	else
		{
		gtk_label_set_text( GTK_LABEL(label_8),"  Button 8  \n      ON");
		buttoncheck_8 = button_8 ;
		}
	}

if (button_9 != buttoncheck_9)
	{
	if (button_9 == 0)
		{
		gtk_label_set_text( GTK_LABEL(label_9),"  Button 9  \n      OFF");
		buttoncheck_9 = button_9 ;
		}
	else
		{
		gtk_label_set_text( GTK_LABEL(label_9),"  Button 9  \n      ON");
		buttoncheck_9 = button_9 ;
		}
	}

if (button_10 != buttoncheck_10)
	{
	if (button_10 == 0)
		{
		gtk_label_set_text( GTK_LABEL(label_10),"  Button 10  \n      OFF");
		buttoncheck_10 = button_10 ;
		}
	else
		{
		gtk_label_set_text( GTK_LABEL(label_10),"  Button 10  \n      ON");
		buttoncheck_10 = button_10 ;
		}
	}

if (button_11 != buttoncheck_11)
	{
	if (button_11 == 0)
		{
		gtk_label_set_text( GTK_LABEL(label_11),"  Button 11  \n      OFF");
		buttoncheck_11 = button_11 ;
		}
	else
		{
		gtk_label_set_text( GTK_LABEL(label_11),"  Button 11  \n      ON");
		buttoncheck_11 = button_11 ;
		}
	}

if (button_12 != buttoncheck_12)
	{
	if (button_12 == 0)
		{
		gtk_label_set_text( GTK_LABEL(label_12),"  Button 12  \n      OFF");
		buttoncheck_12 = button_12 ;
		}
	else
		{
		gtk_label_set_text( GTK_LABEL(label_12),"  Button 12  \n      ON");
		buttoncheck_12 = button_12 ;
		}
	}

horizontal = jsside/maxvalue;
vertical = 0 - jsfront/maxvalue;
power = (32767-jspower)/doublemaxvalue;



if (atoi(buffer) == 8)
  	{
	if (vertical != 1)
		{
      		vertical = vertical + 0.1;
		}
        }

if (atoi(buffer) == 2)
  	{
	if (vertical != -1)
		{
      		vertical = vertical - 0.1;
		}
	}

if (atoi(buffer) == 4)
  	{
	if (horizontal != -1)
		{
      		horizontal = horizontal - 0.1;
		}
        }

if (atoi(buffer) == 6)
  	{
	if (horizontal != 1)
		{
      		horizontal = horizontal + 0.1;
		}
	}

if (atoi(buffer) == 9)
  	{
	if (power != 1)
		{
      		power = power + 0.1;
		}
	}

if (atoi(buffer) == 3)
  	{
	if (power != 0)
		{
      		power = power - 0.1;
		}
	}

if (horizontal <= 0)
	{
	gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (bar_left), 0 - horizontal);
	gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (bar_right), 0);
	}
if (horizontal > 0)
	{
	gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (bar_left), 0);
	gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (bar_right), horizontal);
	}
if (vertical <= 0)
	{
	gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (bar_back), 0 - vertical);
	gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (bar_front), 0);
	}
if (vertical > 0)
	{
	gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (bar_back), 0);
	gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (bar_front), vertical);
	}

gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (bar_power), power);

}

return NULL;
}

int myfunction()
	{
	while(1)
		{
		printf("Hello World\n");
		sleep(1);
		}
			
	return 0;
	}

/* This is a callback function. The data arguments are ignored
 * in this example. More on callbacks below. */
static void hello(GtkWidget *widget, gpointer data)
	{
    	g_print ("Hello World\n");
	}

static gboolean delete_event(GtkWidget *widget, GdkEvent *event, gpointer data)
	{

    	g_print ("delete event occurred\n");
	gtk_main_quit ();

    	/* Change TRUE to FALSE and the main window will be destroyed with
     	* a "delete-event". */

    	return TRUE;
	}

static void destroy(GtkWidget *widget, gpointer data)
	{
    	gtk_main_quit ();
	}

int main(int argc, char* argv[])
	{
    	GtkWidget *entry;
    	GtkWidget *window;
	GtkWidget *button;
	GtkWidget *grid;

	GtkWidget *bar_1;
	GtkWidget *bar_2;
	GtkWidget *bar_3;
	GtkWidget *bar_4;

	//initial values
	float front = 0;
	float back = 0;
	float left = 0;
	float right = 0;

	pthread_t input_check;

if(pthread_create (&input_check, NULL, func, (void *) argv))
	{
    	perror("ERROR creating func thread.");
	}
    
    	gtk_init (&argc, &argv);

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

    	//g_print ("Hello World\n");
    
    	gtk_main ();

    	return 0;
	}





