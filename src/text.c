#include "config.h"

#include "text.h"

#include <inttypes.h>

const char *argp_program_version = SOFTWARE_STRING;
const char *argp_program_bug_address = BUG_REPORT_URL;

/* Program documentation. */
const char doc[] = SOFTWARE_STRING
    " -- " SOFTWARE_DESCRIPTION "\n"
    "\nThis program is part of libgrape, a free software: you can "
    "redistribute it and/or modify"
    "it under the terms of the GNU General Public License as published by "
    "the Free Software Foundation, either version 3 of the License, or "
    "(at your option) any later version."
    "\v" SOFTWARE_LONG_DESCRIPTION;

/* A description of the arguments we accept. */
const char args_doc[] = "-m WIDTH -n HEIGHT -o OUTPUT_FILE [FILE1 FILE2 ...]";

/* The options we understand. */
struct argp_option options[] = {
    {"verbose", 'v', 0, 0, "Produce verbose output"},
    {"output", 'o', "FILE", 0, "Output to FILE"},
    {"width", 'm', "uint16", 0, "Specify image width"},
    {"height", 'n', "uint16", 0, "Specify image height"},
    {"compress", 'c', 0, 0, "Compress base image (LZ77)"},
    {"palette", 'p', "FILE", 0,
     "Specify palette binary file, usually *.pal.bin"},
    {0, '8', 0, 0, "8-bit images (default)"},
    {0, 'g', 0, 0, "16-bit images"},
    {0, 0, 0, 0, "The following options should be grouped together:"},
    {0},
};

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

    case 'c':
        arguments->image_flag |= IMG_COMPRESSED;
        break;

    case '8':
        arguments->image_flag |= IMG_8B_256_COLOR;
        break;

    case 'g':
        arguments->image_flag |= IMG_16B_TRUE_COLOR;
        break;

    case 'p':
        arguments->palette_file = arg;
        break;

    case 'm':
        if (sscanf(arg, "%" SCNu16, &arguments->width) <= 0) {
            ret = ARGP_KEY_ERROR;
        }
        break;

    case 'n':
        if (sscanf(arg, "%" SCNu16, &arguments->height) <= 0) {
            ret = ARGP_KEY_ERROR;
        }
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

int parse_arguments(arguments_t *arguments, int argc, char **const argv) {
    int ret = 0;
    argp_parse(&argp, argc, argv, 0, 0, arguments);

    if (arguments->output_file == NULL) {
        fprintf(stderr, "Must specify an output file.\n");
        argp_help(&argp, stderr, ARGP_HELP_SHORT_USAGE, argv[0]);
        ret = 1;
    } else if (!arguments->width) {
        fprintf(stderr, "Must specify image width.\n");
        argp_help(&argp, stderr, ARGP_HELP_SHORT_USAGE, argv[0]);
        ret = 1;
    } else if (!arguments->height) {
        fprintf(stderr, "Must specify image height.\n");
        argp_help(&argp, stderr, ARGP_HELP_SHORT_USAGE, argv[0]);
        ret = 1;
    } else if ((arguments->image_flag & IMG_8B_256_COLOR) &&
               (arguments->image_flag & IMG_16B_TRUE_COLOR)) {
        fprintf(stderr, "You can't specify both 8-bit and 16-bit!\n");
        ret = 1;
    } else if (!(arguments->image_flag & IMG_8B_256_COLOR) &&
               !(arguments->image_flag & IMG_16B_TRUE_COLOR)) {
        fprintf(stderr, "Must specify color bit-depth: 8-bit(-8)/16-bit(-g)!\n");
        ret = 1;
    } else if ((arguments->image_flag & IMG_8B_256_COLOR) &&
               arguments->palette_file == NULL) {
        fprintf(stderr, "Must specify a palette file for 8-bit color image!\n");
        ret = 1;
    } else if ((arguments->image_flag & IMG_16B_TRUE_COLOR) &&
               arguments->palette_file != NULL) {
        fprintf(stderr, "Palette file will be ignored for 16-bit color.\n");
    } else {
        //
    }

    return ret;
}
