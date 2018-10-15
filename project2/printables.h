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

/* External variables */
extern char **environ;              // environment array

/* Functions */
void ditto(char *input);
void help();
void printEnv();
void printPrompt();

#endif