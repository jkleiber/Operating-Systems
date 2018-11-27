CC = gcc
CFLAGS = -c -I -O3 -Wall -pthread -lrt

#make everything
all: hw4

#delete the program
clean:
	rm *.o
	rm thread_incr_psem

#convert .c to .o
.c.o:
	$(CC) $(CFLAGS) $< -o $@

#compile homework code
hw4: tlpi-dist/lib/error_functions.o tlpi-dist/lib/get_num.o thread_incr_psem.o
	$(CC) thread_incr_psem.o tlpi-dist/lib/error_functions.o tlpi-dist/lib/get_num.o -pthread -o thread_incr_psem
