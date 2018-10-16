#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>

/* Macros */
#define FALSE 0
#define TRUE  1

//Mimic and Help commands
#define MIMIC_OP 1
#define HELP_OP  2

//README path
#define HELP_PATH "/projects/1/README.txt"

/* Pre-compiler constants */
#define MAX_ARGS 	64         // max # args
#define MAX_BUFFER 	1024       // max line buffer
#define SEPARATORS 	" \t\n"    // token sparators

/* External variables */
extern char **environ;         // environment array

/* Declare the functions used */
void change_dir(char *dst);
void ditto(char *input);
void file_operations(char *src, char *dst, int operation);
void filez(char *arg, char *input);
void generalErrorHandler(char *arg);
void morph(char *src, char *dst);
void printEnv();
int run_command(char **args, char *orig_input);

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

    /* Declare and initialize constant */
    const char *prompt = "==>" ;	// shell prompt
    
    /* Declare local variables */
    char  **arg;            //working pointer thru args
    char   *args[MAX_ARGS]; //pointers to arg strings
    int     batch_input;    //is stdin a batch input file?
    char    buf[MAX_BUFFER];//line buffer
    char   *orig_input;     //the user's original input
    
    /* Initialize variables */
    batch_input = FALSE;

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
            fputs(prompt, stdout);
        }

        //Read the input to a buffer for tokenization and a string for use
        orig_input = fgets(buf, MAX_BUFFER, stdin);

    	//Read a line from stdin
        if(orig_input)
        {
            //Preserve the input before tokenization
            orig_input = strdup(buf);
            
            //If a batch file is being read in, print the prompt and input after it is read in
            if(batch_input == TRUE)
            {
                //Write the prompt
                fputs(prompt, stdout);

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

            //Go through the current string until it ends
            while((*arg = strtok(NULL,SEPARATORS)))
            {
                arg++;
            }

            //If there are any arguments, then run the command associated with it
            if(args[0]) 
    		{
                //Continue along with the program unless the command runner returns false
                if(!run_command(args, orig_input))
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
 * change_dir - changes the directory of the program when user calls chdir.
 * Sets the PWD environment variable to make the command 'stick'
 * 
 * @param *dst: destination path to change to.
 */
void change_dir(char *dst)
{
    /* Declare local variables */
    int result;                //Result of any external operations
    char *work_dir; //Working directory placeholder variable

    /* Initialize variables */
    result = 0;

    if(dst == NULL)
    {
        //Print the working directory if it is found
        result = system("pwd");

        //Otherwise indicate an error occurred
        if(result)
        {
            fprintf(stderr, "Failed to get current working directory.\n");
        }
    }
    else
    {
        //Change the directory
        result = chdir(dst);
        
        //If the chdir() succeeds, set the PWD variable
        if(result == 0)
        {
            //Allocate and init the working directory
            work_dir = (char*)malloc((strlen(dst)+5) * sizeof(char));
            work_dir[0] = '\0';

            //Update the PWD variable
            strcat(work_dir, "PWD=");
            strcat(work_dir, dst);
            result =  putenv(work_dir);

            free(work_dir);
            work_dir = NULL;

            if(result)
            {
                generalErrorHandler("chdir");
            }
        }
        //chdir() failed, so notify user
        else
        {
            fprintf(stderr, "Changing the directory failed: %s\n", strerror(errno));
        }
    }
}

/*
 * ditto - outputs whatever the user wants to the shell.
 * 
 * @param *input: the full ditto command from user input
 */
void ditto(char *input)
{
    /* Declare local variables */
    char   *command;    //command string for system calls.
    int     result;     //error code tracker for operations

    /* Initialize variables */
    result = 0;

    //Protect against cases where the string is short somehow
    if(strlen(input) >= 7)
    {
        command = (char*)malloc((strlen(input) + 6)*sizeof(char));
        command[0] = '\0';

        //Form the echo command
        strcat(command, "echo ");
        strncpy(&input[0], &input[6], strlen(input));
        strcat(command, input);

        //Execute the echo command
        result = system(command);

        //Free dynamically allocated memory
        free(command);
        command = NULL;
    }

    if(result != 0)
    {
        generalErrorHandler("ditto");
    }
}

/*
 * file_operations - runs the mimic and help commands by reading files byte
 * by byte and either printing it to stdout or by copying the data to another
 * location on disk
 * 
 * @param *src: filepath to the source/README file.
 * @param *dst: filepath to the destination file (NULL if help command)
 * @param operation: the operation being performed (help or morph)
 */
void file_operations(char *src, char *dst, int operation)
{
    /* Declare local variables */
    char    data;       //data to move between files
    FILE   *dst_file;   //destination file pointer
    size_t  num_read;   //number of items read from file
    FILE   *src_file;   //source file pointer

    //Initialize dst file
    dst_file = NULL;

    //Open the source file
    src_file = fopen(src, "r");

    //If the source file exists, then create/open the dst file
    if(src_file)
    {
        //Create/open the dst file
        if(operation != HELP_OP)
        {
            dst_file = fopen(dst, "w");
        }
        
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

            //If the dst file is not NULL, close it.
            if(dst_file)
            {
                fclose(dst_file);
            }

            //Print a line for the next command after a help command
            if(operation == HELP_OP)
            {
                fprintf(stdout, "\n");
            }
        }
        else
        {
            //Indicate that there was a problem with the dst_file operation
            fprintf(stderr, "Mimic could not open target destination for writing\n");
        }
    }
    else
    {
        if(operation == HELP_OP)
        {
            fprintf(stderr, "README file could not be opened. System error: %s\n", strerror(errno));
        }
        else
        {
            fprintf(stderr, "Mimic could not open %s for reading. System error: %s\n", src, strerror(errno));
        }
    }
}

/*
 * filez - lists all files in a directory or lists the files the user wants to display
 * 
 * @param *arg: the first target the user wants to find (if any)
 * @param *input: the full line of input from the user
 */
void filez(char *arg, char *input)
{
    /* Declare local variables */
    char   *command;    //command string for system calls.
    int     result;     //error code tracker for operations

    /* Initialize variable */
    result = 0;

    //Allocate memory to the command variable and init
    command = (char*)malloc(7 * sizeof(char));
    command[0] = '\0';

    //Form the command
    strcat(command, "ls -1 ");
    if(arg != NULL)
    {
        command = (char*)realloc(command, (7 + strlen(input)) * sizeof(char));
        strncpy(&input[0], &input[6], strlen(input));
        strcat(command, input);
    }
    
    //Show the files
    result = system(command);

    //If the system has errors, alert the user
    if(result)
    {
        generalErrorHandler("filez");
    }

    //Deallocate command variable
    free(command);
    command = NULL;
}

/*
 * generalErrorHandler - custom error handler when the special commands run
 * into system errors. Converts errno to a nicer message and prints that to 
 * stderr as well
 * 
 * @param *arg: the command that generated the error
 */
void generalErrorHandler(char *arg)
{
    //Wipe error
    if(!strcmp(arg, "wipe"))
    {
        fprintf(stderr, "wipe failed: %s\n", strerror(errno));
    }
    //Filez error possibilities
    else if(!strcmp(arg, "filez"))
    {
        switch(errno)
        {
            case 0:
                fprintf(stderr, "filez unsuccessful on one or more files.\n");
                break;
            default:
                fprintf(stderr, "filez failed: %s\n", strerror(errno));
                break;
        }
    }
    //Ditto error
    else if(!strcmp(arg, "ditto"))
    {
        switch(errno)
        {
            case 0:
                fprintf(stderr, "ditto failed due to an incorrect input format or some general error.\n");
                break;
            default:
                fprintf(stderr, "ditto unsuccessful, system error: %s\n", strerror(errno));
                break;
        }
    }
    //Erase error
    else if(!strcmp(arg, "erase"))
    {
        fprintf(stderr, "unable to erase, error: %s\n", strerror(errno));
    }
    //Morph error
    else if(!strcmp(arg, "morph"))
    {
        fprintf(stderr, "morph not successful: %s\n", strerror(errno));
    }
    //Print general error if we don't know what command this is
    else
    {
        fprintf(stderr, "%s\n", strerror(errno));
    }
}

/*
 * morph - moves a file from one location to another
 * 
 * @param *src: path to the file to move
 * @param *dst: path to move the file to
 */
void morph(char *src, char *dst)
{
    /* Declare local variables */
    char   *command;    //command output to the system
    char   *name;       //file name
    int     result;     //result of commands interacting with OS

    /* Initialize variables */
    result = 0;

    //If args[2] is a directory, append the current file's name on the end.
    if(dst[strlen(dst) - 1] == '/')
    {
        //Get the file's non-path name
        name = basename(src);

        //Allocate command based on the file's name and dst path
        command = (char*)malloc((strlen(dst) + strlen(name) + 1) * sizeof(char));
        command[0] = '\0';

        //Add the directory path to command
        strcat(command, dst);

        //Add the file's name to the directory
        strcat(command, name);

        //Perform the move
        result = rename(src, command);

        //Free dynamic memory
        free(command);
        command = NULL;
    }
    else
    {
        //Duplicate the file string to command var
        command = strdup(dst);

        //Perform the move
        result = rename(src, command);

        //Free dynamic memory
        free(command);
        command = NULL;
    }

    //If there were errors, tell the user.
    if(result != 0)
    {
        generalErrorHandler("morph");
    }
}

/*
 * printEnv - Prints the system environment variables
 */
void printEnv()
{
    /* Declare local variables */
    char  **env;    //environment

    //Retrieve the environment
    env = environ;

    //Print out each environment variable line by line
    while(*env)
    {
        fprintf(stdout, "%s\n", *env);
        env++;
    }
}

/*
 * run_command - takes a command in from the user and runs it
 * 
 * @param **args: list of args passed in by the user
 * @param *orig_input: original program input
 */
int run_command(char **args, char *orig_input)
{
    /* Declare local variables */
    int result; //return value of command arg

    /* Initialize variables */
    result = 0;

    //esc -> quit program
    if (!strcmp(args[0],"esc")) 
    {
        //End the program
        return FALSE;
    }
    //wipe -> clear
    else if (!strcmp(args[0],"wipe"))
    { 
        result = system("clear");
    }
    //filez [target] -> ls -1 [target]
    else if(!strcmp(args[0], "filez"))
    {
        filez(args[1], orig_input);
    }
    //environ -> env
    else if(!strcmp(args[0], "environ"))
    {
        printEnv();
    }
    //ditto -> echo
    else if(!strcmp(args[0], "ditto"))
    {
        //If there is something to echo, do it.
        if(args[1] != NULL)
        {
            ditto(orig_input);
        }
    }
    //help -> Print readme
    else if(!strcmp(args[0], "help"))
    {
        file_operations(HELP_PATH, NULL, HELP_OP);
    }
    //mimic [src] [dst] -> cp [src] [dst] (not using system())
    else if(!strcmp(args[0], "mimic"))
    {
        //Make sure that the source and destination are defined
        if(args[1] != NULL && args[2] != NULL)
        {
            file_operations(args[1], args[2], MIMIC_OP);
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
        else
        {
            fprintf(stderr, "Input Error: File target missing; use format erase [file].\n");
        }
    }
    //morph [src] [dst] -> mv [src] [dst] (not using system())
    else if(!strcmp(args[0], "morph"))
    {
        //Perform morph operation iff both args aren't null
        if(args[1] != NULL && args[2] != NULL)
        {
            //run morph
            morph(args[1], args[2]);
        }
        else
        {
            fprintf(stderr, "Input Error: use format morph [src] [dst]\n");
        }
    }
    //cd, chdir [directory] -> change to the target directory
    else if(!strcmp(args[0], "chdir"))
    {
        change_dir(args[1]);
    }
    //Otherwise pass command onto OS
    else
    {
        //Execute the unrecognized command
        result = system(orig_input);
        
        //if the external program fails, let it output its own error message
        result = 0;
    }

    /* Error Handling */
    //If there was an error, print a notification to stderr
    if(result != 0)
    {
        generalErrorHandler(args[0]);
        result = 0;
    }

    //Program should continue
    return TRUE;
}
