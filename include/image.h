#ifndef GRAPE_IMAGE_H
#define GRAPE_IMAGE_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef HOST_BUILD
#include <nds/ndstypes.h>
#else
#define ITCM_CODE
#endif // HOST_BUILD

typedef struct _area {
    int x_off;
    int y_off;
    int width;
    int height;
} grape_area_t;

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

// Initialize image
void grape_image_init(grape_image_t *img, int width, int height,
                      image_flag_t image_flag, grape_malloc_func *grape_malloc);
// Returns buffer size.
size_t grape_image_size(const grape_image_t *img);
// Returns buffer size in byte.
size_t grape_image_size_byte(const grape_image_t *img);
// Clean up an image.
void grape_image_flush(grape_image_t *img);
// Composite two images.
GRAPE_RET grape_image_composite(grape_image_t *dest, const grape_image_t *src,
                                int x, int y, grape_area_t area) ITCM_CODE;
// Copy one image to another.
GRAPE_RET grape_image_cover(grape_image_t *dest, const grape_image_t *src,
                            int x, int y);
// Crop one image into another.
GRAPE_RET grape_image_crop(grape_image_t *dest, const grape_image_t *src,
                           int x_off, int y_off);
// Update image with the diff info
GRAPE_RET grape_image_load_diff(grape_image_t *img,
                                const grape_diff_t *diff) ITCM_CODE;
// Flip an image horizontally
GRAPE_RET grape_image_horizontal_flip(grape_image_t *img);
// Flip an image vertically
GRAPE_RET grape_image_vertical_flip(grape_image_t *img);
// Flit a diff
void diff_flip(grape_diff_t *diff, int width);
// Flip diff series
void diff_series_flip(grape_diff_t *diff_series, int width, int count);

#ifdef __cplusplus
}
#endif

#endif // GRAPE_IMAGE_H
