Project 2

Name: Justin Kleiber

Email address: jkleiber@ou.edu | jkleiber8@gmail.com

Date: 10/21/2018

Description
For project 2, I built largely off of project 1, but focused on organizing my
code into modules to help make future development easier. The main difference
between project 1 and this project is that fork() and exec() replace system(),
so I developed a general way to run any code using fork and exec from any
function (including handling all file direction that may be required). The
other big change was that I made mimic and morph recursive. I did this by 
making a depth first copy using dirent and a recursive function. Since I spent
a good amount of time making fork and exec modular, I was able to use this in
my recursive copy function.
Each type of function is now categorically organized into its own code file.
Header files were added to make the function interfacing more organized, and a 
global constants header was added to keep a standard set of constants across 
files. This was all done in an effort to make the code more readable and easier
to understand.

Instructions
Run my code from your parent shell with "./project2" (no quotes). Batch files
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
mimic (-r) [src] [dst] - copies a src file to a dst file.
                         src and dst can be files or directories.
                         Both arguments are required.
morph (-r) [src] [dst] - Move a source file to a destination file. src and dst
                         can be files and directories.
                         Both arguments are required.
erase [target] - deletes a file from the filesystem. The target is required
mkdirz [dir] - creates a new directory named [dir]. [dir] is required
rmdirz [dir] - deletes [dir] from disk. The [dir] argument is required.

You can also run any normal linux command through this shell.

To run a batch file through this shell, perform one of the following operations
./project1 <file>
./project1 < <file>
cat <file> | ./project1

Known Bugs/Assumptions
If the batch file submitted to the program does not exist (or is not a file)
the program closes and prints an error.

References
In general, I leaned on the man pages, lecture notes, and given code for help 
on this assignment. I also looked back on what I did in project 1 (and copied 
a lot of that code over to this assignment initially) to understand things.
When I didn't know how to do something, I searched online,
and those links are below.

I talked to Trey Sullivent about the new morph and mimic changes, and about how
fork and exec were supposed to be implemented. 

For learning how to get an absolute path of the current working directory
I referenced this code from the Open Group:
http://pubs.opengroup.org/onlinepubs/009695399/functions/getcwd.html

To gain a better understanding of how to use fork and exec, I referred to this
link: https://www.geeksforgeeks.org/difference-fork-exec/

I needed to see if echo took multiple arguments and I found that doc
here: https://www.mkssoftware.com/docs/man1/echo.1.asp

I needed to reference dirname from this online manual:
https://linux.die.net/man/3/dirname

To convert my file pointers to file descriptors, I found this explanation of
fileno(): https://stackoverflow.com/questions/3167298/how-can-i-convert-a-file-pointer-file-fp-to-a-file-descriptor-int-fd

For some dup2() examples I looked at this:
https://www.geeksforgeeks.org/dup-dup2-linux-system-call/

I found this link useful when trying to figure out how to determine if
a directory is empty: https://stackoverflow.com/questions/6383584/check-if-a-directory-is-empty-using-c-on-linux
