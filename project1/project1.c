#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* Macros and pre-compiler constants */
#define MAX_BUFFER 	1024       // max line buffer
#define MAX_ARGS 	64         // max # args
#define SEPARATORS 	" \t\n"    // token sparators

/* External variables */
extern char **environ;         // environment array

/*
 *
 * 
 */
int main (int argc, char ** argv)
{
    /* Declare and initialize constants */
    const char *prompt = "==>" ;	// shell prompt 
    
    /* Declare local variables */
    char  **arg;            //working pointer thru args
    char   *args[MAX_ARGS]; //pointers to arg strings
    char    buf[MAX_BUFFER];//line buffer
    char  **env;            //environment variables
    int     result;         //return value of command arg   
    
    /* Initialize variables */
    result = 0;
    env    = environ;
    
    // Read command inputs until "quit" command or eof of redirected input
    while(!feof(stdin)) 
    {
        //Write the prompt
        fputs(prompt, stdout);       

    	//Read a line from stdin
        if(fgets(buf, MAX_BUFFER, stdin ))
        {
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
                //filez [target] -> ls -l [target]
                //TODO: fully implement filez to do targetted ls
                else if(!strcmp(args[0], "filez"))
                {
                    //TODO: implementation
                    result = system("ls -l");
                }
                //environ -> env
                else if(!strcmp(args[0], "environ"))
                {
                    env = environ;

                    while(*env)
                    {
                        fprintf(stdout, "%s\n", *env);
                        env++;
                    }
                }
                //ditto -> echo (using fprintf here)
                else if(!strcmp(args[0], "ditto"))
                {
                    //TODO: implementation
                }
                //help
                else if(!strcmp(args[0], "help"))
                {
                    //TODO: implementation
                }
                //mimic [src] [dst] -> cp [src] [dst]
                else if(!strcmp(args[0], "mimic"))
                {
                    //TODO: implementation
                }
                //erase [file] -> rm (not using system())
                else if(!strcmp(args[0], "erase"))
                {
                    //TODO: implementation
                }
                //morph [src] [dst] -> mv [src] [dst] (not using system)
                else if(!strcmp(args[0], "morph"))
                {
                    //TODO: implementation
                }
                //chdir [directory] -> cd [directory]
                else if(!strcmp(args[0], "chdir"))
                {
                    //TODO: implementation
                    //use putenv()
                }
                //Otherwise pass command onto OS (or in this instance, print them out)
                else
                {
                    result = system(args[0]);
                    //arg = args;
                    //while(*arg) fprintf(stdout,"%s ",*arg++);
                    //fputs("\n", stdout);
                }

                /* Error Handling */
                //If there was an error, print a notification to stderr
                if(result != 0)
                {
                    //fprintf(stdout, "%s%d", args[0], result);
                    result = 0;
                }
            }
        }
    }
    return 0; 
}