#ifndef PRINTABLES_H
#define PRINTABLES_H

/* Libraries needed */
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* Custom headers */
#include "error_handler.h"
#include "global_constants.h"
#include "system_calls.h"

/* External variables */
extern char **environ;              // environment array

/* Functions */
void ditto(char **args, int num_tokens);
void help();
void printCWD();
void printEnv();
void printPrompt();
void wipe();

#endif