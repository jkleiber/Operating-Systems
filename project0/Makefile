CC = gcc
CFLAGS = -c -Wall -g

#Add back -O3 above

all: project0

.c.o:
	$(CC) $(CFLAGS) $< -o $@

project0: project0.o
	$(CC) project0.o -o project0
