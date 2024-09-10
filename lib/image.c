#include "grape.h"

#include <assert.h>

#ifndef HOST_BUILD
#include <nds/dma.h>
#include <nds/ndstypes.h>

#else
#define BIT(n) (1 << (n))
#include <string.h>
static inline void dmaFillWords(uint32_t value, void *dest, uint32_t size) {
    memset(dest, value, size);
}

#endif // GRAPE_LIB

#define TRANSPARENT(color)     (!((color) & BIT(15)))
#define BITMAP_OFFSET(x, w, y) (((y) * (w)) + (x))

#define LOOP(it, start, len)                                                   \
    for (typeof((start) + (len))(it) = (start); (it) < (start) + (len); (it)++)
#define LOOP_MATRIX(x, y, w, h) LOOP(i, x, w) LOOP(j, y, h)

#define swap(a, b)                                                             \
    do {                                                                       \
        _Static_assert(                                                        \
            __builtin_types_compatible_p(typeof(*(a)), typeof(*(b))),          \
            "Swap arguments have incompatible types.");                        \
        typeof(*(a)) _tmp = *(a);                                              \
        *(a) = *(b);                                                           \
        *(b) = _tmp;                                                           \
    } while (0)

void grape_image_init(grape_image_t *img, int width, int height,
                      image_flag_t image_flag,
                      grape_malloc_func *grape_malloc) {
    size_t size;
    if (image_flag & IMG_8B_256_COLOR) {
        img->pixel_size = 1;
        img->width = ALIGN4(width);
    } else {
        img->pixel_size = 2;
        img->width = ALIGN4(2 * width) / 2;
    }
    img->height = height;

    if (grape_malloc != NULL) {
        size = grape_image_size_byte(img);
        img->buffer->as_ptr = grape_malloc(size);
    }
}

size_t grape_image_size(const grape_image_t *img) {
    return img->width * img->height;
}

size_t grape_image_size_byte(const grape_image_t *img) {
    return grape_image_size(img) * img->pixel_size;
}

void grape_image_flush(grape_image_t *img) {
    int size = grape_image_size_byte(img);
    dmaFillWords(0, img->buffer->as_ptr, size);
}

GRAPE_RET grape_image_composite(grape_image_t *dest, const grape_image_t *src,
                                int x, int y, grape_area_t area) {
    if (dest->pixel_size != src->pixel_size) {
        return GRAPE_ERR;
    }

    int offset;
    int x_start = x;
    int y_start = y;
    uint8_t pixel_size = dest->pixel_size;

    if (x < 0) {
        area.width += x;
        area.x_off -= x;
        if ((area.x_off + area.width) > src->width) {
            area.width = src->width - area.x_off;
        }
        x_start = 0;
    } else if ((x + area.width) > dest->width) {
        area.width = dest->width - x;
    } else {
        // nothing to do
    }

    if (y < 0) {
        area.height += y;
        area.y_off -= y;
        if ((area.y_off + area.height) > src->height) {
            area.height = src->height - area.y_off;
        }
        y_start = 0;
    } else if ((y + area.height) > dest->height) {
        area.height = dest->height - y;
    } else {
        // nothing to do
    }

    if (pixel_size == 1) {
        LOOP_MATRIX(x_start, y_start, area.width, area.height) {
            offset = BITMAP_OFFSET((i - x_start + area.x_off), src->width,
                                   (j - y_start + area.y_off));
            uint8_t color = src->buffer->as_u8[offset];
            if (TRANSPARENT(color)) {
                continue;
            }

            offset = BITMAP_OFFSET(i, dest->width, j);
            dest->buffer->as_u8[offset] = color;
        }
    } else if (pixel_size == 2) {
        LOOP_MATRIX(x_start, y_start, area.width, area.height) {
            offset = BITMAP_OFFSET((i - x_start + area.x_off), src->width,
                                   (j - y_start + area.y_off));
            uint16_t color = src->buffer->as_u16[offset];
            if (TRANSPARENT(color)) {
                continue;
            }

            offset = BITMAP_OFFSET(i, dest->width, j);
            dest->buffer->as_u16[offset] = color;
        }
    } else {
        return GRAPE_FAIL;
    }

    return GRAPE_OK;
}

GRAPE_RET grape_image_cover(grape_image_t *dest, const grape_image_t *src,
                            int x, int y) {
    return grape_image_composite(dest, src, x, y,
                                 (grape_area_t){.width = src->width,
                                                .height = src->height,
                                                .x_off = 0,
                                                .y_off = 0});
}

GRAPE_RET grape_image_crop(grape_image_t *dest, const grape_image_t *src,
                           int x_off, int y_off) {
    return grape_image_composite(dest, src, 0, 0,
                                 (grape_area_t){.width = dest->width,
                                                .height = dest->height,
                                                .x_off = x_off,
                                                .y_off = y_off});
}

GRAPE_RET grape_image_load_diff(grape_image_t *img, const grape_diff_t *diff) {
    GRAPE_RET ret = GRAPE_OK;
    uint8_t pixel_size = img->pixel_size;

    if (pixel_size == 1) {
        LOOP(i, 0, diff->len) {
            grape_pixel_t data = diff->data[i];
            int offset = BITMAP_OFFSET(data.x_off, img->width, data.y_off);
            img->buffer->as_u8[offset] = data.color;
        }
    } else if (pixel_size == 2) {
        LOOP(i, 0, diff->len) {
            grape_pixel_t data = diff->data[i];
            int offset = BITMAP_OFFSET(data.x_off, img->width, data.y_off);
            img->buffer->as_u16[offset] = data.color;
        }
    } else {
        ret = GRAPE_FAIL;
    }

    return ret;
}

GRAPE_RET grape_image_horizontal_flip(grape_image_t *img) {
    GRAPE_RET ret = GRAPE_OK;
    uint8_t pixel_size = img->pixel_size;

    if (pixel_size == 1) {
        uint8_t *row = img->buffer->as_u8;

        LOOP(i, 0, img->height) {
            uint8_t *left = row;
            uint8_t *right = &row[img->width - 1];

            LOOP(j, 0, img->width / 2) {
                swap(left, right);
                left++;
                right--;
            }

            row = &row[img->width];
        }
    } else if (pixel_size == 2) {
        uint16_t *row = img->buffer->as_u16;

        LOOP(i, 0, img->height) {
            uint16_t *left = row;
            uint16_t *right = &row[img->width - 1];

            LOOP(j, 0, img->width / 2) {
                swap(left, right);
                left++;
                right--;
            }

            row = &row[img->width];
        }
    } else {
        ret = GRAPE_FAIL;
    }

    return ret;
}

GRAPE_RET grape_image_vertical_flip(grape_image_t *img) {
    GRAPE_RET ret = GRAPE_OK;
    uint8_t pixel_size = img->pixel_size;

    if (pixel_size == 1) {
        uint8_t *top_row = img->buffer->as_u8;
        uint8_t *bottom_row = &top_row[(img->height - 1) * img->width];

        LOOP_MATRIX(0, 0, img->height / 2, img->width) {
            top_row = &top_row[img->width];
            bottom_row = &bottom_row[-img->width];
        }
    } else if (pixel_size == 2) {
        uint16_t *top_row = img->buffer->as_u16;
        uint16_t *bottom_row = &top_row[(img->height - 1) * img->width];

        LOOP_MATRIX(0, 0, img->height / 2, img->width) {
            top_row = &top_row[img->width];
            bottom_row = &bottom_row[-img->width];
        }
    } else {
        ret = GRAPE_FAIL;
    }

    return ret;
}

void diff_flip(grape_diff_t *diff, int width) {
    assert((width % 2) == 0);

    LOOP(i, 0, diff->len) {
        grape_pixel_t *data = &diff->data[i];
        data->x_off += width - (2 * data->x_off) - 1;
    }
}

void diff_series_flip(grape_diff_t *diff_series, int width, int count) {
    LOOP(i, 0, count) {
        grape_diff_t *diff = &diff_series[i];
        diff_flip(diff, width);
    }
}
