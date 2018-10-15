#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

/* Libraries Required */
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>
#include <sys/stat.h>

/* Functions */
void generalErrorHandler(char *arg);

#endif