#include "error_handler.h"

/*
 * generalErrorHandler - custom error handler when the special commands run
 * into system errors. Converts errno to a nicer message and prints that to 
 * stderr as well
 * 
 * @param *arg: the command that generated the error
 */
void generalErrorHandler(char *arg)
{
    //Wipe error
    if(!strcmp(arg, "wipe"))
    {
        fprintf(stderr, "wipe failed: %s\n", strerror(errno));
    }
    //Filez error possibilities
    else if(!strcmp(arg, "filez"))
    {
        switch(errno)
        {
            case 0:
                fprintf(stderr, "filez unsuccessful on one or more files.\n");
                break;
            default:
                fprintf(stderr, "filez failed: %s\n", strerror(errno));
                break;
        }
    }
    //Ditto error
    else if(!strcmp(arg, "ditto"))
    {
        switch(errno)
        {
            case 0:
                fprintf(stderr, "ditto failed due to an incorrect input format or some general error.\n");
                break;
            default:
                fprintf(stderr, "ditto unsuccessful, system error: %s\n", strerror(errno));
                break;
        }
    }
    //Erase error
    else if(!strcmp(arg, "erase"))
    {
        fprintf(stderr, "unable to erase, error: %s\n", strerror(errno));
    }
    //Morph error
    else if(!strcmp(arg, "morph"))
    {
        fprintf(stderr, "morph not successful: %s\n", strerror(errno));
    }
    //Print general error if we don't know what command this is
    else
    {
        fprintf(stderr, "%s\n", strerror(errno));
    }
}