
CC=gcc
CFLAGS = -g -Wall -lm -pthread
EXEC = ./pc-terminal
SOURCE_FILES =\
pc_terminal.c\
../message/crc.c\
../message/message.c\
pc_message_helper.c\
time_helper.c\
pc_term.c pc_rs232.c\
js.c\
gui.c

all:
	$(CC) $(CFLAGS) $(SOURCE_FILES) -o $(EXEC) `pkg-config --cflags --libs gtk+-3.0`

run: all
	$(EXEC)