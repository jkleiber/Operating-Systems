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
int fork_exec(char *command, char *arg_list[]);
int general_command(char *command, char **flags, int num_flags, char **args, int num_tokens);

#endif