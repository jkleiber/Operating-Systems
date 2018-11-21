
CC = gcc
CFLAGS = -c -I -O3 -Wall
OUFS = oufs_lib_support.o vdisk.o

#Default to make the executable
all: zinspect zformat zfilez zrmdir zmkdir ztouch zcreate zappend zmore

#Clean the project workspace
clean: 
	rm *.o
	rm zinspect
	rm zformat
	rm zfilez
	rm zmkdir
	rm zrmdir
	rm ztouch
	rm zcreate
	rm zappend
	rm zmore

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
	
#Build the ztouch program
ztouch: ztouch.o $(OUFS)
	$(CC) ztouch.o $(OUFS) -o ztouch
	
#Build the zcreate program
zcreate: zcreate.o $(OUFS)
	$(CC) zcreate.o $(OUFS) -o zcreate
	
#Build the zappend program
zappend: zappend.o $(OUFS)
	$(CC) zappend.o $(OUFS) -o zappend
	
#Build the zmore program
zmore: zmore.o $(OUFS)
	$(CC) zmore.o $(OUFS) -o zmore
