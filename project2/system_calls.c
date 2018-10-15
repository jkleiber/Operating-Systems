#include "system_calls.h"

void fork_exec(char * command, char * arg_list[])
{
    //Declare local variables
    int pid;    //process ID of child process
    int status; //status of child process

    switch(pid = fork())
    {
        //If there was an error, alert the user
        case -1:
            fprintf(stderr, "Fork error: %s\n", strerror(errno));
            break;
        //If this is the child process, execute a system command with exec()
        case 0:
            execvp(command, arg_list);
            exit(0);
        //Otherwise wait for the child process to exit before continuing
        default:
            waitpid(pid, &status, WUNTRACED);
    }
}