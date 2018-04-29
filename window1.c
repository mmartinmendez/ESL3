#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <termios.h>
#include <stdio.h>


GtkWidget *bar_power;
GtkWidget *bar_front;
GtkWidget *bar_back;
GtkWidget *bar_left;
GtkWidget *bar_right;



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
char buffer[1] = {0};
float horizontal = 0;
float vertical = 0;
float power = 0;

//char **arguments = (char**)param;

while (1)
{
buffer[0] = getch();
//buffer[0] = getchar();

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
	gtk_widget_set_size_request (window, 300, 200);
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

	//create button
  	button = gtk_button_new_with_label ("Quit");
  	g_signal_connect_swapped (button, "clicked", G_CALLBACK (gtk_widget_destroy), window);
	gtk_grid_attach (GTK_GRID (grid), button, vposition + 4, hposition + 2, 1, 1);

	//show everything in window
	gtk_widget_show_all (window);

    	//g_print ("Hello World\n");
    
    	gtk_main ();

    	return 0;
	}





