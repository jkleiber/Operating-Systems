Homework 4

Justin Kleiber | jkleiber@ou.edu

Description
I took the sample code from the book's website using wget.
I then modified it to accept a variable number of threads by
dynamically allocating memory to an array of pthreads. I used
for loops to create each thread and to join the threads at the
end of execution. I made no modifications to the thread function
itself except adding comments

For compiling and running my code, I downloaded the tlpi library
to my local ubuntu machine and put it in the same directory as my
code. I made a makefile that operates near the same way my project
files work, but they list two of the tlpi files as dependencies. I
also had to add the -pthread flag to the linking phase due to the 
man page's advice.

Assumptions
The tlpi-dist library must be in the same directory as the code.
In order to compile the program at that point, make just needs to be
run.

Bugs/Edge cases
The bug I ran into was that the code wouldn't compile because -pthread
wasn't specified, and because my dependencies were not set right. Once this
was fixed, everything else went smoothly.

Semaphores
The way this project works is each thread I create is trying to access the
glob resource simultaneously. The semaphore manages which thread has access
to the resource at any given time through calls to sem_wait. Once a thread 
has completed the increment operation, it allows another thread to acces glob.

Real vs User vs Kernel time
Real time is the total amount of time spent doing a task. User time is the amount
of time a thread spends in user mode. The kernel time is the amount of time the
thread spends in the kernel. Therefore real time is going to be the most obvious 
measure of how long a program takes, but the other two times help find where the 
program is being slowed down. 

In the script, real time is being used to show how the number of threads trying 
to access a shared resource affects the total time that it takes to run the 
program. These results will be discussed next.

Plots/Results
In the plot it can be seen that as more threads try to access a 
resource, it takes longer and longer to complete the process of incrementing 
glob. Additionally, as the number of loops increases, the time taken to run the 
program increases. So when loops increase and the number of threads increase, the
time increases exponentially.

These effects result because more threads trying to access a resource means the 
semaphore has to do more work to block and unblock access to the resource. With
each passing loop these time differences add up, so more loops make the overall
real time difference worse as threads increase.