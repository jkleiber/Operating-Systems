CC = gcc
CFLAGS = -c -I -O3 -Wall

all: hw4

clean:
	rm thread_incr_psem

#convert .c to .o
.c.o:
	$(CC) $(CFLAGS) $< -o $@

hw4: thread_incr_psem.o
	$(CC) thread_incr_psem.o -o thread_incr_psem
