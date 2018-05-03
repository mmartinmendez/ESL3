
CC=gcc
CFLAGS = -g -Wall -lm
EXEC = ./pc-terminal

all:
	$(CC) $(CFLAGS) pc_terminal.c ../message/crc.c ../message/message.c pc_message_helper.c time_helper.c pc_term.c pc_rs232.c -o $(EXEC)

run: all
	$(EXEC)