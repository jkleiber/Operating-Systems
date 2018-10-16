Project 0

Name: Justin Kleiber

Email address: jkleiber@ou.edu | jkleiber8@gmail.com

Date: 09/09/2018

Description
My approach to solve this problem is to first cast the UPPER and LOWER 
program arguments to unsigned characters. My program optimizes 
compute time by not allowing any computation if the upper character is after 
the lower character in the character table. Then I proceed to read input from 
stdin using the getchar() function until the input stream ended. I detect the 
end of stdin with the feof() function as I found that comparing getchar() to 
the EOF macro is insufficient (EOF can be triggered by errors, as well as 
end-of-file). As characters are read in, my program keeps track of the number
of appearances of each character in an array. After the stdin "file" reaches 
the end, I loop through elements in the array based on the boundary characters 
provided by the user and output the number of times they occured in the file 
while printing out the corresponding number of # characters.

References
I used https://www.geeksforgeeks.org/eof-and-feof-in-c/ to learn more about 
feof() and how getchar() handles errors and EOF.

I also used http://www.cplusplus.com/reference/cstdio/printf/ to learn how to
do the 4 character right-flush output formatting.

For help converting two ASCII chars to a single UTF-8 character, I referred to
this link: https://www.utf8-chartable.de/unicode-utf8-table.pl?utf8=dec. I also 
referred Trey Sullivent to this link when talking about high-level project 
requirements. I did not end up using this as I found that casting worked 
better. 

For my Makefile, I used the things I learned from Dr. Fagg during his in-class 
demonstration to set it up.

I've used C on Linux a lot for work, so most of this was pretty familiar. I 
did need to look at the man pages for memset to remember the ordering of the 
arguments, and I also used the man pages to learn more about getchar().
