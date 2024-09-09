#include "config.h"

#include "text.h"

const char *argp_program_version = SOFTWARE_STRING;
const char *argp_program_bug_address = BUG_REPORT_URL;

/* Program documentation. */
const char doc[] = SOFTWARE_STRING
    " -- " SOFTWARE_DESCRIPTION "\n"
    "\nThis program is part of libgrape, and is free software: you can "
    "redistribute it and/or modify"
    "it under the terms of the GNU General Public License as published by "
    "the Free Software Foundation, either version 3 of the License, or "
    "(at your option) any later version."
    "\v" SOFTWARE_LONG_DESCRIPTION;

/* A description of the arguments we accept. */
const char args_doc[] = "-o OUTPUT_FILE [FILE1 FILE2 ...]";

/* The options we understand. */
struct argp_option options[] = {
    {"verbose", 'v', 0, 0, "Produce verbose output"},
    {"output", 'o', "FILE", 0, "Output to FILE instead of standard output"},

    {0, 0, 0, 0, "The following options should be grouped together:"},
    {0}};

struct argp argp = {options, parse_opt, args_doc, doc};

/* Parse a single option. */
error_t parse_opt(int key, char *arg, struct argp_state *state) {
    error_t ret = 0;
    arguments_t *arguments = state->input;

    switch (key) {
    case 'v':
        arguments->verbose = true;
        break;

    case 'o':
        arguments->output_file = arg;
        break;

    case ARGP_KEY_NO_ARGS:
        argp_usage(state);

    case ARGP_KEY_ARG:
        arguments->files = &state->argv[state->next - 1];
        arguments->file_count = state->argc - (state->next - 1);
        // Go to end to stop parsing
        state->next = state->argc;
        break;

    default:
        ret = ARGP_ERR_UNKNOWN;
    }
    return ret;
}

int parse_arguments(arguments_t *arguments, int argc, char **argv) {
    argp_parse(&argp, argc, argv, 0, 0, arguments);

    if (!arguments->output_file) {
        fprintf(stderr, "Must specify an output file.\n");
        argp_help(&argp, stderr, ARGP_HELP_SHORT_USAGE, argv[0]);
        return 1;
    }

    return 0;
}
