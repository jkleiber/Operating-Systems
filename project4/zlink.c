/**
Link two files in the OU File System.

CS 3113

*/

#include <stdio.h>
#include <string.h>

#include "oufs_lib.h"

int main(int argc, char **argv)
{
    /* Declare local variables */
    char cwd[MAX_PATH_LENGTH];      //current working directory
    char disk_name[MAX_PATH_LENGTH];//name of the virtual disk

    //Fetch the environment variables
    oufs_get_environment(cwd, disk_name);

    //Check arguments
    if (argc == 3)
    {
        //Open the virtual disk
        vdisk_disk_open(disk_name);

        //Link the two files if possible
        oufs_link(cwd, argv[1], argv[2]);

        //Clean up
        vdisk_disk_close();
    }
    else
    {
        //Wrong number of parameters
        fprintf(stderr, "Input error: correct usage is zlink <src> <dst>\n");
    }
}
