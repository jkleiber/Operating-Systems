Project 1

Name: Justin Kleiber

Email address: jkleiber@ou.edu | jkleiber8@gmail.com

Date: 09/27/2018

Description
My approach to this problem was to create a sequence of if statements that
determine the command the user wants to run. Before this though, I check to 
see if a batch file was input to the program. Either way it accepts user 
commands and executes them accordingly. When possible, I used system() but when
this was not possible I leaned on libc functions like rename() and on FILE 
pointers.
I used dynamic memory allocation to reduce memory requirements and based my
code off of the code posted on the course website as a starting point. I tried
to encapsulate code into functions when possible, and I found a way to 
generalize the code for help and mimic.

Instructions
Run my code from your parent shell with "./project1" (no quotes). Batch files
can be read in from redirected standard input, or as a program argument.

Here's a list of commands:
esc - ends the shell
wipe - clears the view on the shell
filez (target) - shows files in the current working directory (or optionally 
                 target)
chdir (dir) - changes the current directory (if dir is not passed this prints 
              working directory)
environ - see environment variables
ditto (message) - prints back message to the shell
help - pull up this helpful menu
mimic [src] [dst] - copies a src file to a dst file. Both arguments are 
                    required.
morph [src] [dst] - Move a source file to a destination file. If dst is a 
                    directory, the filename is kept but the path changes.
                    Both arguments are required.
erase [target] - deletes a file from the filesystem. The target is required

You can also run any normal linux command through this shell.

Known Bugs/Assumptions
My code assumes that all directory paths end with a forward slash.
I also made the assumption that only files will be copied with mimic.
When the linux command 'cd' is executed, its effect does not stick after the 
command is run because its child process ends.

References
In general, I leaned on the man pages, lecture notes, and given code for help 
on this assignment. When I didn't know how to do something, I searched online,
and those links are below.

I talked to Alex Miller about possible ways to implement the morph feature 
after the new requirements were released. I also talked to Shelby Conklin and 
Trey Sullivent about high level requirements. 

In the past when I wanted to copy strings, I've used strcpy. I referenced man
pages for this, but it was interesting to learn the differences between strcpy
and strdup here:
https://stackoverflow.com/questions/14020380/strcpy-vs-strdup

I used the Open Group's website to learn how to implement putenv()
http://pubs.opengroup.org/onlinepubs/009604599/functions/putenv.html

Learning how to add errno to the code:
http://man7.org/linux/man-pages/man3/errno.3.html

Printing errno in human readable form:
https://stackoverflow.com/questions/503878/how-to-know-what-the-errno-means

I used this as a reference when using fopen to read and write:
https://www.tutorialspoint.com/c_standard_library/c_function_fopen.htm

Found this after searching how to remove files in C:
https://linux.die.net/man/3/remove

I wanted to know how to take a substring of a character sequence in C, and this 
helped me:
https://www.linuxquestions.org/questions/programming-9/extract-substring-from-string-in-c-432620/

For detecting if stdin was from a terminal or a redirection, I found this link: 
https://stackoverflow.com/questions/26747597/perform-action-if-input-is-redirected

For finding the filename in a path, I found out about basename through google
leading me to the man page for it:
http://man7.org/linux/man-pages/man3/basename.3.html