#include <stdio.h>
#include <string.h>

#include "oufs_lib.h"

int main(int argc, char **argv)
{
    /* Declare local variables */
    char cwd[MAX_PATH_LENGTH];      //env variable for current dir
    char disk_name[MAX_PATH_LENGTH];//env variable for virtual disk name
    
    //Get the environment
    oufs_get_environment(cwd, disk_name);

    // Check arguments
    if (argc == 2)
    {
        // Open the virtual disk
        vdisk_disk_open(disk_name);

        // Remove the specified directory
        oufs_rmdir(cwd, argv[1]);

        // Clean up
        vdisk_disk_close();
    }
    else
    {
        // Wrong number of parameters
        fprintf(stderr, "Input error: use zrmdir <dirname>\n");
    }
    
    return 0;
}