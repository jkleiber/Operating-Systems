#ifndef FILE_OPERATIONS_H
#define FILE_OPERATIONS_H

/* Libraries needed */
#include <dirent.h>
#include <errno.h>
#include <ftw.h>
#include <libgen.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

/* Custom headers */
#include "error_handler.h"
#include "global_constants.h"
#include "printables.h"
#include "system_calls.h"

/* Functions */
void change_dir(char *dst);
int copy_file(char *src, char *dst);
void dfs_copy(char *src, char *dst, char *mode);
void filez(char **args, int num_tokens);
int get_type(const char *path);
void mkdirz(char **args, int num_tokens);
void morph_mimic(char *src, char *dst, char* mode, int recursive);
void rmdirz(char **args, int num_tokens);

#endif