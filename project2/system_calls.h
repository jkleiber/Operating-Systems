#ifndef SYSTEM_CALLS_H
#define SYSTEM_CALLS_H

/* Libraries needed */
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

/* Custom headers */
#include "global_constants.h"

/* Functions */
int file_redirection(char **args, int num_tokens, int mode, file_info *files);
int fork_exec(char * command, char * arg_list[], file_info *files);
int general_command(char *command, char **flags, int num_flags, char **args, int num_tokens, file_info *files);

#endif