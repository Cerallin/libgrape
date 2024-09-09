#ifndef GRAPE_BUNDLE_LOAD_H
#define GRAPE_BUNDLE_LOAD_H

#ifdef GRAPE_LOAD

#include "image.h"

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void *grape_malloc_func(size_t);
typedef void grape_free_func(void *);

GRAPE_RET grape_bundle_load(grape_bundle_t *bundle, FILE *file,
                            int compressed, grape_malloc_func *grape_malloc);
void grape_bundle_free(grape_bundle_t *bundle,
                       grape_free_func *grape_free);

#ifdef __cplusplus
}
#endif

#endif // GRAPE_LOAD

#endif // GRAPE_BUNDLE_LOAD_H
