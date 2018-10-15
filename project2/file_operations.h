#ifndef FILE_OPERATIONS_H
#define FILE_OPERATIONS_H

/* Libraries needed */
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>
#include <sys/stat.h>

/* Custom headers */
#include "error_handler.h"
#include "global_constants.h"
#include "system_calls.h"

/* Functions */
void change_dir(char *dst);
void filez(char *arg, char *input);
int get_type(const char *path);
void mkdirz(char *path);
void morph_mimic(char *src, char *dst, int mode, int recursive);
void rmdirz(char *path);

#endif