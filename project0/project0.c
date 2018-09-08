#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CLOSING_CURLY_BRACE 0x7d
#define NUM_VALID_CHARS 256
#define SPACE 0x20
#define WIDTH 4

/*
 * Adds a character to the occurences array if it is in bounds.
 * 
 * @param c: character to add to the array
 * @param *occurences: integer array of the number of times a character is seen
 */
void add_char(unsigned char c, int* occurences)
{
    /* Declare variable */
    int index; //index in occurences array to increment

    //Find the index in the array based on the offset from the SPACE character
    index = (int)(c - 0);

    //If the character is valid, increment the occurences counter
    if(index >= 0
    && index <= NUM_VALID_CHARS)
    {
        *(occurences + index) += 1;
    }
}

/*
 * This function prints N # characters.
 * 
 * @param num_hashtags: number of # characters to print (N)
 */
void print_hashtags(int num_hashtags)
{
    /* Declare variable */
    int i;  //loop iteration

    //Print the number of hashtags requested
    for(i = 0; i < num_hashtags; ++i)
    {
        fprintf(stdout, "#");
    }
}

/*
 * The main function. Sets up the program, reads the input and formats the output.
 * 
 * @param argc: the number of program arguments passed from the terminal
 * @param **argv: the program arguments passed from the terminal
 */
int main(int argc, char** argv)
{
    /* Declare variables */
    unsigned char c;                                //iteration variable for looping through characters
    unsigned char cur_char;                         //current character read from 
    int           index;                            //index of character in the occurences array
    unsigned char lower_char;                       //lowest character in ASCII table to check for in file
    int           num_occurences[NUM_VALID_CHARS];  //number of occurences of each character in the file
    unsigned char upper_char;                       //highest character in ASCII table to check for in file

    /* Initialize variables */
    upper_char = SPACE;                 //SPACE character is default
    lower_char = CLOSING_CURLY_BRACE;   //'}' is the default

    //Initialize occurences array to 0 for all values.
    memset(num_occurences, 0, sizeof(num_occurences));

    //If the upper character argument is specified, then set it
    if(argc == 2)
    {
        upper_char = argv[1][0];
    }
    //If both the upper and lower character arguments are specified, set new boundaries
    else if(argc == 3)
    {
        upper_char = argv[1][0];
        lower_char = argv[2][0];
    }
    //If an invalid configuration was provided, exit early
    else if(argc != 1)
    {
        //fprintf(stderr, "Invalid input configuration\n");
        exit(-1);
    }

    //Don't bother reading in the file unless the upper char is equal or lesser than lower char.
    //Upper char needs to be greater than 0 to be on the ASCII table
    if(upper_char <= lower_char
    && upper_char >= 0)
    {
        //Prepare to read file
        cur_char = getchar();

        //Read files from stdin (piped in from console)
        while(!feof(stdin))
        {
            if(cur_char != EOF)
            {
                //Check to see if the character falls into a valid range in the tracking array
                add_char(cur_char, num_occurences);
            }

            //Read next character
            cur_char = getchar();
        }
    }

    //Print header information
    fprintf(stdout, "Range: %c-%c\n", upper_char, lower_char);

    //Only perform additional output if the upper character is equal to or comes after the lower character.
    //Also, the upper char should be at least 0 to avoid out of bounds errors
    if(upper_char <= lower_char
    && upper_char >= 0)
    {
        //Loop through the array and print output
        for(c = upper_char; c <= lower_char; ++c)
        {
            //Convert the character to a numeric index
            index = (int)(c - 0);

            //Print the number of occurences in a right-aligned way with 4 characters
            fprintf(stdout, "%c: %*d ", c, WIDTH, num_occurences[index]);

            //Print the number of hashtags that corresponds to the number of occurences of the character
            print_hashtags(num_occurences[index]);

            //Newline
            fprintf(stdout, "\n");
        }
    }
    
    return 0;
}
