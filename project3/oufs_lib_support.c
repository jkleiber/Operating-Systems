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
 * 
 */
int oufs_format_disk(char *virtual_disk_name)
{
    /* Declare local variables */
    BLOCK           block;
    BLOCK_REFERENCE block_ref;

    //Set everything in the block and inode to 0
    memset(block.data.data, 0, BLOCK_SIZE);

    //Loop through all bytes to set the bytes to 0.
    for(block_ref = MASTER_BLOCK_REFERENCE; block_ref < N_BLOCKS_IN_DISK; ++block_ref)
    {
        if(vdisk_write_block(block_ref, &block))
        {
            return -1;
        }
    }

    //Initialize the master block
    block.master.block_allocated_flag[MASTER_BLOCK_REFERENCE] = 0xff;
    //block.master.inode_allocated_flag[0] = 0x01;

    //Write the master block to the disk
    if(vdisk_write_block(MASTER_BLOCK_REFERENCE, &block))
    {
        return -1;
    }

    //Allocate block 8 as well to account for the master block allocating 0 - (N_INODE_BLOCKS - 1)
    oufs_allocate_new_block();

    //Initialize the first inode
    oufs_allocate_new_inode(0);

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
 * 
 */
int oufs_write_inode_by_reference(INODE_REFERENCE i, INODE *inode)
{
    /* Declare local variables */
    BLOCK           block;
    BLOCK_REFERENCE block_ref;
    int             element;

    // Find location in block
    block_ref = (i / INODES_PER_BLOCK) + 1;
    element = (i % INODES_PER_BLOCK);

    // Load the inode into the block
    block.inodes.inode[element] = *inode;

    // Attempt to write the inode to the block
    if(vdisk_write_block(block_ref, &block))
    {
        return -1;
    }

    return 0;
}

/**
 * 
 */
int oufs_find_file(char *cwd, char *path, INODE_REFERENCE *parent, BLOCK_REFERENCE *child)
{
    /* Declare local variables */
    char *abs_path;
    BLOCK_REFERENCE block_ref;
    INODE inode;
    INODE_REFERENCE next_inode_ref;
    char *token;

    //Initialize tree walking
    *child = MASTER_BLOCK_REFERENCE;
    *parent = 0;

    //Concatenate the name onto the cwd if the name exists
    if(path != NULL)
    {
        //If the input name is an absolute path use it instead
        if(path[0] == '/')
        {
            abs_path = strdup(path);
        }
        //otherwise add the path to the cwd
        else
        {
            abs_path = strdup(cwd);

            //Make sure the current directory has a slash at the end
            if(abs_path[strlen(abs_path) - 1] != '/')
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
    if(token)
    {
        //Get the next inode reference
        next_inode_ref = get_next_inode(*parent, token, &block_ref);

        //Find the associated inode for each token
        while((token = strtok(NULL, "/")))
        {
            //Only update the parent if it is allocated
            if(next_inode_ref != UNALLOCATED_INODE)
            {
                *parent = next_inode_ref;
            }
            //Otherwise break out of the loop
            else
            {
                break;
            }

            next_inode_ref = get_next_inode(*parent, token, &block_ref);
        }
    }
    //Otherwise, the root was asked for
    else
    {
        next_inode_ref = *parent;
        block_ref = ROOT_DIRECTORY_BLOCK;
    }

    //Read the inode of the found file
    if(next_inode_ref != UNALLOCATED_INODE)
    {
        //oufs_read_inode_by_reference(next_inode_ref, &inode);
        //Set the child to the last returned block reference
        *child = block_ref;
    }
 
    //Return success
    return (0);
}

/**
 * 
 */
int oufs_mkdir(char *cwd, char *path)
{

    INODE testnode;
    /* Declare local variables */
    BLOCK block;
    BLOCK_REFERENCE block_ref;
    INODE_REFERENCE child;
    DIRECTORY_ENTRY entry;
    char *name;
    INODE_REFERENCE parent;
    INODE parent_inode;
//Debugging!
oufs_read_inode_by_reference(0, &testnode);
printf("%d\n", testnode.size);
    //Find the parent of the directory to create
    if(oufs_find_file(cwd, path, &parent, &block_ref))
    {
        fprintf(stderr, "Error: path to directory does not exist\n");
        return (-1);
    }
//Debugging!
oufs_read_inode_by_reference(parent, &testnode);
printf("%d\n", testnode.size);
    //Allocate a new inode
    child = oufs_allocate_new_inode(parent);
//Debugging!
oufs_read_inode_by_reference(parent, &testnode);
printf("%d\n", testnode.size);
    //Get the target inode
    if(!oufs_read_inode_by_reference(parent, &parent_inode))
    {
        //Add an entry to the parent directory that points to the child inode
        entry.inode_reference = child;
        name = basename(path);
        strncpy(entry.name, name, FILE_NAME_SIZE);

        //Add an entry to the directory
        vdisk_read_block(parent_inode.data[0], &block);

        //Only add directories if they fit
        if(parent_inode.size < DIRECTORY_ENTRIES_PER_BLOCK)
        {
            block.directory.entry[parent_inode.size] = entry;
            parent_inode.size++;

            //Write the changes
            vdisk_write_block(parent_inode.data[0], &block);
            //Debugging!
oufs_read_inode_by_reference(parent, &testnode);
printf("%d\n", testnode.size);
            oufs_write_inode_by_reference(parent, &parent_inode);
            //Debugging!
oufs_read_inode_by_reference(parent, &testnode);
printf("%d\n", testnode.size);
        }
        //If there is no more space in this directory, throw an error
        else
        {
            fprintf(stderr, "Error: no more space left in this directory\n");
            return (-1);
        }
    }
    else
    {
        fprintf(stderr, "Could not open parent directory\n");
    }

    //Indicate success
    return (0);
}


/**
 * 
 */
int oufs_list(char *cwd, char *path)
{
    /* Declare local variables */
    BLOCK               block;
    BLOCK_REFERENCE     block_ref;
    int                 i;          //iteration variable
    char               *name;
    INODE_REFERENCE     parent;

    //Initialize variables
    name = NULL;

    //Walk through the file tree to find the file or directory
    if(oufs_find_file(cwd, path, &parent, &block_ref))
    {
        //If no file was found, print an error
        fprintf(stderr, "Unable to find file/directory at path: %s\n", path);
        return -1;
    }

    //If name is NULL, we have a directory. Print its contents
    if(name == NULL)
    {
        //Read the block that was found by the search
        vdisk_read_block(block_ref, &block);

        //Sort the directory entries by name
        qsort(block.directory.entry, DIRECTORY_ENTRIES_PER_BLOCK, sizeof(DIRECTORY_ENTRY), oufs_dir_entry_comp);

        //Go through all the directory entries
        for(i = 0; i < DIRECTORY_ENTRIES_PER_BLOCK; ++i)
        {
            //Only print entries that exist
            if(block.directory.entry[i].inode_reference != UNALLOCATED_INODE)
            {
                printf("%s\n", block.directory.entry[i].name);
            }
        }
    }
    //Otherwise a file was found. Print its name
    else if(block_ref != UNALLOCATED_BLOCK)
    {
        printf("%s\n", name);
    }
    //Otherwise no matching files were found 

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
 * 
 */
INODE_REFERENCE oufs_allocate_new_inode(INODE_REFERENCE parent)
{
    /* Declare local variables */
    BLOCK block;
    BLOCK_REFERENCE block_ref;
    INODE inode;
    int inode_bit;
    int inode_byte;
    INODE_REFERENCE inode_ref;

    //Read the master block to analyze the allocation table
    vdisk_read_block(MASTER_BLOCK_REFERENCE, &block);
    
    //Look for the first open inode byte
    for(inode_byte = 0; inode_byte < N_INODE_BLOCKS; ++inode_byte)
    {
        //If a byte isn't full, we found an open byte
        if(block.master.inode_allocated_flag[inode_byte] != 0xff)
        {
            break;
        }
        //If this is the last iteration and no block was found, return a fail status
        else if(inode_byte == (N_INODE_BLOCKS - 1))
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

    //Initialize a directory block and an empty directory
    memset(block.directory.entry, UNALLOCATED_BLOCK, DIRECTORY_ENTRIES_PER_BLOCK * sizeof(BLOCK_REFERENCE)); 
    oufs_clean_directory_block(inode_ref, parent, &block);

    //Allocate a new block and add the directory block to the filesystem
    block_ref = oufs_allocate_new_block();
    vdisk_write_block(block_ref, &block);

    //Create a new inode (default to directory type)
    oufs_read_inode_by_reference(inode_ref, &inode);
    inode.type = IT_DIRECTORY;
    inode.n_references = 1;
    memset(inode.data, UNALLOCATED_BLOCK, BLOCKS_PER_INODE * sizeof(BLOCK_REFERENCE));
    inode.size = 2;
    inode.data[0] = block_ref;

    //Add the inode to the filesystem
    oufs_write_inode_by_reference(inode_ref, &inode);

    return (inode_ref);
}

/**
 * 
 */
int oufs_dir_entry_comp(const void *a, const void *b)
{
    /* Declare local variables */
    DIRECTORY_ENTRY a_entry;
    DIRECTORY_ENTRY b_entry;

    //Convert void pointers to directory entries
    a_entry = *(DIRECTORY_ENTRY *)a;
    b_entry = *(DIRECTORY_ENTRY *)b;
    
    //Return the ascii difference in the name strings
    return strcmp(a_entry.name, b_entry.name);
}

/*
 *
 */
int oufs_find_open_bit(unsigned char value)
{
    /* Declare local variables */
    int bit;

    //Initialize local variables
    bit = 0;

    //Loop through the bits in value and find the first one that has a 0
    for (; bit < 8; ++bit)
    {
        if (!((value >> bit) & 1))
        {
            break;
        }
    }

    return bit;
}

INODE_REFERENCE get_next_inode(INODE_REFERENCE inode_ref, char *file, BLOCK_REFERENCE *block_ref)
{
    /* Declare local variables */
    BLOCK block;
    int i;
    INODE inode;
    int j;

    //Initialize return values
    *block_ref = UNALLOCATED_BLOCK;

    //Get the inode data
    if(oufs_read_inode_by_reference(inode_ref, &inode))
    {
        //if there is an error, return unallocated inode
        return UNALLOCATED_INODE;
    }

    //Go through each entry in the inode until the file is found
    for(i = 0; i < BLOCKS_PER_INODE; ++i)
    {
        //Only consider allocated blocks
        if(inode.data[i] != UNALLOCATED_BLOCK)
        {
            //Read the directory block
            vdisk_read_block(inode.data[i], &block);

            //Loop through all directory entries to find the file
            for(j = 0; j < DIRECTORY_ENTRIES_PER_BLOCK; ++j)
            {
                //If the file is found, set the block reference and return the inode reference
                if(!strcmp(file, block.directory.entry[j].name))
                {
                    *block_ref = inode.data[i];
                    return (block.directory.entry[j].inode_reference);
                }
            }
        }
    }

    //The file could not be found, so return the unallocated inode
    return (UNALLOCATED_INODE);
}

char* oufs_basename(char *path)
{
    char *base;

    base = strrchr(path, '/') + 1;

    return base;
}