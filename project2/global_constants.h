#ifndef GLOBAL_CONSTS_H
#define GLOBAL_CONSTS_H

/* Macros for file types*/
#define OTHER        0
#define REGULAR_FILE 1
#define DIRECTORY    2

/* Macros for morph and mimic */
#define MIMIC_OP 0
#define MORPH_OP 1

/* Logic */
#define FALSE 0
#define TRUE  1

/* README.txt path */
#define HELP_PATH "/projects/2/README.txt"

/* Command line parsing */
#define MAX_ARGS 	64         // max # args
#define MAX_BUFFER 	1024       // max line buffer
#define SEPARATORS 	" \t\n"    // token sparators

/* File redirection codes */
#define READ            0
#define WRITE           1
#define APPEND          2
#define PIPE            3

/* File redirection flags*/
#define READ_FLAG       0
#define WRITE_FLAG      1
#define APPEND_FLAG     4
#define PIPE_FLAG       8

/* Maximum number of file redirects */
#define NUM_REDIRECTS   2

/* File redirection struct */
typedef struct file_info
{
    FILE* file;
    int type;
} file_info;

#endif