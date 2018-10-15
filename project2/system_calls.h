#ifndef SYSTEM_CALLS_H
#define SYSTEM_CALLS_H

/* Libraries needed */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

/* Custom headers */
#include "global_constants.h"

/* Functions */
void fork_exec(char *command, char *arg_list[]);

#endif