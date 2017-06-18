##
##Created by Stewart Collins - scollins2 326206
##


## CC  = Compiler.
## CFLAGS = Compiler flags.
CC	= gcc
CFLAGS 	= -pthread

## OBJ = Object files.
## SRC = Source files.
## EXE = Executable name.

SRC =		server.c formatTime.c client.c
OBJ =		server.o formatTime.o client.o
EXE = 		server client



server: server.o formatTime.o
		$(CC) $(CFLAGS) -o server server.o formatTime.o -lm
		
client: client.o
		$(CC) -o client client.o -lm
		
## Clean: Remove object files and core dump files.
clean:
		/bin/rm $(OBJ) 

## Clobber: Performs Clean and removes executable file.

clobber: clean
		/bin/rm $(EXE) 

## Dependencies

server.o:		server.h formatTime.h
formatTime.o: 	formatTime.h
client.o:   	client.h