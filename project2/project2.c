#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//Custom headers
#include "error_handler.h"
#include "file_operations.h"
#include "global_constants.h"
#include "printables.h"

/* Declare the functions used */
int run_command(char **args, char *orig_input, int num_tokens);

/*
 * main - runs the majority of the program and passes more complicated stuff
 * to external functions
 * 
 * @param argc: number of program arguments
 * @param **argv: program arguments
 */
int main (int argc, char ** argv)
{
    /* Remove output buffering */
    setbuf(stdout, NULL);

    /* Declare local variables */
    char  **arg;            //working pointer thru args
    char   *args[MAX_ARGS]; //pointers to arg strings
    int     batch_input;    //is stdin a batch input file?
    char    buf[MAX_BUFFER];//line buffer
    int     num_tokens;     //number of tokens in command line input
    char   *orig_input;     //the user's original input
    
    /* Initialize variables */
    batch_input = FALSE;
    num_tokens  = 0;

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
    
    //If a batch file is not existant or is not a file, print an error and exit
    if(!stdin)
    {
        fprintf(stderr, "Error opening batch file: %s\n", strerror(errno));
        return 0;
    }

    // Read command inputs until "quit" command or eof of redirected input
    while(!feof(stdin)) 
    {
        //Write the prompt before input is read for user interaction
        if(batch_input == FALSE)
        {
            printPrompt();
        }

        //Read the input to a buffer for tokenization and a string for use
        orig_input = fgets(buf, MAX_BUFFER, stdin);

    	//Read a line from stdin
        if(orig_input)
        {
            //Reset the token counter
            num_tokens = 0;

            //Preserve the input before tokenization
            orig_input = strdup(buf);
            
            //If a batch file is being read in, print the prompt and input after it is read in
            if(batch_input == TRUE)
            {
                //Write the prompt
                printPrompt();

                //Write the input immediately
                fprintf(stdout, "%s", orig_input);
                fflush(stdout);
            }
            
            //Given the input as a string, tokenize each input argument
            //Store each token in the args array by pointing to them with the arg pointers
            //Note: the last entry in the list will be NULL
            arg = args;

            //Set up the string tokenization
            *arg = strtok(buf,SEPARATORS);
            arg++;
            num_tokens++;

            //Go through the current string until it ends
            while((*arg = strtok(NULL,SEPARATORS)))
            {
                arg++;
                num_tokens++;
            }

            //If there are any arguments, then run the command associated with it
            if(args[0]) 
    		{
                //Continue along with the program unless the command runner returns false
                if(!run_command(args, orig_input, num_tokens))
                {
                    //End the program
                    break;
                }
            }
        }
    }
    
    return 0; 
}

/***********************
    HELPER FUNCTIONS
***********************/

/*
 * run_command - takes a command in from the user and runs it
 * 
 * @param **args: list of args passed in by the user
 * @param  *orig_input: original program input
 * @param   num_tokens: number of tokens in the args array
 */
int run_command(char **args, char *orig_input, int num_tokens)
{
    /* Declare local variables */
    int result; //return value of command arg

    /* Initialize variables */
    result = 0;

    //cd, chdir [directory] -> change to the target directory
    if(!strcmp(args[0], "chdir"))
    {
        change_dir(args[1]);
    }
    //ditto -> echo
    else if(!strcmp(args[0], "ditto"))
    {
        //If there is something to echo, do it.
        if(args[1] != NULL)
        {
            ditto(args, num_tokens);
        }
    }
    //environ -> env
    else if(!strcmp(args[0], "environ"))
    {
        printEnv();
    }
    //erase [file] -> rm (not using system())
    else if(!strcmp(args[0], "erase"))
    {
        //If a file/directory is specified, remove it
        if(args[1] != NULL)
        {
            result = remove(args[1]);

            if(result != 0)
            {
                generalErrorHandler("erase");
                result = 0;
            }
        }
        else
        {
            fprintf(stderr, "Input Error: File target missing; use format erase [file].\n");
        }
    }
    //esc -> quit program
    else if (!strcmp(args[0],"esc")) 
    {
        //End the program
        return FALSE;
    }
    //filez [target] -> ls -1 [target]
    else if(!strcmp(args[0], "filez"))
    {
        filez(args, num_tokens);
    }
    //help -> Print readme
    else if(!strcmp(args[0], "help"))
    {
        help();
    }
    //mimic [src] [dst] -> cp [src] [dst] (not using system())
    else if(!strcmp(args[0], "mimic"))
    {
        //Make sure that the source and destination are defined
        if(args[1] != NULL && args[2] != NULL)
        {
            if(!strcmp(args[1], "-r") && args[3] != NULL)
            {
                morph_mimic(args[2], args[3], MIMIC_OP, TRUE);
            }
            else if(!strcmp(args[1], "-r"))
            {
                fprintf(stderr, "Input Error: recursive flag specified but no destination path provided. Use format mimic (-r) [src] [dst]\n");
            }
            else
            {
                morph_mimic(args[1], args[2], MIMIC_OP, FALSE);
            }
        }
        else
        {
            fprintf(stderr, "Input Error: use format mimic (-r) [src] [dst]");
        }
    }
    //mkdirz [path]
    else if(!strcmp(args[0],"mkdirz"))
    {
        //Only make a directory if a path is provided
        if(args[1] != NULL)
        {
            mkdirz(args, num_tokens);
        }
        else
        {
            fprintf(stderr, "Input Error: mkdirz needs a path as an argument. Use format mkdirz [path]\n");
        }
    }
    //morph [src] [dst] -> mv [src] [dst] (not using system())
    else if(!strcmp(args[0], "morph"))
    {
        //Perform morph operation iff both args aren't null
        if(args[1] != NULL && args[2] != NULL)
        {
            if(!strcmp(args[1], "-r") && args[3] != NULL)
            {
                morph_mimic(args[2], args[3], MORPH_OP, TRUE);
            }
            else if(!strcmp(args[1], "-r"))
            {
                fprintf(stderr, "Input Error: recursive flag specified but no destination path provided. Use format morph (-r) [src] [dst]\n");
            }
            else
            {
                morph_mimic(args[1], args[2], MORPH_OP, FALSE);
            }
        }
        else
        {
            fprintf(stderr, "Input Error: use format morph (-r) [src] [dst]\n");
        }
    }
    //rmdirz [path] -> rmdir [path]
    else if(!strcmp(args[0], "rmdirz"))
    {
        if(args[1] != NULL)
        {
            rmdirz(args, num_tokens);
        }
        else
        {
            fprintf(stderr, "Input Error: rmdirz needs a path in order to work. Use the format rmdirz [path]\n");
        }
    }
    //wipe -> clear
    else if (!strcmp(args[0],"wipe"))
    { 
        wipe();
    }
    //Otherwise pass command onto OS
    else
    {
        //Execute the unrecognized command
        general_command(args[0], NULL, 0, args, num_tokens);
    }

    /* Error Handling */
    //If there was an error, print a notification to stderr
    

    //Program should continue
    return TRUE;
}
