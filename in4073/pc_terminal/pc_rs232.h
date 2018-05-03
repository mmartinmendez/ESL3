#ifndef _PC_RS232_H_
#define _PC_RS232_H_

void rs232_open(void);
void rs232_close(void);
int	rs232_getchar_nb();
int rs232_getchar();
int rs232_putchar(char c);

#endif
