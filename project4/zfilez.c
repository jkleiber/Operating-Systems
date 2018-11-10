#include <stdio.h>
#include <string.h>

#include "oufs_lib.h"

int main(int argc, char **argv)
{
    /* Declare local variables */
    char cwd[MAX_PATH_LENGTH];      //env variable for current dir
    char disk_name[MAX_PATH_LENGTH];//env variable for virtual disk name
    char *name;

    //Initialize variables
    name = NULL;

    //Get the environment
    oufs_get_environment(cwd, disk_name);

    //Open the virtual disk
    vdisk_disk_open(disk_name);

    //Get a name argument if it exists
    if(argc > 1)
    {
        name = strdup(argv[1]);
    }

    //List the files
    oufs_list(cwd, name);

    //Close the disk
    vdisk_disk_close();

    //Free dynamic memory
    free(name);
    
    return 0;
}