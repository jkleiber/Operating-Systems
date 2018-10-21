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
 *
 */
void dfs_copy(char *src, char *dst, char *mode)
{
    /* Declare local variables */
    char           *cur_path;
    int             cur_path_len;
    char           *dst_path;
    struct dirent  *file_reader;
    DIR            *src_dir;
    
    //Initialize variables
    cur_path = strdup(src);
    cur_path_len = strlen(cur_path);

    //Open the source directory
    src_dir = opendir(src);

    //Read through the directory
    while((file_reader = readdir(src_dir)))
    {
        //Only process items that are not the . or the .. special directories
        if(strcmp(file_reader->d_name, ".") && strcmp(file_reader->d_name, ".."))
        {
            //Reallocate memory and concatenate the new file name onto the current path
            cur_path = (char*)realloc(cur_path, cur_path_len + strlen(file_reader->d_name) + 1);
            strcat(cur_path, file_reader->d_name);

            //If the listed item is a file, morph/mimic it correctly
            if(get_type(cur_path) == REGULAR_FILE)
            {

            }
            //If the listed item is a directory, recursively step into it
            else
            {
                //Make a destination path
                dst_path = strdup(dst);
                dst_path = (char*)realloc(dst_path, strlen(dst) + strlen(file_reader->d_name) + 2);
                strcat(dst_path, file_reader->d_name);
                strcat(dst_path, "/");

                //Make a directory on the other end
                
                //Step into the directory
                dfs_copy(cur_path, dst_path, mode);
            }
        }
    }
}

/*
 * 
 */
int copy_file(char *src, char *dst)
{
    /* Declare local variables */
    char c;
    FILE *dst_file;
    FILE *src_file;

    //Open the source file
    src_file = fopen(src, "r");

    //If the source file opened fine, open the destination
    if(src_file)
    {
        //Open the dst file
        dst_file = fopen(dst, "w");

        //If the destination file opened fine, perform the copy
        if(dst_file)
        {
            //Read a byte from source and write it to dst until the EOF is found
            while(!feof(src_file))
            {
                //If the read works, then write to dst
                if(!fread(&c, sizeof(char), 1, src_file))
                {
                    //If the write fails, return an error
                    if(fwrite(&c, sizeof(char), 1, dst_file))
                    {
                        return -1;
                    }
                }
                //If the read fails, return an error
                else
                {
                    return -1;
                }
            }
        }
    }

    //No errors, return 0
    return 0;
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
    if(general_command("ls", flags, 1, args, num_tokens, NULL))
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
int is_empty_dir(char *path)
{
    /* Declare local variables */
    DIR            *dir;            //directory pointer (to read and test existence)
    struct dirent  *d;              //dirent operator for reading directories
    int             num_entries;    //number of entries in a directory

    //Initialize variables
    num_entries = 0;

    //Check to see if the dst directory is empty
    dir = opendir(path);

    //If the directory doesn't exist, it is not empty for our purposes
    if(!dir)
    {
        return FALSE;
    }
    else
    {
        //Read through the elements in the directory and count them
        while((d = readdir(dir)))
        {
            num_entries++;

            //Once more than two entries are counted (. and ..) the directory is not empty.
            if(num_entries > 2)
            {
                closedir(dir);
                return FALSE;
            }
        }

        //Close the directory
        closedir(dir);
    }

    //If the function reaches this point, the directory must be empty
    return TRUE;
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
        if(general_command("mkdir", NULL, 0, args, num_tokens, NULL))
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
 * @param *mode: which operation to perform (morph or mimic?)
 * @param recursive: should the copy/move be recursive?
 */
void morph_mimic(char *src, char *dst, char* mode, int recursive)
{
    /* Declare local variables */
    char       *flags[1];
    char       *dst_path;   //path to destination file/directory
    int         dst_type;   //type of item the dst is
    char       *name;       //file name
    int         result;     //result of commands interacting with OS
    int         src_type;   //type of item the src is

    /* Initialize variables */
    result = 0;

    //Confirm the source file/directory exists. If it does not, print an error and exit
    result = access(src, F_OK);
    if(!result)
    {
        fprintf(stderr, "Error: source file/directory %s does not exist\n", src);
        return;
    }

    //Get the type of item that source is
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

    //Get the file type of dst
    dst_type = get_type(dst);

    //If source isn't a file...
    if(!src_type)
    {
        //If source is a directory and dest is a file, this is an error
        if(dst_type == REGULAR_FILE)
        {
            fprintf(stderr, "Error: cannot %s a directory to a file. Try using a destination that is a directory.\n", mode);
        }
        //Copy the src directory into the dst directory
        else if(dst_type == DIRECTORY || dst_type == OTHER)
        {
            //Find the actual target path
            //Check to see if the directory exists
            result = access(dst, F_OK);

            //Duplicate the dst path into dst path
            dst_path = strdup(dst);

            //If the directory exists, add the current directory's name to the path
            if(result)
            {
                //Get current directory name
                name = basename(src);

                //If the dst path has an ending slash, allocate additional memory for the current directory's name only
                if(dst[strlen(dst) - 1] == '/')
                {
                    dst_path = (char*)realloc(dst_path, strlen(dst) + strlen(name) + 1);
                }
                //Otherwise do the same, but account for the missing slash
                else
                {
                    dst_path = (char*)realloc(dst_path, strlen(dst) + strlen(name) + 1);
                    strcat(dst_path, "/");
                }
            }

            //Only do recursive dir -> dir copy/move if the recursive flag is set.
            if(recursive)
            {
                //Perform a recursive copy using depth first search
                dfs_copy(src, dst_path, mode);
            }
            //If the src directory is empty, copy/move it over
            else if(is_empty_dir(src))
            {
                //If this is morph, just rename the directory
                if(!strcmp(mode, "morph"))
                {
                    rename(src, dst_path);
                }
                //Otherwise the directory should be copied over to the new path.
                else
                {
                    flags[1] = strdup(dst_path);
                    if(general_command("mkdir", flags, 1, NULL, 0, NULL))
                    {
                        generalErrorHandler(mode);
                    }
                }
            }
        }
        //If recursive is specified, create a new directory and copy/move the files
        else
        {
            fprintf(stderr, "File mismatch error: Directory to directory %s needs recursive flag.\n", mode);
        }
    }
    //If source is a file...
    else
    {
        //If the destination is a file, perform a straight copy
        if(dst_type == REGULAR_FILE)
        {
            //If the command is mimic, copy the src file to dst
            if(!strcmp(mode, "mimic"))
            {
                result = copy_file(src, dst);
            }
            //If the command is morph, move the src file to dst
            else
            {
                result = rename(src, dst);
            }
        }
        //If the destination is a directory, we need to create the file first
        //and then perform a normal copy
        else
        {
            //Get the source file's name (removing rest of path)
            name = basename(src);

            //duplicate dst into the dst path variable
            dst_path = strdup(dst);

            //Get the directory path of dst
            //If the dst directory ends in slash, malloc the length of dst and the name length
            if(dst[strlen(dst) - 1] == '/')
            {
                dst_path = (char*)realloc(dst_path, strlen(dst) + strlen(name) + 1);
            }
            //Otherwise do the same, but account for the missing slash
            else
            {
                dst_path = (char*)realloc(dst_path, strlen(dst) + strlen(name) + 1);
                strcat(dst_path, "/");
            }

            //Add the current file name to the path
            strcat(dst_path, name);

            //If this is mimic, perform the copy
            if(!strcmp(mode, "mimic"))
            {
                result = copy_file(src, dst_path);
            }
            else
            {
                result = rename(src, dst_path);
            }

            //Free dynamically allocated memory
            free(dst_path);
        }
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
    if(general_command("rmdir", NULL, 0, args, num_tokens, NULL))
    {
        //If the command failed, print an error
        generalErrorHandler("rmdirz");
    }
}