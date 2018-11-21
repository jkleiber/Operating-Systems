#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "oufs_lib.h"

#define MAX_BUFFER 255

int main(int argc, char **argv)
{
    /* Declare local variables */
    unsigned char   c;
    char            cwd[MAX_PATH_LENGTH];
    char            disk_name[MAX_PATH_LENGTH];
    OUFILE         *file;

    //Get the current environment
    oufs_get_environment(cwd, disk_name);

    //Check arguments
    if (argc == 2)
    {
        //Open the virtual disk
        vdisk_disk_open(disk_name);

        //Open the file
        file = oufs_fopen(cwd, argv[1], "a");

        //Read first character
        c = getchar();

        //Read input from stdin and append to the file
        while(!feof(stdin))
        {
            //If stdin has input, add this to the end of the file
            if(c != EOF)
            {
                oufs_fwrite(file, &c, 1);
            }

            //Get the next character
            c = getchar();
        }

        //Close the file
        oufs_fclose(file);

        //Clean up
        vdisk_disk_close();
    }
    else
    {
        //Wrong number of parameters
        fprintf(stderr, "Input error: correct format is zcreate <file>\n");
    }
}