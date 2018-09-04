CC = gcc
CFLAGS = -c -O3 -Wall

all: project0

.c.o:
	$(CC) $(CFLAGS) $< -o $@

project0: project0.o
	$(CC) project0.o -o project0
