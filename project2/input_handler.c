#include "input_handler.h"

int parseInput(char *buf, char *args[MAX_BUFFER])
{
    //Declare local variables
    char  **arg;        //working pointer thru args
    int     num_tokens; //number of tokens in the command string

    //Initialize variables
    num_tokens = 0;

    //Given the input as a string, tokenize each input argument
    //Store each token in the args array by pointing to them with the arg pointers
    //Note: the last entry in the list will be NULL
    arg = args;

    //Set up the string tokenization
    *arg = strtok(buf, SEPARATORS);
    arg++;
    num_tokens++;

    //Go through the current string until it ends
    while((*arg = strtok(NULL, SEPARATORS)))
    {
        arg++;
        num_tokens++;
    }

    //Return the number of tokens in the input
    return num_tokens;
}