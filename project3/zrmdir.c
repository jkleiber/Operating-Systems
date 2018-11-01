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
    
    return 0;
}