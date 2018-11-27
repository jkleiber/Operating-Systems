Project 4

Name: Justin Kleiber

Email address: jkleiber@ou.edu | jkleiber8@gmail.com

Date: 11/15/2018

Description
I have built on my project 3 code for this project. My ztouch function
uses the fopen and fclose functions provided in the API, which I have coded.
My code from project 3 needed some minor adjustments to handle the files in
the file system (i.e. not assuming everything is a directory).

For the remaining additional functions I have built out the oufs prototypes
that I did not implement in project 3. After developing oufs_fread and 
oufs_fwrite I was able to quickly make the more, removal and linking functions. 
I used my find file function to do these latter functions. After finding a file,
my code will perform the appropriate operation on it and then close the file 
pointer.

For the zcreate and zappend function, I kept it simple and only read the stdin
one character at a time. For zmore however I found it easy to read block by block.

Troubleshooting
I ran into some segmentation faults because I was deallocating blocks that were
already unallocated.

Assumptions
I assume in my code that a call to zcreate does not destroy links (i.e. when zcreate
truncates a file it affects all linked files)

Instructions
When running my code, run it as shown in the specifications. It will handle errors
appropriately

References
I used the man pages to refresh myself on functions like getchar and basename. I also
looked back at my previous projects to see how I used getchar() and other code.
Obviously I built off of project 3 code and the oufs API provided.

I have discussed the project in passing with Trey Sullivent. I talked to Alex Miller 
about the project as well, mostly with regard to fread and fwrite.
