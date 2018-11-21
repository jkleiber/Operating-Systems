#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "oufs_lib.h"

#define MAX_BUFFER 255

int main(int argc, char **argv)
{
    /* Declare local variables */
    unsigned char   buf[MAX_BUFFER + 1];
    char            cwd[MAX_PATH_LENGTH];
    char            disk_name[MAX_PATH_LENGTH];
    int             num_bytes;
    OUFILE         *file;

    //Get the current environment
    oufs_get_environment(cwd, disk_name);

    //Check arguments
    if (argc == 2)
    {
        //Open the virtual disk
        vdisk_disk_open(disk_name);

        //Open the file
        file = oufs_fopen(cwd, argv[1], "r");

        //Read the file if it opened fine
        if(file)
        {
            //Read the file and output its contents to stdout
            while((num_bytes = oufs_fread(file, buf, MAX_BUFFER)) > 0)
            {
                //NULL terminate the buffer
                buf[num_bytes] = '\0';

                //Print the file contents
                fprintf(stdout, "%s", buf);
                fflush(stdout);
            }

            //Close the file
            oufs_fclose(file);
        }
        //IF the file didn't open fine, print an error
        else
        {
            fprintf(stderr, "File could not be opened for reading\n");
        }

        //Clean up
        vdisk_disk_close();
    }
    else
    {
        //Wrong number of parameters
        fprintf(stderr, "Input error: correct format is zmore <file>\n");
    }
}