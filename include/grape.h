#ifndef GRAPE_H
#define GRAPE_H

#include "gidf.h"
#include "pixel.h"

#include <stdio.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int GRAPE_RET;
#define GRAPE_OK   ((GRAPE_RET)0)
#define GRAPE_FAIL ((GRAPE_RET)1)
#define GRAPE_ERR  ((GRAPE_RET)(-1))

typedef void *grape_bundle_ptr_t;

grape_bundle_ptr_t grape_init(uint8_t imageWidth, uint8_t imageHeight,
                          uint8_t imageType);
GRAPE_RET grape_add_file(grape_bundle_ptr_t bundlePtr, const char *filename,
                         ssize_t len);
GRAPE_RET grape_dump(const grape_bundle_ptr_t bundlePtr, const char *outputFile);
void grape_deinit(grape_bundle_ptr_t bundlePtr);

#ifdef __cplusplus
}
#endif

#endif // GRAPE_H
