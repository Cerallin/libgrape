#ifndef GRAPE_BUNDLE_DUMP_H
#define GRAPE_BUNDLE_DUMP_H

#ifdef GRAPE_DUMP

#include "gidf.h"
#include "image.h"

#include <stdio.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void *grape_bundle_ptr_t;

grape_bundle_ptr_t grape_init(uint8_t imageWidth, uint8_t imageHeight,
                              image_flag_t imageFlag);
GRAPE_RET grape_add_file(grape_bundle_ptr_t bundlePtr, const char *filename,
                         ssize_t len);
GRAPE_RET grape_dump(const grape_bundle_ptr_t bundlePtr,
                     const char *outputFile);
void grape_deinit(grape_bundle_ptr_t bundlePtr);

#ifdef __cplusplus
}
#endif

#endif // GRAPE_DUMP

#endif // GRAPE_BUNDLE_DUMP_H
