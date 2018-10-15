#include "file_operations.h"

/*
 * change_dir - changes the directory of the program when user calls chdir.
 * Sets the PWD environment variable to make the command 'stick'
 * 
 * @param *dst: destination path to change to.
 */
void change_dir(char *dst)
{
    /* Declare local variables */
    int result;                //Result of any external operations
    char *work_dir; //Working directory placeholder variable

    /* Initialize variables */
    result = 0;

    if(dst == NULL)
    {
        //Print the working directory if it is found
        printCWD();
    }
    else
    {
        //Change the directory
        result = chdir(dst);
        
        //If the chdir() succeeds, set the PWD variable
        if(result == 0)
        {
            //Allocate and init the working directory
            work_dir = (char*)malloc((strlen(dst)+5) * sizeof(char));
            work_dir[0] = '\0';

            //Update the PWD variable
            strcat(work_dir, "PWD=");
            strcat(work_dir, dst);
            result =  putenv(work_dir);

            free(work_dir);
            work_dir = NULL;

            if(result)
            {
                generalErrorHandler("chdir");
            }
        }
        //chdir() failed, so notify user
        else
        {
            fprintf(stderr, "Changing the directory failed: %s\n", strerror(errno));
        }
    }
}

/*
 * filez - lists all files in a directory or lists the files the user wants to display
 * 
 * @param *arg: the first target the user wants to find (if any)
 * @param *input: the full line of input from the user
 */
void filez(char **args, int num_tokens)
{
    /* Declare local variables */
    char *flags[1] = {"-1"};  //flag for the ls command

    //If the system has errors, alert the user
    if(general_command("ls", flags, 1, args, num_tokens))
    {
        generalErrorHandler("filez");
    }
}

/*
 *
 */
int get_type(const char *path) 
{
    //Declare local variables
    struct stat statbuf;    //stats about a file/directory

    //If stat fails, return -1 to indicate an error occurred
    if(stat(path, &statbuf) != 0)
    {
        return -1;
    }

    if(S_ISREG(statbuf.st_mode))
    {
        return REGULAR_FILE;
    }
    else if(S_ISDIR(statbuf.st_mode))
    {
        return DIRECTORY;
    }

    //Return if a file is a directory or not
    return OTHER;
}

/*
 *
 */
void mkdirz(char **args, int num_tokens)
{
    //Declare local variables
    char   *dir;        //parent directory of the new directory
    int     dir_type;   //type of file the base path is
    char   *full_path;  //original path to target
    
    //Initialize variables
    full_path = strdup(args[1]);

    //Get the parent directory of the new folder and its type
    dir = dirname(full_path);
    dir_type = get_type(dir);

    //If the target directory will be valid, create the directory
    if(dir_type != REGULAR_FILE && dir_type != -1)
    {
        if(general_command("mkdir", NULL, 0, args, num_tokens))
        {
            generalErrorHandler("mkdirz");
        }
    }
    //If the target directory is a regular file, print an error
    else if(dir_type == REGULAR_FILE)
    {
        fprintf(stderr, "mkdirz error: cannot create directory inside regular file\n");
    }
    //Otherwise the target directory was not accessible, so print an error
    else
    {
        fprintf(stderr, "mkdirz error: cannot access target directory. System error: %s", strerror(errno));
    }
}

/*
 * morph_mimic - two file and directory organizing functions in one
 * morph: moves an item to another location
 * mimic: copies an item in one location to another location, making 
 *        two separate items
 * 
 * @param *src: path to the file to move
 * @param *dst: path to move the file to
 * @param mode: which operation to perform (morph or mimic?)
 * @param recursive: should the copy/move be recursive?
 */
void morph_mimic(char *src, char *dst, int mode, int recursive)
{
    /* Declare local variables */
    char   *command;    //command output to the system
    int     dst_type;   //type of item the dst is
    char   *name;       //file name
    int     result;     //result of commands interacting with OS
    int     src_type;   //type of item the src is

    /* Initialize variables */
    result   = 0;
    src_type = get_type(src);

    //Check to see if the source item is accessible.
    //If inaccessible print an error
    if(src_type == -1)
    {
        fprintf(stderr, "Source item not accessible: %s", strerror(errno));
        return;
    }
    //If the item is accessible, classify it as a file (1) or not a file (0)
    else
    {
        src_type = src_type == REGULAR_FILE ? 1 : 0;
    }

    //If source isn't a file...
    if(!src_type)
    {

    }
    //If source is a file...
    else
    {

    }

    //If args[2] is a directory, append the current file's name on the end.
    if(get_type(dst) == DIRECTORY)
    {
        //Get the file's non-path name
        name = basename(src);

        //Allocate command based on the file's name and dst path
        command = (char*)malloc((strlen(dst) + strlen(name) + 1) * sizeof(char));
        command[0] = '\0';

        //Add the directory path to command
        strcat(command, dst);

        //Add the file's name to the directory
        strcat(command, name);

        //Perform the move
        result = rename(src, command);

        //Free dynamic memory
        free(command);
        command = NULL;
    }
    else
    {
        //Duplicate the file string to command var
        command = strdup(dst);

        //Perform the move
        result = rename(src, command);

        //Free dynamic memory
        free(command);
        command = NULL;
    }

    //If there were errors, tell the user.
    if(result != 0)
    {
        generalErrorHandler("morph");
    }
}

/*
 *
 */
void rmdirz(char **args, int num_tokens)
{
    //Run the rmdir command
    if(general_command("rmdir", NULL, 0, args, num_tokens))
    {
        //If the command failed, print an error
        generalErrorHandler("rmdirz");
    }
}