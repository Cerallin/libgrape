#include "gidf.h"
#include "grape.h"

#include <nds/arm9/decompress.h>
#include <string.h>

static GRAPE_RET load_gidf_header(grape_bundle_t *bundle, FILE *file,
                                  grape_malloc_func *grape_malloc) {
    GRAPE_RET ret;
    uint8_t count;
    GIDF_Header file_header[1];
    const char SIG_GIDF[4] = {'G', 'I', 'D', 'F'};
    // Reposition to head
    fseek(file, 0, SEEK_SET);
    // Read file header
    fread(file_header, sizeof(GIDF_Header), 1, file);
    if (strncmp(file_header->signatureGIDF, SIG_GIDF, sizeof(SIG_GIDF)) != 0) {
        ret = GRAPE_ERR;
    } else {
        ret = GRAPE_OK;
    }

    count = file_header->diffCount;
    bundle->diff_count = count;
    bundle->diff_list = grape_malloc(count * sizeof(grape_diff_t));

    return ret;
}

GRAPE_RET load_image(grape_bundle_t *bundle, FILE *file, int compressed,
                     grape_malloc_func *grape_malloc) {
    GRAPE_RET ret = GRAPE_OK;
    GIDF_ImageHeader image_header[1];
    const char SIG_IMG[4] = {'I', 'M', 'G', ' '};

    // Read image header
    fread(image_header, sizeof(GIDF_ImageHeader), 1, file);
    if (strncmp(image_header->signatureIMG, SIG_IMG, sizeof(SIG_IMG)) != 0) {
        ret = GRAPE_ERR;
    } else {
        uint32_t size;
        uint32_t buffer_size;
        grape_image_t *img = bundle->base_img;
        // TODO ALIGN4
        img->width = image_header->width;
        img->height = image_header->height;
        buffer_size = image_header->imageSize;
        img->buffer->as_ptr =
            grape_malloc(img->width * img->height * sizeof(img->pixel_size));
        // Load image content
        if (compressed) {
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

GRAPE_RET load_diff_list(grape_bundle_t *bundle, FILE *file,
                         grape_malloc_func *grape_malloc) {
    uint32_t len;
    GRAPE_RET ret = GRAPE_OK;
    GIDF_DiffHeader diff_header[1];
    const char SIG_DIFF[4] = {'D', 'I', 'F', 'F'};

    for (uint8_t i = 0; i < bundle->diff_count; i++) {
        grape_diff_t *diff = &bundle->diff_list[i];
        // Read diff header
        fread(diff_header, sizeof(GIDF_DiffHeader), 1, file);
        int cmp =
            strncmp(diff_header->signatureDIFF, SIG_DIFF, sizeof(SIG_DIFF));
        if (cmp != 0) {
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

GRAPE_RET grape_bundle_load(grape_bundle_t *bundle, FILE *file,
                            int compressed, grape_malloc_func *grape_malloc) {
    load_gidf_header(bundle, file, grape_malloc);
    load_image(bundle, file, compressed, grape_malloc);
    load_diff_list(bundle, file, grape_malloc);

    return GRAPE_OK;
}

void grape_bundle_free(grape_bundle_t *bundle,
                       grape_free_func *grape_free) {
    for (int i = 0; i < bundle->diff_count; i++) {
        grape_diff_t *diff = &bundle->diff_list[i];
        grape_free(diff->data);
    }
    grape_free(bundle->diff_list);
    grape_free(bundle->base_img->buffer->as_ptr);
}
