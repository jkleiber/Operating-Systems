#include "system_calls.h"

/*
 *
 */
int fork_exec(char * command, char * arg_list[])
{
    //Declare local variables
    int pid;    //process ID of child process
    int status; //status of child process

    //Create a child process to do to execution of the command
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

            //TODO: error handling
            return status;
    }

    //Nothing bad happened, so return 0
    return 0;
}

/*
 *
 */
int general_command(char *command, char **flags, int num_flags, char **args, int num_tokens)
{
    //Declare local variables
    char  **args_list;  //list of tokens to pass to fork + exec combo
    int     i;          //iteration variable
    int     result;     //result of the system call

    //Initialize variables
    args_list = (char**)malloc((num_tokens + num_flags + 1) * sizeof(char*));

    //Create the command part of the args list
    args_list[0] = strdup(command);

    //Add any applicable flags to the front of the command
    for(i = 0; i < num_flags; ++i)
    {
        args_list[i + 1] = strdup(flags[i]);
    }

    //Add the arguments provided next
    for(i = 1; i < num_tokens; ++i)
    {
        args_list[i + num_flags] = strdup(args[i]);
    }

    //NULL terminate the command
    args_list[num_flags + num_tokens] = NULL;

    //Run the command
    result = fork_exec(command, args_list);

    //Free dynamically allocated memory
    for(i = 0; i < (num_flags + num_tokens + 1); ++i)
    {
        free(args_list[i]);
    }
    free(args_list);

    //Return execution completion status
    return result;
}