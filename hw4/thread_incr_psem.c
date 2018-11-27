/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2018.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* thread_incr_psem.c

   Use a POSIX unnamed semaphore to synchronize access by two threads to
   a global variable.

   See also thread_incr.c and thread_incr_mutex.c.
*/
#include <semaphore.h>
#include <pthread.h>
#include "tlpi-dist/lib/tlpi_hdr.h"

static int glob = 0;    //global resource to increment
static sem_t sem;       //semaphore to control access to glob

static void *                   /* Loop 'arg' times incrementing 'glob' */
threadFunc(void *arg)
{
    /* Local variables */
    int loops = *((int *) arg); //loop max
    int loc, j;                 //glob incrementer, and iteration variable

    //Go the number of loops requested
    for (j = 0; j < loops; j++) 
    {
        //Wait for the semaphore to allow the thread to execute
        if (sem_wait(&sem) == -1)
        {
            errExit("sem_wait");
        }

        //Increment glob
        loc = glob;
        loc++;
        glob = loc;

        //Indicate that the increment is done
        //This will unlock access to the glob resource for another thread
        if (sem_post(&sem) == -1)
        {
            errExit("sem_post");
        }
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    /* Declare local variables */
    int         i;          //iteration variable
    int         loops;      //number of loops for the program
    int         num_threads;//number of threads
    int         s;          //error variable
    pthread_t  *threads;    //array of threads

    //Get program arguments for number of loops and threads
    loops = (argc > 1) ? getInt(argv[1], GN_GT_0, "num-loops") : 10000000;
    num_threads = (argc > 2) ? getInt(argv[2], GN_GT_0, "num-threads") : 2;

    //Allocate a dynamic array of threads
    threads = (pthread_t*)malloc(num_threads * sizeof(pthread_t));

    /* Initialize a semaphore with the value 1 */
    if (sem_init(&sem, 0, 1) == -1)
    {
        errExit("sem_init");
    }

    /* Create threads that increment 'glob' */
    for(i = 0; i < num_threads; ++i)
    {
        //Create each thread
        s = pthread_create(&threads[i], NULL, threadFunc, &loops);
        if (s != 0)
        {
            errExitEN(s, "pthread_create");
        }
    }

    /* Wait for threads to terminate */
    for(i = 0; i < num_threads; ++i)
    {
        //Join each thread when it completes
        s = pthread_join(threads[i], NULL);
        if (s != 0)
        {
            errExitEN(s, "pthread_join");
        }
    }

    //Free dynamically allocated threads
    free(threads);

    //Output the result and indicate success
    printf("glob = %d\n", glob);
    exit(EXIT_SUCCESS);
}
