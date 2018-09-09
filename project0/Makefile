CC = gcc
CFLAGS = -c -O3 -Wall -g

#Always make the project 0 application
all: project0

#Convert .c to .o
.c.o:
	$(CC) $(CFLAGS) $< -o $@

#Build the project 0 app from the .o files
project0: project0.o
	$(CC) project0.o -o project0
