#include "printables.h"

/* Static constants */
static const char *prompt = "==>";	// shell prompt

/*
 * ditto - outputs whatever the user wants to the shell.
 * 
 * @param *input: the full ditto command from user input
 */
void ditto(char **args, int num_tokens)
{
    //Run an echo command on the system to print the tokens
    if(general_command("echo", NULL, 0, args, num_tokens, NULL))
    {
        //Handle any errors that may come up
        generalErrorHandler("ditto");
    }
}

/*
 * help - runs the help command by reading the README byte
 * by byte and printing its contents to to stdout
 */
void help()
{
    /* Declare local variables */
    char    data;       //data to move between files
    FILE   *readme;     //destination file pointer
    size_t  num_read;   //number of items read from file

    //Open the README.txt file
    readme = fopen(HELP_PATH, "r");

    //Print the readme if it was able to be opened
    if(readme)
    {
        //Read the source file until EOF
        while(num_read > 0 && !feof(readme))
        {
            //Print the contents of the README.txt file
            fprintf(stdout, "%c", data);
            fflush(stdout);

            //Read more
            num_read = fread(&data, sizeof(char), 1, readme);
        }

        //Close the README
        fclose(readme);

        fprintf(stdout, "\n");
        
    }
    else
    {
        fprintf(stderr, "README file could not be opened. System error: %s\n", strerror(errno));
    }
}

/*
 *
 */
void printCWD()
{
    //Declare local variables
    int     cwd_length; //length of the current working directory string
    char   *cwd;        //current working directory string

    //Find the current path size
    cwd_length = pathconf(".", _PC_PATH_MAX);

    //Allocate memory for the cwd string
    cwd = (char*)malloc(cwd_length*sizeof(char));

    //Get the current directory and print it to the console
    if(getcwd(cwd, cwd_length))
    {
        fputs(cwd, stdout);
    }
    else
    {
        fprintf(stderr, "Failed to get current working directory.\n");
    }

    //free the dynamically allocated memory
    free(cwd);
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
 *
 */
void printPrompt(int just_cwd)
{
    printCWD();

    //Print the prompt characters
    fputs(prompt, stdout);
}

/*
 *
 */
void wipe()
{
    //Send the system a command to clear the screen
    if(general_command("clear", NULL, 0, NULL, 1, NULL))
    {
        //Handle any errors that occur when clearing the screen
        generalErrorHandler("wipe");
    }
}