/*------------------------------------------------------------
 * Simple pc terminal in C
 *
 * Arjan J.C. van Gemund (+ mods by Ioannis Protonotarios)
 *
 * read more: http://mirror.datenwolf.net/serial/
 *------------------------------------------------------------
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <inttypes.h>
#include <ctype.h>
#include "pc_term.h"

/*------------------------------------------------------------
 * console I/O
 *------------------------------------------------------------
 */
struct termios 	savetty;

void term_initio()
{
	struct termios tty;

	tcgetattr(0, &savetty);
	tcgetattr(0, &tty);

	tty.c_lflag &= ~(ECHO|ECHONL|ICANON|IEXTEN);
	tty.c_cc[VTIME] = 0;
	tty.c_cc[VMIN] = 0;

	tcsetattr(0, TCSADRAIN, &tty);
}

void term_exitio()
{
	tcsetattr(0, TCSADRAIN, &savetty);
}

void term_puts(char *s)
{
	fprintf(stderr,"%s",s);
}

void term_putchar(char c)
{
	putc(c,stderr);
}

// Author: Mithun Martin Mendez
uint8_t parse_key(char *buf) {
  switch (buf[0]) {
  case '\x7f':
  case '\b':
    return KEY_BACKSPACE;

  case '0':
    return KEY_0;

  case '1':
    return KEY_1;

  case '2':
    return KEY_2;

  case '3':
    return KEY_3;

  case '4':
    return KEY_4;

  case '5':
    return KEY_5;

  case '6':
    return KEY_6;

  case '7':
    return KEY_7;

  case '8':
    return KEY_8;

  case '9':
    return KEY_9;

  case '[':
    return KEY_LEFT_BRACKET;

  case ']':
    return KEY_RIGHT_BRACKET;

  case 'a':
    return KEY_A;

  case 'b':
    return KEY_B;

  case 'c':
    return KEY_C;

  case 'd':
    return KEY_D;

  case 'e':
    return KEY_E;

  case 'f':
    return KEY_F;

  case 'g':
    return KEY_G;

  case 'h':
    return KEY_H;

  case 'i':
    return KEY_I;

  case 'j':
    return KEY_J;

  case 'k':
    return KEY_K;

  case 'l':
    return KEY_L;

  case 'm':
    return KEY_M;

  case 'n':
    return KEY_N;

  case 'o':
    return KEY_O;

  case 'p':
    return KEY_P;

  case 'q':
    return KEY_Q;

  case 'r':
    return KEY_R;

  case 's':
    return KEY_S;

  case 't':
    return KEY_T;

  case 'u':
    return KEY_U;

  case 'v':
    return KEY_V;

  case 'w':
    return KEY_W;

  case 'x':
    return KEY_X;

  case 'y':
    return KEY_Y;

  case 'z':
    return KEY_Z;

  case '\x1b':
    switch (buf[1]) {
    case '\0':
      return KEY_ESCAPE;

    case '[':
      switch (buf[2]) {
      case 'A':
        return KEY_UP;

      case 'B':
        return KEY_DOWN;

      case 'C':
        return KEY_RIGHT;

      case 'D':
        return KEY_LEFT;
      }
    }
		/*@fallthrough@*/
  default:
    return KEY_UNKNOWN;
  }
}

// Author: Mithun Martin Mendez
uint8_t	term_getchar_nb()
{

	int i;

	char *buf = (char *) malloc(5 * sizeof(char));

  if (buf == NULL) {
    return KEY_UNKNOWN;
  }

  buf[0] = '\0';
  buf[1] = '\0';
  buf[2] = '\0';
	buf[3] = '\0';

  i = 0;
	char c = '\0';

  ssize_t n = 0;

	while(i < 3) {
		n = read(0, &c, 1);
		buf[i++] = c;
	}

	if(n) {
		uint8_t k = parse_key(buf);
		free(buf);
		return k;
	}

	return KEY_UNKNOWN;
}

int	term_getchar()
{
        int    c;

        while ((c = term_getchar_nb()) == -1)
                ;
        return c;
}
