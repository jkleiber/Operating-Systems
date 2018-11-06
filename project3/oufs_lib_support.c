#include <stdlib.h>
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
    INODE           inode;

    //Set everything in the block and inode to 0
    memset(block.data.data, 0, BLOCK_SIZE);

    //Loop through all bytes to set the bytes to 0.
    for(block_ref = 0; block_ref < N_BLOCKS_IN_DISK; ++block_ref)
    {
        if(vdisk_write_block(block_ref, block.data.data))
        {
            return -1;
        }
    }

    //Initialize the master block
    block.master.block_allocated_flag[MASTER_BLOCK_REFERENCE] = 0xff;
    block.master.inode_allocated_flag[0] = 0x01;

    //Write the master block to the disk
    if(vdisk_write_block(MASTER_BLOCK_REFERENCE, block.master.inode_allocated_flag))
    {
        return -1;
    }

    //Create the first inode
    inode.type = IT_DIRECTORY;
    inode.n_references = 1;
    memset(inode.data, UNALLOCATED_BLOCK, BLOCKS_PER_INODE * sizeof(BLOCK_REFERENCE));
    inode.size = 2;

    //Add the inode to the filesystem
    oufs_write_inode_by_reference(0, &inode);

    //Initialize a directory block
    memset(block.directory.entry, UNALLOCATED_BLOCK, DIRECTORY_ENTRIES_PER_BLOCK * sizeof(BLOCK_REFERENCE));

    //Initialize an empty directory
    oufs_clean_directory_block(0, 0, &block);

    //Add the directory block to the file system
    vdisk_write_block(ROOT_DIRECTORY_BLOCK, block.directory.entry);

    //Update the block allocation table
    vdisk_read_block(MASTER_BLOCK_REFERENCE, &block.master.inode_allocated_flag);
    block.master.block_allocated_flag[MASTER_BLOCK_REFERENCE + 1] = 0x03;
    vdisk_write_block(MASTER_BLOCK_REFERENCE, block.master.inode_allocated_flag);
    
    //Add the directory block to the first inode
    oufs_read_inode_by_reference(0, &inode);
    inode.data[0] = ROOT_DIRECTORY_BLOCK;
    oufs_write_inode_by_reference(0, &inode);

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
    if(vdisk_write_block(block_ref, block.inodes.inode))
    {
        return -1;
    }

    return 0;
}

/**
 * 
 */
int oufs_find_file(char *cwd, char *path, INODE_REFERENCE *parent, BLOCK_REFERENCE *child, char **local_name)
{
    /* Declare local variables */
    char *abs_path;
    BLOCK_REFERENCE block_ref;
    INODE inode;
    INODE_REFERENCE next_inode_ref;
    char *token;

    //Initialize tree walking
    *child = MASTER_BLOCK_REFERENCE;
    *local_name = NULL;
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
                abs_path = (char *)realloc(abs_path, (strlen(abs_path) + strlen(path) + 1) * sizeof(char));
                strncat(abs_path, "/", 1);
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
            if(next_inode_ref != UNALLOCATED_INODE)
            {
                *parent = next_inode_ref;
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
        oufs_read_inode_by_reference(next_inode_ref, &inode);
    }
    //If the file was not found, give up!
    else
    {
        return (-1);
    }
  
    //IF the inode represents a file, set its local name
    if(inode.type == IT_FILE)
    {
        *local_name = strdup(token);
    }

    //Set the child to the last returned block reference
    *child = block_ref;

    //Return success
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

    //Walk through the file tree to find the file or directory
    if(oufs_find_file(cwd, path, &parent, &block_ref, &name))
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
            //Only print entries that exist and are not the . or .. directory
            if(block.directory.entry[i].inode_reference != UNALLOCATED_INODE
            && strcmp(block.directory.entry[i].name, ".")
            && strcmp(block.directory.entry[i].name, ".."))
            {
                printf("%s\n", block.directory.entry[i].name);
            }
        }
    }
    //Otherwise a file was found. Print its name
    else
    {
        printf("%s\n", name);
    }

    return 0;
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

    // Parent (same as self
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
