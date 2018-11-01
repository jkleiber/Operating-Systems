
CC = gcc
CFLAGS = -c -I -O3 -Wall
OUFS = oufs_lib_support.o vdisk.o

#Default to make the executable
all: zinspect zformat zfilez zrmdir
#zmkdir

#Clean the project workspace
clean: 
	rm *.o
	rm zinspect
	rm zformat
	rm zfilez
	#rm zmkdir
	rm zrmdir

#Convert the C files to O files
.c.o:
	$(CC) $(CFLAGS) $< -o $@

#Build the zinspect program
zinspect: zinspect.o $(OUFS)
	$(CC) zinspect.o $(OUFS) -o zinspect

#Build the zformat program
zformat: zformat.o $(OUFS)
	$(CC) zformat.o $(OUFS) -o zformat

#Build the zfilez program
zfilez: zfilez.o $(OUFS)
	$(CC) zfilez.o $(OUFS) -o zfilez

#Build the zmkdir program
zmkdir: zmkdir.o $(OUFS)
	$(CC) zmkdir.o $(OUFS) -o zmkdir

#Build the zrmdir program
zrmdir: zrmdir.o $(OUFS)
	$(CC) zrmdir.o $(OUFS) -o zrmdir
