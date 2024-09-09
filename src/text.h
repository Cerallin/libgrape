#ifndef ARGS_H
#define ARGS_H

#include <argp.h>
#include <stdbool.h>

/* Used by main to communicate with parse_opt. */
typedef struct _arguments {
    bool verbose;
    char **files;
    char *output_file;
    int file_count;
    int compress;
} arguments_t;

extern struct argp_option options[];
error_t parse_opt(int key, char *arg, struct argp_state *state);
int parse_arguments(arguments_t *arguments, int argc, char **argv);

/* Our argp parser. */
extern struct argp argp;

#endif // ARGS_H
