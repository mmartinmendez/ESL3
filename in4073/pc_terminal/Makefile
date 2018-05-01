
CC=gcc
CFLAGS = -g -Wall -lm
EXEC = ./pc-terminal

all:
	$(CC) $(CFLAGS) pc_terminal.c ../message/crc.c ../message/message.c -o $(EXEC)

run: all
	$(EXEC)