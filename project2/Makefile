
CC = gcc
CFLAGS = -c -I -O3 -Wall

#Default to make the executable
all: project2

#Clean the project1 workspace
clean: 
	rm *.o
	rm project2

#Convert the C files to O files
.c.o:
	$(CC) $(CFLAGS) $< -o $@

#Build the program for project1
project2: project2.o error_handler.o file_operations.o printables.o system_calls.o
	$(CC) project2.o error_handler.o file_operations.o printables.o system_calls.o -o project2
