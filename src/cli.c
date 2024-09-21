#include "grape.h"
#include "text.h"

#include <argp.h>
#include <error.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
    GRAPE_RET res;
    grape_bundle_ptr_t bundle;

    // Default options
    arguments_t arguments[1] = {{
        .image_flag = 0,
        .verbose = 0,
    }};
    int parse_res = parse_arguments(arguments, argc, argv);
    if (parse_res != 0) {
        return parse_res;
    }

    bundle =
        grape_init(arguments->width, arguments->height, arguments->image_flag);

    {
        const char *palette_file = arguments->palette_file;
        if (palette_file) {
            grape_add_palette_file(bundle, palette_file, strlen(palette_file));
        }
    }

    for (int i = 0; i < arguments->file_count; i++) {
        const char *filename = arguments->files[i];
        fprintf(stderr, "Trying file %s\n", filename);
        res = grape_add_file(bundle, filename, strlen(filename));
        if (res != GRAPE_OK) {
            fprintf(stderr, "Failed to open file: %s\n", filename);
            return 1;
        }
    }

    res = grape_dump(bundle, arguments->output_file);
    if (res != GRAPE_OK) {
        fprintf(stderr, "Dump failed.\n");
    }

    grape_deinit(bundle);

    return 0;
}
