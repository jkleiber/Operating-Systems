#include "system_calls.h"

/*
 * file_redirection - handles redirection characters and sets up file redirects
 * 
 * @param **args: tokenized command line input
 * @param num_tokens: number of tokens of command line input.
 * @param mode: allowed redirect operations
 * @param *files: files array to add redirection to.
 */
int file_redirection(char **args, int num_tokens, int mode, file_info *files)
{
    //Declare local variables
    int i;          //iteration variable for args
    int j;          //number of redirects
    int max_tokens; //number of input tokens

    //Initialize non-redirection token counter and redirection counter
    j = 0;
    max_tokens = num_tokens;

    //Find any and all redirection tokens and pipe the output accordingly
    for(i = 1; i < num_tokens; ++i)
    {
        //Write stdout to a file
        if(!strcmp(args[i], ">") && ((mode >> WRITE) & 1))
        {
            //Prevent any more args from appearing after redirection chars
            if(max_tokens > i)
            {
                max_tokens = i;
            }

            //if the file argument is provided...
            if(args[i + 1] != NULL && i < (num_tokens - 1))
            {
                //open the file and pipe it to stdout
                files[j].file = fopen(args[i + 1], "w");
                files[j].type = WRITE;
                ++j;
            }
            else
            {
                fprintf(stderr, "Input Error: no file provided for redirection. Use format cmd > file.txt\n");
                return -1;
            }
        }
        //Append stdout to a file
        else if(!strcmp(args[i], ">>") && ((mode >> APPEND) & 1))
        {
            //Prevent any more args from appearing after redirection chars
            if(max_tokens > i)
            {
                max_tokens = i;
            }

            //if the file argument is provided...
            if(args[i + 1] != NULL && i < (num_tokens - 1))
            {
                //open the file and append stdout to it
                files[j].file = fopen(args[i + 1], "a");
                files[j].type = APPEND;
                ++j;
            }
            else
            {
                fprintf(stderr, "Input Error: no file provided for redirection. Use format cmd >> file.txt\n");
                return -1;
            }
        }
        //Read file input into stdin
        else if(!strcmp(args[i],"<") && ((mode >> READ) & 1))
        {
            //Prevent any more args from appearing after redirection chars
            if(max_tokens > i)
            {
                max_tokens = i;
            }

            //if the file argument is provided...
            if(args[i + 1] != NULL && i < (num_tokens - 1))
            {
                //and the read permissions are permitted
                if(!access(args[i + 1], R_OK))
                {
                    //open the file and pipe it to stdin
                    files[j].file = fopen(args[i + 1], "r");
                    files[j].type = READ;
                    ++j;
                }
                else
                {
                    fprintf(stderr, "Unable to access file for redirection: %s\n", strerror(errno));
                }
            }
            else
            {
                fprintf(stderr, "Input Error: no file provided for redirection. Use format cmd < file.txt\n");
                return -1;
            }
        }

        //End redirection once the maximum number of redirections are passed in
        if(j >= NUM_REDIRECTS)
        {
            break;
        }
    }

    //If no errors were encountered, return the number of tokens before redirection
    return max_tokens;
}

/*
 * fork_exec - forks a child process and then executes a command
 * 
 * @param *command: command to execute
 * @param *arg_list[]: arguments to pass to the command
 * @param *files: files to redirect to
 * @return: status of the process execution upon return
 */
int fork_exec(char * command, char * arg_list[], file_info *files)
{
    //Declare local variables
    int fd;     //file descriptor for redirects
    int i;      //iteration variable
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
            //Only consider redirecting files if the files array isn't NULL
            if(files)
            {
                //Redirect file descriptors as necessary
                for(i = 0; i < NUM_REDIRECTS; ++i)
                {
                    //If the file exists
                    if(files[i].file)
                    {
                        //get the file descriptor associated with the file
                        fd = fileno(files[i].file);

                        //Assign a file to stdin if it is a READ type
                        if(files[i].type == READ)
                        {
                            fd = dup2(fd, STDIN_FILENO);
                        }
                        //Assign a file to stdout if it is a WRITE or APPEND type
                        else if(files[i].type == WRITE || files[i].type == APPEND)
                        {
                            fd = dup2(fd, STDOUT_FILENO);
                        }
                    }
                }
            }
fprintf(stderr, "%s\n", command);
            //Run the command
            execvp(command, arg_list);

            //Flush stdout
            fflush(stdout);

            if(files)
            {
                //Close all files used for redirection
                for(i = 0; i < NUM_REDIRECTS; ++i)
                {
                    fclose(files[i].file);
                    files[i].type = -1;
                    files[i].file = NULL;
                }
            }

            //Exit with a success code
            exit(EXIT_SUCCESS);

        //Otherwise wait for the child process to exit before continuing
        default:
            //Wait until the child process has exited with some status
            waitpid(pid, &status, WUNTRACED);
fprintf(stderr, "status: %d\n", status);
            //Return the status of the child process
            return status;
    }

    //Nothing bad happened, so return 0
    return 0;
}

/*
 * general_command - command wrapper for fork() and exec()
 * 
 * @param *command: the one word command that is entered first in the terminal
 * @param **flags: any flags that are passed to the command.
 * @param num_flags: number of flags in the flags array
 * @param **args: any additional arguments to the command
 * @param num_tokens: number of arguments including the command
 * @param *files: list of files to redirect to
 * @return: status of the fork and exec results
 * 
 * Note: if flags are all that are passed in (for custom reasons), num_tokens needs to be set to 1 for the last flag to be read.
 * This is to reflect that the number of tokens should never be less than 1, as a command is a token
 */
int general_command(char *command, char **flags, int num_flags, char **args, int num_tokens, file_info *files)
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
    fprintf(stderr, "%s\n", args_list[i+num_flags]);
    }

    //NULL terminate the command
    args_list[num_flags + num_tokens] = NULL;

    //Run the command
    result = fork_exec(command, args_list, files);

    //Free dynamically allocated memory
    for(i = 0; i < (num_flags + num_tokens + 1); ++i)
    {
        free(args_list[i]);
    }
    free(args_list);

    //Return execution completion status
    return result;
}