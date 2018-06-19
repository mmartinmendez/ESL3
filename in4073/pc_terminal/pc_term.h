#ifndef _PC_TERM_H_
#define _PC_TERM_H_

void term_initio();
void term_exitio();
void term_puts(char *s);
void term_putchar(char c);
key	term_getchar_nb();
int	term_getchar();

typedef enum {
  KEY_BACKSPACE,

  KEY_0,
  KEY_1,
  KEY_2,
  KEY_3,
  KEY_4,
  KEY_5,
  KEY_6,
  KEY_7,
  KEY_8,
  KEY_9,

  KEY_LEFT_BRACKET,
  KEY_RIGHT_BRACKET,

  KEY_A,
  KEY_B,
  KEY_C,
  KEY_D,
  KEY_E,
  KEY_F,
  KEY_G,
  KEY_H,
  KEY_I,
  KEY_J,
  KEY_K,
  KEY_L,
  KEY_M,
  KEY_N,
  KEY_O,
  KEY_P,
  KEY_Q,
  KEY_R,
  KEY_S,
  KEY_T,
  KEY_U,
  KEY_V,
  KEY_W,
  KEY_X,
  KEY_Y,
  KEY_Z,

  KEY_UP,
  KEY_DOWN,
  KEY_RIGHT,
  KEY_LEFT,

  KEY_ESCAPE,

  KEY_UNKNOWN
} key;

key parse_key(char *buf);

#endif
