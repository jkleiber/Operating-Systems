CC = gcc
CFLAGS = -c -O3 -Wall

#Default to make the executable
all: project1

#Clean the project1 workspace
clean: 
	rm *.o
	rm project1

#Convert the C files to O files
.c.o:
	$(CC) $(CFLAGS) $< -o $@

#Build the program for project1
project1: project1.o
	$(CC) project1.o -o project1