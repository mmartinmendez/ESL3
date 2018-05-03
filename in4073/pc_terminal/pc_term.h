#ifndef _PC_TERM_H_
#define _PC_TERM_H_

void term_initio();
void term_exitio();
void term_puts(char *s);
void term_putchar(char c);
int	term_getchar_nb();
int	term_getchar();

#endif
