#ifndef GRAPE_BUNDLE_LOAD_H
#define GRAPE_BUNDLE_LOAD_H

#include "image.h"

#ifndef HOST_BUILD
#include <nds/arm9/sassert.h>

#else
#define sassert(...) ((void)0)

#endif

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _grape_image_bundle {
    grape_image_t base_img[1];
    void *palette;
    grape_diff_t *diff_series;
    uint16_t palette_size;
    uint16_t diff_count;
} grape_bundle_t;

GRAPE_RET grape_bundle_load_call(grape_bundle_t *bundle, FILE *file,

                                 grape_malloc_func *grape_malloc);
void grape_bundle_free_call(grape_bundle_t *bundle,
                            grape_free_func *grape_free);

inline GRAPE_RET grape_bundle_load(grape_bundle_t *bundle, FILE *file,
                                   grape_malloc_func *grape_malloc) {
    sassert(grape_malloc != NULL, "grape_malloc(size_t) cannot be NULL!");

    return grape_bundle_load_call(bundle, file, grape_malloc);
}
inline void grape_bundle_free(grape_bundle_t *bundle,
                              grape_free_func *grape_free) {
    sassert(grape_free != NULL, "grape_free() cannot be NULL!");

    return grape_bundle_free_call(bundle, grape_free);
}

#ifdef __cplusplus
}
#endif

#endif // GRAPE_BUNDLE_LOAD_H
