#include <stdlib.h>
#include <libgen.h>
#include "oufs_lib.h"

#define debug 0

/**
 * Read the ZPWD and ZDISK environment variables & copy their values into cwd and disk_name.
 * If these environment variables are not set, then reasonable defaults are given.
 *
 * @param cwd String buffer in which to place the OUFS current working directory.
 * @param disk_name String buffer containing the file name of the virtual disk.
 */
void oufs_get_environment(char *cwd, char *disk_name)
{
    // Current working directory for the OUFS
    char *str = getenv("ZPWD");
    if (str == NULL)
    {
        // Provide default
        strcpy(cwd, "/");
    }
    else
    {
        // Exists
        strncpy(cwd, str, MAX_PATH_LENGTH - 1);
    }

    // Virtual disk location
    str = getenv("ZDISK");
    if (str == NULL)
    {
        // Default
        strcpy(disk_name, "vdisk1");
    }
    else
    {
        // Exists: copy
        strncpy(disk_name, str, MAX_PATH_LENGTH - 1);
    }
}

/**
 * Formats the virtual disk so it can be used by the system
 * 
 * @param *virtual_disk_name: name of disk to format
 * @return: 0 if successful
 */
int oufs_format_disk(char *virtual_disk_name)
{
    /* Declare local variables */
    BLOCK block;               //Blocks to format
    BLOCK_REFERENCE block_ref; //references to format

    //Set everything in the block and inode to 0
    memset(block.data.data, 0, BLOCK_SIZE);

    //Loop through all bytes to set the bytes to 0.
    for (block_ref = MASTER_BLOCK_REFERENCE; block_ref < N_BLOCKS_IN_DISK; ++block_ref)
    {
        if (vdisk_write_block(block_ref, &block))
        {
            return -1;
        }
    }

    //Initialize the master block
    block.master.block_allocated_flag[MASTER_BLOCK_REFERENCE] = 0xff;
    //block.master.inode_allocated_flag[0] = 0x01;

    //Write the master block to the disk
    if (vdisk_write_block(MASTER_BLOCK_REFERENCE, &block))
    {
        return -1;
    }

    //Allocate block 8 as well to account for the master block allocating 0 - (N_INODE_BLOCKS - 1)
    oufs_allocate_new_block();

    //Initialize the first inode
    oufs_allocate_new_inode(0, IT_DIRECTORY);

    return 0;
}

/**
 *  Given an inode reference, read the inode from the virtual disk.
 *
 *  @param i Inode reference (index into the inode list)
 *  @param inode Pointer to an inode memory structure.  This structure will be
 *                filled in before return)
 *  @return 0 = successfully loaded the inode
 *         -1 = an error has occurred
 *
 */
int oufs_read_inode_by_reference(INODE_REFERENCE i, INODE *inode)
{
    if (debug)
        fprintf(stderr, "Fetching inode %d\n", i);

    // Find the address of the inode block and the inode within the block
    BLOCK_REFERENCE block = i / INODES_PER_BLOCK + 1;
    int element = (i % INODES_PER_BLOCK);

    BLOCK b;
    if (vdisk_read_block(block, &b) == 0)
    {
        // Successfully loaded the block: copy just this inode
        *inode = b.inodes.inode[element];
        return (0);
    }
    // Error case
    return (-1);
}

/**
 * Writes an inode to the file system
 * 
 * @param i: reference to write inode to
 * @param *inode: data to write
 * @return: 0 on success, non-zero on failure
 */
int oufs_write_inode_by_reference(INODE_REFERENCE i, INODE *inode)
{
    /* Declare local variables */
    BLOCK block;               //block to update
    BLOCK_REFERENCE block_ref; //reference to the block needing update
    int element;               //element to update in the block

    // Find location in block
    block_ref = (i / INODES_PER_BLOCK) + 1;
    element = (i % INODES_PER_BLOCK);

    //Read in the inode block
    if (!vdisk_read_block(block_ref, &block))
    {
        // Load the inode into the block
        block.inodes.inode[element] = *inode;

        // Attempt to write the inode to the block
        if (vdisk_write_block(block_ref, &block))
        {
            return -1;
        }
    }
    //IF the read failed, throw error
    else
    {
        return -1;
    }

    //return success
    return 0;
}

/**
 * Finds a file based on a cwd and path, and returns its parent inode reference, 
 * and its block reference and its directory entry.
 * 
 * @param *cwd: current working directory
 * @param *path: path to file to try to find
 * @param *parent: parent inode reference to file
 * @param *child: block reference of the file
 * @param *dir_entry: directory entry details of a file
 * @return: positive if path does not exist, negative if file cannot be found and path exists, 0 if found 
 */
int oufs_find_file(char *cwd, char *path, INODE_REFERENCE *parent, BLOCK_REFERENCE *child, DIRECTORY_ENTRY *dir_entry)
{
    /* Declare local variables */
    char *abs_path;                 //absolute path to a file
    BLOCK_REFERENCE block_ref;      //reference to a block the file may be in
    int missing_tokens;             //missing tokens in a given path
    INODE_REFERENCE next_inode_ref; //the next inode reference in the sequence
    char *token;                    //token for path string

    //Initialize tree walking
    *child = MASTER_BLOCK_REFERENCE;
    *parent = 0;
    missing_tokens = 0;

    //Concatenate the name onto the cwd if the name exists
    if (path != NULL)
    {
        //If the input name is an absolute path use it instead
        if (path[0] == '/')
        {
            abs_path = strdup(path);
        }
        //otherwise add the path to the cwd
        else
        {
            abs_path = strdup(cwd);

            //Make sure the current directory has a slash at the end
            if (abs_path[strlen(abs_path) - 1] != '/')
            {
                abs_path = (char *)realloc(abs_path, (strlen(abs_path) + strlen(path) + 2) * sizeof(char));
                strncat(abs_path, "/", 2);
            }

            strcat(abs_path, path);
        }
    }
    //Otherwise just list the current directory
    else
    {
        abs_path = strdup(cwd);
    }

    //Break the path into tokens
    token = strtok(abs_path, "/");

    //Only attempt to process non-zero paths
    if (token)
    {
        //Get the next inode reference
        next_inode_ref = get_next_inode(*parent, token, &block_ref, dir_entry);

        //Detect if the first inode does not exist
        if (next_inode_ref == UNALLOCATED_INODE)
        {
            missing_tokens++;
        }

        //Find the associated inode for each token
        while ((token = strtok(NULL, "/")))
        {
            //Only update the parent if it is allocated
            if (next_inode_ref != UNALLOCATED_INODE)
            {
                *parent = next_inode_ref;
                next_inode_ref = get_next_inode(*parent, token, &block_ref, dir_entry);
            }
            //Otherwise count how many tokens short we were
            else
            {
                missing_tokens++;
            }

            //If there was more than one missing token, the path does not yet exist
            //Return a positive value to indicate this error
            if (missing_tokens > 1)
            {
                return (1);
            }
        }
    }
    //Otherwise, the root was asked for
    else
    {
        next_inode_ref = *parent;
        block_ref = ROOT_DIRECTORY_BLOCK;
    }

    //Assign the appropriate child block reference if its inode is allocated
    if (next_inode_ref != UNALLOCATED_INODE)
    {
        //Set the child to the last returned block reference
        *child = block_ref;
    }
    //Otherwise, this file does not exist yet
    else
    {
        return (-1);
    }

    //Return success
    return (0);
}

/**
 * Makes a directory at a given location. Does not do so recursively
 * 
 * @param *cwd: current working directory
 * @param *path: path to create a directory at
 * @return: 0 on success
 */
int oufs_mkdir(char *cwd, char *path)
{
    /* Declare local variables */
    BLOCK block;               //block for writing new stuff to
    BLOCK_REFERENCE block_ref; //reference to the block for writing
    INODE_REFERENCE child;     //inode reference to the new dir
    int i;                     //iteration variable
    char *name;                //name of the file to create
    INODE_REFERENCE parent;    //parent of the made directory
    INODE parent_inode;        //parent inode of new dir
    int result;                //result of different operations

    //See if the directory and/or its path exists
    result = oufs_find_file(cwd, path, &parent, &block_ref, NULL);

    //Find the parent of the directory to create
    if (result > 0)
    {
        fprintf(stderr, "Error: path to directory does not exist\n");
        return (-1);
    }
    else if (!result)
    {
        fprintf(stderr, "Error: directory already exists!\n");
        return (-1);
    }

    //Find out if space in the parent inode exists
    result = oufs_find_space_available(parent);

    //If there is space available, make the new directory
    if (!result)
    {
        //Allocate a new inode
        child = oufs_allocate_new_inode(parent, IT_DIRECTORY);

        //Get the target inode
        if (!oufs_read_inode_by_reference(parent, &parent_inode))
        {
            //Add an entry to the directory
            vdisk_read_block(parent_inode.data[0], &block);

            //Find the first available directory entry
            for (i = 0; i < DIRECTORY_ENTRIES_PER_BLOCK; ++i)
            {
                //We look for the first entry with an unallocated inode reference
                if (block.directory.entry[i].inode_reference == UNALLOCATED_INODE)
                {
                    //End the loop and assign entry
                    break;
                }
            }

            //Add an entry to the parent directory that points to the child inode
            //Truncate file names longer than the maximum length
            name = basename(path);
            strncpy(block.directory.entry[i].name, name, FILE_NAME_SIZE - 1);
            block.directory.entry[i].name[FILE_NAME_SIZE - 1] = '\0';
            block.directory.entry[i].inode_reference = child;
            parent_inode.size++;

            //Write the changes
            vdisk_write_block(parent_inode.data[0], &block);
            oufs_write_inode_by_reference(parent, &parent_inode);
        }
        else
        {
            fprintf(stderr, "Could not open parent directory\n");
        }
    }
    //Print an error saying there is no space left in this parent directory
    else
    {
        fprintf(stderr, "Unable to make directory at %s: out of space\n", path);
    }

    //Indicate success
    return (0);
}

/**
 * Lists all the files and directories located at a path
 * 
 * @param *cwd: current working directory
 * @param *path: path to list
 * @return: 0 on success
 */
int oufs_list(char *cwd, char *path)
{
    /* Declare local variables */
    BLOCK block;               //block to list
    BLOCK_REFERENCE block_ref; //reference to block to list
    int i;                     //iteration variable
    INODE inode;               //inode to list
    char *name;                //name of the file to list if applicable
    INODE_REFERENCE parent;    //parent of the list directory

    //Initialize variables
    name = NULL;

    //Walk through the file tree to find the file or directory
    if (oufs_find_file(cwd, path, &parent, &block_ref, NULL) < 0)
    {
        //If no file was found, print an error
        fprintf(stderr, "Unable to find file/directory at path: %s\n", path);
        return (-1);
    }

    //If name is NULL, we have a directory. Print its contents
    if (name == NULL)
    {
        //Read the block that was found by the search
        vdisk_read_block(block_ref, &block);

        //Sort the directory entries by name
        qsort(block.directory.entry, DIRECTORY_ENTRIES_PER_BLOCK, sizeof(DIRECTORY_ENTRY), oufs_dir_entry_comp);

        //Go through all the directory entries
        for (i = 0; i < DIRECTORY_ENTRIES_PER_BLOCK; ++i)
        {
            //Only print entries that exist
            if (block.directory.entry[i].inode_reference != UNALLOCATED_INODE)
            {
                //Determine file type
                oufs_read_inode_by_reference(block.directory.entry[i].inode_reference, &inode);

                //Add a slash to the entry if it is a directory
                if (inode.type == IT_DIRECTORY)
                {
                    printf("%s/\n", block.directory.entry[i].name);
                }
                //Otherwise just print normally
                else
                {
                    printf("%s\n", block.directory.entry[i].name);
                }
            }
        }
    }
    //Otherwise a file was found. Print its name
    else if (block_ref != UNALLOCATED_BLOCK)
    {
        printf("%s\n", name);
    }
    //Otherwise no matching files were found

    return (0);
}

/**
 * Removes an empty directory from the file system
 * 
 * @param *cwd: current working directory in the file system
 * @param *path: path to a directory to remove
 * @return: 0 if successful, non-zero if unsuccessful
 */
int oufs_rmdir(char *cwd, char *path)
{
    /* Declare local variables */
    BLOCK block;               //block for reading and writing
    BLOCK_REFERENCE block_ref; //block reference for block to read/write
    DIRECTORY_ENTRY dir_entry; //directory entry for
    int i;                     //iteration variable
    INODE inode;               //i-node for removal operations
    INODE_REFERENCE parent;    //reference to parent inode of file being removed
    INODE parent_inode;        //parent inode of removed directory

    //If we are deleting something that can't be deleted, throw an error
    if (!strcmp(path, "..") || !strcmp(path, "/") || !strcmp(path, "."))
    {
        fprintf(stderr, "Error: cannot remove %s, protected directory type.\n", path);
        return (-1);
    }

    //Walk through the file tree to find the file or directory
    if (oufs_find_file(cwd, path, &parent, &block_ref, &dir_entry))
    {
        //If no file was found, print an error
        fprintf(stderr, "Unable to find file/directory at path: %s\n", path);
        return (-1);
    }

    //Get the inode of the directory entry
    oufs_read_inode_by_reference(dir_entry.inode_reference, &inode);

    //Get the parent inode
    oufs_read_inode_by_reference(parent, &parent_inode);

    //Read in the block with the entry to remove
    vdisk_read_block(parent_inode.data[0], &block);

    //Only remove this if it is an empty directory
    if (inode.size <= 2 && inode.type == IT_DIRECTORY)
    {
        //Find and remove the directory entry
        for (i = 0; i < DIRECTORY_ENTRIES_PER_BLOCK; ++i)
        {
            //If the directory entry name matches, clean this entry and exit the search
            if (!strcmp(block.directory.entry[i].name, dir_entry.name))
            {
                //Clear the current entry
                oufs_clean_directory_entry(&block.directory.entry[i]);
                break;
            }
        }

        //Write the changes to disk
        vdisk_write_block(parent_inode.data[0], &block);

        //Read the block with the deleted directory contents
        vdisk_read_block(block_ref, &block);

        //Reset the removed directory itself
        for (i = 0; i < DIRECTORY_ENTRIES_PER_BLOCK; ++i)
        {
            oufs_clean_directory_entry(&block.directory.entry[i]);
        }

        //Write the changes to disk
        vdisk_write_block(block_ref, &block);

        //Deallocate the inode and directory block
        oufs_deallocate_block(block_ref);
        oufs_deallocate_inode(dir_entry.inode_reference);

        //Clean the inode for future use
        oufs_clean_inode(dir_entry.inode_reference);

        //Reduce the size of the parent inode and write these changes to the disk
        oufs_read_inode_by_reference(parent, &parent_inode);
        parent_inode.size--;
        oufs_write_inode_by_reference(parent, &parent_inode);
    }
    //Print an error if the directory is not empty
    else
    {
        fprintf(stderr, "Error: directory not empty\n");
        return (-1);
    }

    //Return success
    return (0);
}

/**
 * Configure a directory entry so that it has no name and no inode
 *
 * @param entry The directory entry to be cleaned
 */
void oufs_clean_directory_entry(DIRECTORY_ENTRY *entry)
{
    entry->name[0] = 0; // No name
    entry->inode_reference = UNALLOCATED_INODE;
}

/**
 * Initialize a directory block as an empty directory
 *
 * @param self Inode reference index for this directory
 * @param self Inode reference index for the parent directory
 * @param block The block containing the directory contents
 *
 */

void oufs_clean_directory_block(INODE_REFERENCE self, INODE_REFERENCE parent, BLOCK *block)
{
    // Debugging output
    if (debug)
        fprintf(stderr, "New clean directory: self=%d, parent=%d\n", self, parent);

    // Create an empty directory entry
    DIRECTORY_ENTRY entry;
    oufs_clean_directory_entry(&entry);

    // Copy empty directory entries across the entire directory list
    for (int i = 0; i < DIRECTORY_ENTRIES_PER_BLOCK; ++i)
    {
        block->directory.entry[i] = entry;
    }

    // Now we will set up the two fixed directory entries

    // Self
    strncpy(entry.name, ".", 2);
    entry.inode_reference = self;
    block->directory.entry[0] = entry;

    // Parent (same as self)
    strncpy(entry.name, "..", 3);
    entry.inode_reference = parent;
    block->directory.entry[1] = entry;
}

/**
 * Cleans an inode by reference and writes it to disk
 * 
 * @param inode_ref: reference to the inode to clean
 */
void oufs_clean_inode(INODE_REFERENCE inode_ref)
{
    /* Declare local variables */
    INODE inode; //inode that needs cleaning

    //Clean the inode
    inode.type = IT_NONE;
    inode.n_references = 0;
    memset(inode.data, UNALLOCATED_BLOCK, BLOCKS_PER_INODE * sizeof(BLOCK_REFERENCE));
    inode.size = 0;

    //Write changes to disk
    oufs_write_inode_by_reference(inode_ref, &inode);
}

/**
 * Allocate a new data block
 *
 * If one is found, then the corresponding bit in the block allocation table is set
 *
 * @return The index of the allocated data block.  If no blocks are available,
 * then UNALLOCATED_BLOCK is returned
 *
 */
BLOCK_REFERENCE oufs_allocate_new_block()
{
    BLOCK block;

    // Read the master block
    vdisk_read_block(MASTER_BLOCK_REFERENCE, &block);

    // Scan for an available block
    int block_byte;
    int flag;

    // Loop over each byte in the allocation table.
    for (block_byte = 0, flag = 1; flag && block_byte < N_BLOCKS_IN_DISK / 8; ++block_byte)
    {
        if (block.master.block_allocated_flag[block_byte] != 0xff)
        {
            // Found a byte that has an opening: stop scanning
            flag = 0;
            break;
        }
    }

    // Did we find a candidate byte in the table?
    if (flag == 1)
    {
        // No
        if (debug)
            fprintf(stderr, "No blocks\n");
        return (UNALLOCATED_BLOCK);
    }

    // Found an available data block

    // Set the block allocated bit
    // Find the FIRST bit in the byte that is 0 (we scan in bit order: 0 ... 7)
    int block_bit = oufs_find_open_bit(block.master.block_allocated_flag[block_byte]);

    // Now set the bit in the allocation table
    block.master.block_allocated_flag[block_byte] |= (1 << block_bit);

    // Write out the updated master block
    vdisk_write_block(MASTER_BLOCK_REFERENCE, &block);

    if (debug)
        fprintf(stderr, "Allocating block=%d (%d)\n", block_byte, block_bit);

    // Compute the block index
    BLOCK_REFERENCE block_reference = (block_byte << 3) + block_bit;

    if (debug)
        fprintf(stderr, "Allocating block=%d\n", block_reference);

    // Done
    return (block_reference);
}

/**
 * Allocate new inode based on the parent inode reference
 * 
 * @param parent: the allocated inode's parent
 * @return: an inode reference to the new child inode
 */
INODE_REFERENCE oufs_allocate_new_inode(INODE_REFERENCE parent, char type)
{
    /* Declare local variables */
    BLOCK block;               //block for reading and writing to master
    BLOCK_REFERENCE block_ref; //reference to a block
    INODE inode;               //inode to initialize
    int inode_bit;             //bit in allocation table for inode
    int inode_byte;            //byte in allocaion table for inode
    INODE_REFERENCE inode_ref; //reference to inode

    //Read the master block to analyze the allocation table
    vdisk_read_block(MASTER_BLOCK_REFERENCE, &block);

    //Look for the first open inode byte
    for (inode_byte = 0; inode_byte < N_INODE_BLOCKS; ++inode_byte)
    {
        //If a byte isn't full, we found an open byte
        if (block.master.inode_allocated_flag[inode_byte] != 0xff)
        {
            break;
        }
        //If this is the last iteration and no block was found, return a fail status
        else if (inode_byte == (N_INODE_BLOCKS - 1))
        {
            fprintf(stderr, "No more I-Nodes to allocate!\n");
            return (UNALLOCATED_INODE);
        }
    }

    //Find the first open bit in the open byte and calculate the inode reference
    inode_bit = oufs_find_open_bit(block.master.inode_allocated_flag[inode_byte]);
    inode_ref = (inode_byte << 3) + inode_bit;

    //Allocate the inode in the allocation table
    block.master.inode_allocated_flag[inode_byte] |= (1 << inode_bit);
    vdisk_write_block(MASTER_BLOCK_REFERENCE, &block);

    //Create a new inode
    oufs_read_inode_by_reference(inode_ref, &inode);
    inode.type = type;
    inode.n_references = 1;
    memset(inode.data, UNALLOCATED_BLOCK, BLOCKS_PER_INODE * sizeof(BLOCK_REFERENCE));
    inode.size = 0;

    //If the new inode represents a directory, allocate a directory block
    if(type == IT_DIRECTORY)
    {
        //Initialize a directory block and an empty directory
        memset(block.directory.entry, UNALLOCATED_BLOCK, DIRECTORY_ENTRIES_PER_BLOCK * sizeof(BLOCK_REFERENCE));
        oufs_clean_directory_block(inode_ref, parent, &block);

        //Allocate a new block and add the directory block to the filesystem
        block_ref = oufs_allocate_new_block();
        vdisk_write_block(block_ref, &block);

        //Add the new directory block to the inode        
        inode.data[0] = block_ref;
        inode.size = 2;
    }

    //Add the inode to the filesystem
    oufs_write_inode_by_reference(inode_ref, &inode);

    return (inode_ref);
}

/**
 * Deallocate a block bassed on its reference
 * 
 * @param block_ref: reference to a block to deallocate
 */
void oufs_deallocate_block(BLOCK_REFERENCE block_ref)
{
    /* Declare local variables */
    BLOCK block;    //block for reading/writing to master block
    int block_bit;  //bit in allocation table for block
    int block_byte; //byte in allocation table for block

    //Find location in the allocation table
    block_byte = block_ref / 8;
    block_bit = block_ref % 8;

    //Read the current block and reset the allocation bit
    vdisk_read_block(MASTER_BLOCK_REFERENCE, &block);
    block.master.block_allocated_flag[block_byte] &= ~(1 << block_bit);

    //Write the changes to the disk
    vdisk_write_block(MASTER_BLOCK_REFERENCE, &block);
}

/**
 * Deallocates an inode by its inode reference
 * 
 * @param inode_ref: reference to inode to deallocate
 */
void oufs_deallocate_inode(INODE_REFERENCE inode_ref)
{
    /* Declare local variables */
    BLOCK block;    //block for reading/writing master block
    int inode_bit;  //bit for the inode to deallocate
    int inode_byte; //byte for the inode to deallocate

    //Find location in the allocation table
    inode_byte = inode_ref / N_INODE_BLOCKS;
    inode_bit = inode_ref % N_INODE_BLOCKS;

    //Read the current block and reset the allocation bit
    vdisk_read_block(MASTER_BLOCK_REFERENCE, &block);
    block.master.inode_allocated_flag[inode_byte] &= ~(1 << inode_bit);

    //Write the changes to the disk
    vdisk_write_block(MASTER_BLOCK_REFERENCE, &block);
}

/**
 * Compares two directory entries by name. Used for qsort and file sorting
 * 
 * @param *a: a directory entry
 * @param *b: another directory entry
 * @return: how a compares to b in the form of an integer
 */
int oufs_dir_entry_comp(const void *a, const void *b)
{
    /* Declare local variables */
    DIRECTORY_ENTRY a_entry; //one of the directory entries
    DIRECTORY_ENTRY b_entry; //the other directory entry

    //Convert void pointers to directory entries
    a_entry = *(DIRECTORY_ENTRY *)a;
    b_entry = *(DIRECTORY_ENTRY *)b;

    //Return the ascii difference in the name strings
    return strcmp(a_entry.name, b_entry.name);
}

/**
 * Finds out if any space is available in a directory for writing.
 * 
 * @param parent: reference to the inode that we need to check for space availability
 * @return: 0 if there is space available, non-zero if not
 */
int oufs_find_space_available(INODE_REFERENCE parent)
{
    /* Declare local variables */
    INODE inode; //inode to find the parent reference

    //Read in the inode
    oufs_read_inode_by_reference(parent, &inode);

    //If the inode size can't support more files, indicate the inode is full
    if (inode.size == DIRECTORY_ENTRIES_PER_BLOCK)
    {
        return (-1);
    }

    //Return success
    return (0);
}

/*
 * Finds an open bit in a byte in the allocation table
 * 
 * @param value: byte in allocation table to explore
 * @return: the first available bit in this byte (least significant being first)
 */
int oufs_find_open_bit(unsigned char value)
{
    /* Declare local variables */
    int bit; //first available bit

    //Initialize local variables
    bit = 0;

    //Loop through the bits in value and find the first one that has a 0
    for (; bit < 8; ++bit)
    {
        //If the current shifted bit is 0, break our and return the bit
        if (!((value >> bit) & 1))
        {
            break;
        }
    }

    //Return the first open bit
    return bit;
}

/**
 * Get the next inode in a file search
 * 
 * @param inode_ref: reference to current inode
 * @param *file: the file that needs to be found in this inode
 * @param *block_ref: return value for the child's block reference
 * @param *dir_entry: directory entry for the found file
 * @return: the next inode reference in the sequence
 */
INODE_REFERENCE get_next_inode(INODE_REFERENCE inode_ref, char *file, BLOCK_REFERENCE *block_ref, DIRECTORY_ENTRY *dir_entry)
{
    /* Declare local variables */
    BLOCK block; //block for reading and writing
    int i;       //iteration variable
    INODE inode; //inode for the current inode reference
    int j;       //iteration variable

    //Initialize return values
    *block_ref = UNALLOCATED_BLOCK;

    //Get the inode data
    if (oufs_read_inode_by_reference(inode_ref, &inode))
    {
        //if there is an error, return unallocated inode
        return UNALLOCATED_INODE;
    }

    //Go through each entry in the inode until the file is found
    for (i = 0; i < BLOCKS_PER_INODE; ++i)
    {
        //Only consider allocated blocks
        if (inode.data[i] != UNALLOCATED_BLOCK)
        {
            //Read the directory block
            vdisk_read_block(inode.data[i], &block);

            //Loop through all directory entries to find the file
            for (j = 0; j < DIRECTORY_ENTRIES_PER_BLOCK; ++j)
            {
                //If the file is found, set the file's block reference
                //and return the inode reference
                if (!strcmp(file, block.directory.entry[j].name))
                {
                    oufs_read_inode_by_reference(block.directory.entry[j].inode_reference, &inode);
                    *block_ref = inode.data[0];

                    //Set the directory entry reference only if it exists
                    if (dir_entry)
                    {
                        *dir_entry = block.directory.entry[j];
                    }

                    return (block.directory.entry[j].inode_reference);
                }
            }
        }
    }

    //The file could not be found, so return the unallocated inode
    return (UNALLOCATED_INODE);
}

/**
 * 
 */
OUFILE *oufs_fopen(char *cwd, char *path, char *mode)
{
    /* Declare local variables */
    BLOCK_REFERENCE child_block_ref;    //reference to the child block (if exists)
    DIRECTORY_ENTRY child_dir_entry;    //directory entry of the touched file (if exists)
    INODE           child_inode;        //inode of the file to touch
    INODE_REFERENCE child_ref;          //child inode reference
    BLOCK           dir_block;          //directory block of file to touch
    OUFILE         *file;               //file to open
    int             i;                  //iteration variable
    char           *name;               //name of the new file
    INODE           parent_inode;       //inode of the parent directory
    INODE_REFERENCE parent_ref;         //reference to the parent inode of the touched file
    int             result;             //result of oufs operations

    //Attempt to find the file in the filesystem
    result = oufs_find_file(cwd, path, &parent_ref, &child_block_ref, &child_dir_entry);

    //If the parent path does not exist, this is an error
    if (result > 0)
    {
        fprintf(stderr, "Error: path to file does not exist\n");
        return (NULL);
    }
    //If the file already exists, see if it is a directory or not
    else if (!result)
    {
        //Get the child's inode data
        oufs_read_inode_by_reference(child_dir_entry.inode_reference, &child_inode);

        //If the inode is a directory, this is an error
        if (child_inode.type == IT_DIRECTORY)
        {
            fprintf(stderr, "Error: file provided is actually a directory.\n");
        }

        //Allocate memory for the file
        file = (OUFILE*)malloc(sizeof(OUFILE));

        //Since the file is found, gather its information 
        file->inode_reference = child_dir_entry.inode_reference;
        file->mode = mode[0];

        //Based on the file's mode, set the offset
        //If the file is being appended to, put the offset at the end
        if(mode[0] == 'a')
        {
            file->offset = child_inode.size;
        }
        //Otherwise put the offset at the start and deallocate all data blocks
        else
        {
            //Reset offset
            file->offset = 0;

            //Deallocate data blocks
            for(i = 0; i < BLOCKS_PER_INODE; ++i)
            {
                oufs_deallocate_block(child_inode.data[i]);
            }

            //Reset this inode's data
            child_inode.size = 0;
            oufs_write_inode_by_reference(file->inode_reference, &child_inode);
        }

        //Return the file that has been found
        return (file);
    }
    //If the file is not found but it needs to be read, this is an error
    else if (!strcmp(mode, "r"))
    {
        fprintf(stderr, "Error: file to read does not exist\n");
        return (NULL);
    }
    //If this case is reached, the child does not exist and can be created
    //Note, this only happens when a write operation of some sort is to be performed
    else
    {
        //Read in the parent inode for memory checking
        oufs_read_inode_by_reference(parent_ref, &parent_inode);

        //if the parent is able to hold an additional file, add this one
        if (parent_inode.size < DIRECTORY_ENTRIES_PER_BLOCK)
        {
            //Open a directory block
            vdisk_read_block(parent_inode.data[0], &dir_block);

            //Look for the open space in this inode
            for (i = 0; i < DIRECTORY_ENTRIES_PER_BLOCK; ++i)
            {
                //Once the first available directory entry is found, break out and add the file
                if (dir_block.directory.entry[i].inode_reference == UNALLOCATED_INODE)
                {
                    break;
                }
            }

            //Allocate an inode for the file
            child_ref = oufs_allocate_new_inode(parent_ref, IT_FILE);

            //Add an entry to the parent directory that points to the child inode
            //Truncate any file name longer than the maximum length
            name = basename(path);
            strncpy(dir_block.directory.entry[i].name, name, FILE_NAME_SIZE - 1);
            dir_block.directory.entry[i].name[FILE_NAME_SIZE - 1] = '\0';
            dir_block.directory.entry[i].inode_reference = child_ref;
            parent_inode.size++;

            //Write the changes
            vdisk_write_block(parent_inode.data[0], &dir_block);
            oufs_write_inode_by_reference(parent_ref, &parent_inode);

            //Allocate memory for the new file
            file = (OUFILE*)malloc(sizeof(OUFILE));

            //Create the file representation
            file->inode_reference = child_ref;
            file->offset = 0;
            file->mode = mode[0];

            //Return the file's information
            return (file);
        }
        //Otherwise print an error saying the path is full
        else
        {
            fprintf(stderr, "Memory error: path fully allocated.\n");
            return (NULL);
        }
    }
}

/**
 * 
 */
void oufs_fclose(OUFILE *fp)
{
    //Free the file pointer from memory and set it to NULL
    free(fp);
    fp = NULL;
}

/**
 * 
 */
int oufs_fwrite(OUFILE *fp, unsigned char *buf, int len)
{
    /* Declare local variables */
    int             block_index;    //index of block in inode data table
    int             block_offset;   //offset inside of block to write to
    BLOCK           data_block;     //block for write operation
    BLOCK_REFERENCE data_block_ref; //reference to the data block
    INODE           file_inode;     //inode of the file
    int             i;              //iteration variable

    //Only write to files that were opened for writing or appending
    if(fp->mode == 'r')
    {
        fprintf(stderr, "Error, this file is not open for write operation\n");
        return (-2);
    }

    //Open the file's inode
    if(oufs_read_inode_by_reference(fp->inode_reference, &file_inode))
    {
        fprintf(stderr, "Error, unable to open file for writing\n");
        return (-1);
    }

    //Insert the characters
    for(i = 0; i < len; ++i)
    {
        //Find the block to write to
        block_index = fp->offset / BLOCK_SIZE;
        data_block_ref = file_inode.data[block_index];

        //Find the location in the block for writing
        block_offset = fp->offset % BLOCK_SIZE;

        //Allocate a new block if needed
        if(data_block_ref == UNALLOCATED_BLOCK)
        {
            data_block_ref = oufs_allocate_new_block();
            file_inode.data[block_index] = data_block_ref;
            oufs_write_inode_by_reference(fp->inode_reference, &file_inode);
        }

        //Open the block (if needed)
        if(i == 0 || block_offset == 0)
        {
            vdisk_read_block(data_block_ref, &data_block);
        }

        //Add the character to the block
        data_block.data.data[block_offset] = buf[i];

        //Increment the size of the inode
        file_inode.size++;
        oufs_write_inode_by_reference(fp->inode_reference, &file_inode);

        //Write the block if needed
        if(i == (len -1) || block_offset == (BLOCK_SIZE - 1))
        {
            vdisk_write_block(data_block_ref, &data_block);
        }

        //Increment the file pointer offset
        fp->offset++;
    }
    
    return 0;
}