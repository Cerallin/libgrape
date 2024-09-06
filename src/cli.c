#include "grape.h"

#include <stdio.h>

int main() {
    int res;
    GrapeBundlePtr bundle = grape_init(6, 2, IMG_8B_256_COLOR);
    const char hello_world[] = "../ext/hello_world.txt";
    const char ciallo_world[] = "../ext/ciallo_world.txt";

    grape_add_file(bundle, hello_world, sizeof(hello_world));
    grape_add_file(bundle, ciallo_world, sizeof(ciallo_world));
    res = grape_dump(bundle, "./test.txt");
    if (res != GRAPE_OK) {
        fprintf(stderr, "Dump failed.\n");
    }

    grape_deinit(bundle);

    return 0;
}
