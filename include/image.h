#ifndef GRAPE_IMAGE_H
#define GRAPE_IMAGE_H

#include <stdint.h>

typedef struct _grape_pixel {
    uint16_t color;
    uint8_t x_off;
    uint8_t y_off;
} grape_pixel_t;

typedef struct _grape_diff_info_t {
    grape_pixel_t *data;
    uint32_t len;
} grape_diff_t;

typedef struct _grape_image {
    int width;
    int height;
    uint8_t pixel_size;
    union _grape_image_buffer {
        uint8_t *as_u8;
        uint16_t *as_u16;
        void *as_ptr;
    } buffer[1];
} grape_image_t;

typedef struct _grape_image_bundle {
    grape_image_t base_img[1];
    grape_diff_t *diff_list;
    int diff_count;
} grape_bundle_t;

#endif // GRAPE_IMAGE_H
