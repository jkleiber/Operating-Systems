#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CLOSING_CURLY_BRACE 0x7d
#define NUM_VALID_CHARS 95
#define SPACE 0x20

void add_char(char c, int* occurences)
{
    /* Declare variable */
    int index; //Index in occurences array to increment

    //Find the index in the array based on the offset from the SPACE character
    index = (int)(c - SPACE);

    //If the character is valid, increment the occurences counter
    if(index >= 0
    && index <= NUM_VALID_CHARS)
    {
        *(occurences + index) += 1;
    }
}

int main(int argc, char** argv)
{
    /* Declare variables */
    char c;                                 //iteration variable for looping through characters
    char cur_char;                          //Current character read from 
    int  index;                             //index of character in the occurences array
    char lower_char;                        //Lowest character in ASCII table to check for in file
    int  num_occurences[NUM_VALID_CHARS];   //number of occurences of each character in the file
    char upper_char;                        //Highest character in ASCII table to check for in file
    
    /* Initialize variables */
    upper_char = SPACE; //SPACE character is default
    lower_char = CLOSING_CURLY_BRACE; //'}' is the default
    memset(num_occurences, 0, sizeof(num_occurences));

    //If the upper bound character argument is specified, then set the upper bound
    if(argc == 2)
    {
        upper_char = argv[1][0];
    }
    //If both the upper and lower bound program arguments are specified, set the new boundaries
    else if(argc == 3)
    {
        upper_char = argv[1][0];
        lower_char = argv[2][0];
    }
    //If an invalid configuration was provided, exitg
    else if(argc != 1)
    {
        fprintf(stderr, "Invalid input configuration\n");
        exit(-1);
    }

    //Prepare to read file
    cur_char = getchar();

    //Read files from stdin (piped in from console)
    while(cur_char != EOF)
    {
        //Check to see if the character falls into a valid range in the tracking array
        add_char(cur_char, num_occurences);

        //Read next character
        cur_char = getchar();
    }

    //Print header information
    fprintf(stderr, "Range: %c-%c\n", upper_char, lower_char);

    //Loop through the array and print output
    for(c = upper_char; c <= lower_char; ++c)
    {
        index = c - SPACE;

        fprintf(stderr, "%c: %d ->hashtags\n", c, num_occurences[index]);
    }

    return 0;
}
