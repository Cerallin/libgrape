#include "grape/grape.h"

#include <stdio.h>
#include <stdlib.h>

#include <nds.h>

int main() {
    GRAPE_RET res;
    grape_diff_t *diff;
    grape_bundle_t bundle[1];
    int count = 0;

    // suppose bundle is loaded
    // ...

    grape_image_t *img = bundle->base_img;
    // copy image to BG GFX
    dmaCopy(img->buffer->as_ptr, bgGetGfxPtr(1), grape_image_size_byte(img));

    while (1) {
        swiWaitForVBlank();
        diff = bundle->diff_series[count];
        grape_image_load_diff(img, diff);
        count++;
        count %= bundle->diff_count;
    }

    return 0;
}