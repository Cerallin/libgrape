#include "gidf.h"
#include "grape.h"

#include <nds/arm9/decompress.h>
#include <string.h>

static inline int sigcmp(const char expected[4], const char signature[4]) {
    return memcmp(expected, signature, 4);
}

static GRAPE_RET load_gidf_header(grape_bundle_t *bundle, FILE *file,
                                  image_flag_t *flag,
                                  grape_malloc_func *grape_malloc) {
    GRAPE_RET ret;
    uint8_t count;
    GIDF_Header file_header[1];
    const char SIG_GIDF[4] = {'G', 'I', 'D', 'F'};
    // Reposition to head
    fseek(file, 0, SEEK_SET);
    // Read file header
    fread(file_header, sizeof(GIDF_Header), 1, file);
    if (sigcmp(SIG_GIDF, file_header->signatureGIDF) != 0) {
        ret = GRAPE_ERR;
    } else {
        ret = GRAPE_OK;

        count = file_header->diffCount;
        bundle->diff_count = count;
        bundle->diff_series = grape_malloc(count * sizeof(grape_diff_t));
        if (flag != NULL) {
            *flag = file_header->imageFlag;
        }
    }

    return ret;
}

GRAPE_RET load_image(grape_bundle_t *bundle, FILE *file, image_flag_t flag,
                     grape_malloc_func *grape_malloc) {
    GRAPE_RET ret = GRAPE_OK;
    GIDF_ImageHeader image_header[1];
    const char SIG_IMG[4] = {'I', 'M', 'G', ' '};

    // Read image header
    fread(image_header, sizeof(GIDF_ImageHeader), 1, file);
    if (sigcmp(SIG_IMG, image_header->signatureIMG) != 0) {
        ret = GRAPE_ERR;
    } else {
        uint32_t size;
        uint32_t buffer_size;
        grape_image_t *img = bundle->base_img;
        // Initialize image
        grape_image_init(img, image_header->width, image_header->height, flag,
                         grape_malloc);
        buffer_size = grape_image_size_byte(img);
        // Load image content
        if (flag & IMG_COMPRESSED) {
            char buffer[buffer_size];
            size = fread(buffer, 1, buffer_size, file);
            if (size != buffer_size) {
                ret = GRAPE_ERR;
            } else {
                decompress(buffer, img->buffer->as_ptr, LZ77Vram);
            }
        } else {
            size = fread(img->buffer->as_ptr, 1, buffer_size, file);
            if (size != buffer_size) {
                ret = GRAPE_ERR;
            }
        }
    }

    return ret;
}

GRAPE_RET load_palette(grape_bundle_t *bundle, FILE *file, image_flag_t flag,
                       grape_malloc_func *grape_malloc) {
    GRAPE_RET ret = GRAPE_OK;
    const char SIG_PAL[4] = {'P', 'A', 'L', ' '};

    // Only 8-bit color image has palette
    if (flag & IMG_8B_256_COLOR) {
        GIDF_PaletteHeader paletteHeader[1];
        // Read file header
        fread(paletteHeader, sizeof(GIDF_PaletteHeader), 1, file);
        if (sigcmp(SIG_PAL, paletteHeader->signaturePalette) != 0) {
            ret = GRAPE_ERR;
        } else {
            uint32_t palette_size = paletteHeader->paletteSize;
            void *ptr = grape_malloc(palette_size);

            bundle->palette_size = palette_size;
            bundle->palette = ptr;
            fread(bundle->palette, 1, palette_size, file);

            ret = GRAPE_OK;
        }
    } else {
        // do nothing
    }

    return ret;
}

GRAPE_RET load_diff_series(grape_bundle_t *bundle, FILE *file,
                           grape_malloc_func *grape_malloc) {
    uint32_t len;
    GRAPE_RET ret = GRAPE_OK;
    GIDF_DiffHeader diff_header[1];
    const char SIG_DIFF[4] = {'D', 'I', 'F', 'F'};

    for (uint8_t i = 0; i < bundle->diff_count; i++) {
        grape_diff_t *diff = &bundle->diff_series[i];
        // Read diff header
        fread(diff_header, sizeof(GIDF_DiffHeader), 1, file);
        if (sigcmp(SIG_DIFF, diff_header->signatureDIFF) != 0) {
            ret = GRAPE_ERR;
            break;
        }
        len = diff_header->diffSize;
        diff->len = len;
        diff->data = grape_malloc(len * sizeof(grape_pixel_t));
        // Load diff content
        int res = fread(diff->data, sizeof(grape_pixel_t), len, file);
        if (res != diff->len) {
            ret = GRAPE_ERR;
            break;
        }
    }

    return ret;
}

GRAPE_RET grape_bundle_load_call(grape_bundle_t *bundle, FILE *file,
                                 grape_malloc_func *grape_malloc) {
    GRAPE_RET ret;

    do {
        image_flag_t flag;

        ret = load_gidf_header(bundle, file, &flag, grape_malloc);
        if (ret != GRAPE_OK) {
            break;
        }

        ret = load_image(bundle, file, flag, grape_malloc);
        if (ret != GRAPE_OK) {
            break;
        }

        ret = load_palette(bundle, file, flag, grape_malloc);
        if (ret != GRAPE_OK) {
            break;
        }

        ret = load_diff_series(bundle, file, grape_malloc);
        if (ret != GRAPE_OK) {
            break;
        }
    } while (0);

    return ret;
}

void grape_bundle_free_call(grape_bundle_t *bundle,
                            grape_free_func *grape_free) {
    for (int i = 0; i < bundle->diff_count; i++) {
        grape_diff_t *diff = &bundle->diff_series[i];
        grape_free(diff->data);
    }
    grape_free(bundle->diff_series);
    grape_free(bundle->base_img->buffer->as_ptr);
    if (bundle->palette_size) {
        grape_free(bundle->palette);
    }
}
