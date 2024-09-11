#include "grape/grape.h"

#include <stdio.h>
#include <stdlib.h>

void *my_malloc(size_t size) { return malloc(size); }
void my_free(void *ptr) { return free(ptr); }

int main() {
    GRAPE_RET res;
    grape_bundle_t bundle[1];

    const char filename[] = "test.bundle";
    FILE *file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Failed to open file: %s\n", filename);
    }

    res = grape_bundle_load(bundle, file, &my_malloc);
    if (res != GRAPE_OK) {
        fprintf(stderr, "Failed to load!\n");
    }

    grape_bundle_free(bundle, my_free);

    return 0;
}