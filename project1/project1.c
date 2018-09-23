#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* Macros */
#define FALSE 0
#define TRUE  1

#define MORPH_OP 0
#define MIMIC_OP 1
#define HELP_OP 2

/* Pre-compiler constants */
#define MAX_ARGS 	    64         // max # args
#define MAX_BUFFER 	    1024       // max line buffer
#define SEPARATORS 	    " \t\n"    // token sparators

/* External variables */
extern char **environ;         // environment array

/*
 *
 * 
 */
int file_operations(char *src, char *dst, int operation)
{
    /* Declare local variables */
    char    data;       //data to move between files
    FILE   *dst_file;   //destination file pointer
    size_t  num_read;   //number of items read from file
    FILE   *src_file;   //source file pointer

    //Open the source file
    src_file = fopen(src, "r");

    //If the source file exists, then create/open the dst file
    if(src_file)
    {
        //Create/open the dst file
        dst_file = fopen(dst, "w");

        //If fopen succeeded, perform the copy/move/print
        if(dst_file
        || operation == HELP_OP)
        {
            num_read = fread(&data, sizeof(char), 1, src_file);

            //Read the source file until EOF
            while(num_read > 0 && !feof(src_file))
            {
                //Decide what to do with the data read in.
                switch(operation)
                {
                    //Print the contents of the README.txt file
                    case HELP_OP:
                        fprintf(stdout, "%c", data);
                        fflush(stdout);
                        break;
                    
                    //Move data between files if it is a mimic or a morph
                    case MIMIC_OP:
                    case MORPH_OP:
                        fwrite(&data, sizeof(char), 1, dst_file);
                        break;

                    //How did we get here? Just break.
                    default:
                        break;
                }

                //Read more
                num_read = fread(&data, sizeof(char), 1, src_file);
            }

            //Close the files
            fclose(src_file);
            fclose(dst_file);

            //Delete the source file if this is a morph
            if(operation == MORPH_OP)
            {
                remove(src);
            }

            //Print a line for the next command after a help command
            if(operation == HELP_OP)
            {
                fprintf(stdout, "\n");
            }
        }
        else
        {
            //Indicate that there was a problem with the src_file operation
            return -100;
        }
    }
    else
    {
        //Indicate that there was a problem with the src_file operation
        return -100;
    }

    //No errors to report
    return 0;
}

/*
 *
 * 
 */
int main (int argc, char ** argv)
{
    /* Declare and initialize constants */
    const char *prompt = "==>" ;	// shell prompt
    const char *help_path = "/home/justin/projects/Operating-Systems/project1/README.txt";
    
    /* Declare local variables */
    char  **arg;                    //working pointer thru args
    char   *args[MAX_ARGS];         //pointers to arg strings
    int     batch_input;            //is stdin a batch input file?
    char    buf[MAX_BUFFER];        //line buffer
    char    command[MAX_BUFFER];    //command output to the real
    char  **env;                    //environment 
    char    file_char;              //variable for reading readme one char at a time
    size_t  num_read;               //number of items read from file
    char    orig_input[MAX_BUFFER]; //the user's original input
    int     result;                 //return value of command arg
    char    work_dir[MAX_BUFFER];   //working directory of the program
    
    /* Initialize variables */
    result      = 0;
    batch_input = FALSE;
    env         = environ;
    work_dir[0] = '.';
    work_dir[1] = '/';
    work_dir[2] = '\0';

    //Check to see if there is a batch file being included as a program argument
    if(argc >= 2)
    {
        stdin = fopen(argv[1], "r");
        batch_input = TRUE;
    }
    //If stdin is not a terminal, it is a batch file.
    else if(!isatty(STDIN_FILENO))
    {
        batch_input = TRUE;
    }
    
    // Read command inputs until "quit" command or eof of redirected input
    while(!feof(stdin)) 
    {
        //Only show a prompt when no batch input is being used
        if(batch_input == FALSE)
        {
            //Write the prompt
            fputs(prompt, stdout);       
        }

    	//Read a line from stdin
        if(fgets(buf, MAX_BUFFER, stdin))
        {
            //Save the original input
            strcpy(orig_input, buf);

            //If a batch file is being read in, print the input
            if(batch_input == TRUE)
            {
                fprintf(stdout, "%s", orig_input);
            }

            //Given the input as a string, tokenize each input argument
            //Store each token in the args array by pointing to them with the arg pointers
            //Note: the last entry in the list will be NULL
            arg = args;

            //Set up the string tokenization
            *arg = strtok(buf,SEPARATORS);
            arg++;

            //Go through the current string until it ends
            while((*arg = strtok(NULL,SEPARATORS)))
            {
                arg++;
            }

            //If there are any arguments, then check for commands
            if(args[0]) 
    		{
                //esc -> quit program
                if (!strcmp(args[0],"esc")) 
                {
                    break;
                }
                //wipe -> clear
                else if (!strcmp(args[0],"wipe"))
                { 
                    result = system("clear");
                }
                //filez [target] -> ls -1 [target]
                else if(!strcmp(args[0], "filez"))
                {
                    command[0] = '\0';                   

                    //Form the command
                    strcat(command, "ls -1 ");
                    if(args[1] != NULL)
                    {
                        strcat(command, args[1]);
                    }
                    
                    //Show the files
                    result = system(command);
                }
                //environ -> env
                else if(!strcmp(args[0], "environ"))
                {
                    //Retrieve the environment
                    env = environ;

                    //Print out each environment variable line by line
                    while(*env)
                    {
                        fprintf(stdout, "%s\n", *env);
                        env++;
                    }
                }
                //ditto -> echo
                else if(!strcmp(args[0], "ditto"))
                {
                    command[0] = '\0';

                    //Form the echo command
                    strcat(command, "echo ");
                    strncpy(&orig_input[0], &orig_input[6], strlen(orig_input));
                    strcat(command, orig_input);

                    //Execute the echo command
                    result = system(command);
                }
                //help -> Print readme
                else if(!strcmp(args[0], "help"))
                {
                    result = file_operations(help_path, NULL, HELP_OP);
                }
                //mimic [src] [dst] -> cp [src] [dst] (not using system())
                else if(!strcmp(args[0], "mimic"))
                {
                    //Make sure that the source and destination are defined
                    if(args[1] != NULL && args[2] != NULL)
                    {
                        result = file_operations(args[1], args[2], MIMIC_OP);
                    }
                    else
                    {
                        fprintf(stderr, "Input Error: use format mimic [src] [dst]");
                    }
                }
                //erase [file] -> rm (not using system())
                else if(!strcmp(args[0], "erase"))
                {
                    //If a file/directory is specified, remove it
                    if(args[1] != NULL)
                    {
                        result = remove(args[1]);
                    }
                }
                //morph [src] [dst] -> mv [src] [dst] (not using system())
                else if(!strcmp(args[0], "morph"))
                {
                    //Make sure that the source and destination are defined
                    if(args[1] != NULL && args[2] != NULL)
                    {
                        file_operations(args[1], args[2], MORPH_OP);
                    }
                    else
                    {
                        fprintf(stderr, "Input Error: use format morph [src] [dst]");
                    }
                }
                //cd, chdir [directory] -> change to the target directory
                else if(!strcmp(args[0], "chdir") || !strcmp(args[0], "cd"))
                {
                    //Re-initialize the command string
                    command[0] = '\0';

                    //If the user wants to change directories to a certain target, they can
                    if(args[1] != NULL)
                    {
                        //Change the directory
                        result = chdir(args[1]);
                        
                        //If the chdir() succeeds, set the PWD variable
                        if(result == 0)
                        {
                            //Update the PWD variable
                            work_dir[0] = '\0';
                            strcat(work_dir, "PWD=");
                            strcat(work_dir, args[1]);
                            result =  putenv(work_dir);
                        }
                    }
                    //Otherwise, show the current directory contents
                    else
                    {
                        strcat(command, "ls -1");
                        result = system(command);
                    }
                }
                //Otherwise pass command onto OS
                else
                {
                    //Execute the unrecognized command
                    result = system(orig_input);
                }

                /* Error Handling */
                //If there was an error, print a notification to stderr
                if(result != 0)
                {
                    fprintf(stderr, "%s", strerror(errno));
                    result = 0;
                }
            }
        }
    }
    return 0; 
}