CC=gcc
CFLAGS=-I.
DEPS = joystick.h
OBJ = js.o 

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

js: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)
