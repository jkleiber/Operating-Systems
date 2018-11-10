Project 3

Name: Justin Kleiber

Email address: jkleiber@ou.edu | jkleiber8@gmail.com

Date: 10/21/2018

Description
For the OUFS filesystem, I have developed code that interacts
with a virtual disk. This code is based off of the framework
posted on the course webpage.

My main strategy was to beef up the API and make it carry the workload.
I made helper functions to find files easier and to allocate and deallocate
blocks and inodes. The search function is the backbone of my program, as a
file tree walk is used to find the applicable directory entries. I use 
string tokenization to split the paths provided and use that to walk the 
tree and find files.

References
I mainly built my code off the provided code on the course website.
I consulted with Trey Sullivent, Alex Miller and Noah Zemlin on edge case
theorizing and code requirements

I used this link to learn how to use qsort():
https://www.tutorialspoint.com/c_standard_library/c_function_qsort.htm

For formatting the comparator function used in qsort, I found this link:
https://www.geeksforgeeks.org/comparator-function-of-qsort-in-c/
