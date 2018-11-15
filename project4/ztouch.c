#include <stdio.h>
#include <string.h>

#include "oufs_lib.h"

int main(int argc, char **argv)
{
    /* Declare local variables */
    char cwd[MAX_PATH_LENGTH];
    char disk_name[MAX_PATH_LENGTH];
    OUFILE *file;

    //Get the current environment
    oufs_get_environment(cwd, disk_name);

    //Check arguments
    if (argc == 2)
    {
        //Open the virtual disk
        vdisk_disk_open(disk_name);

        // Make the specified file if possible
        file = oufs_fopen(cwd, argv[1], "w");

        //Close the file
        oufs_fclose(file);

        //Clean up
        vdisk_disk_close();
    }
    else
    {
        //Wrong number of parameters
        fprintf(stderr, "Input error: correct format is ztouch <file>\n");
    }
}