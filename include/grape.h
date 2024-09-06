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

typedef void *GrapeBundlePtr;

GrapeBundlePtr grape_init(uint8_t imageWidth, uint8_t imageHeight,
                          uint8_t imageType);
GRAPE_RET grape_add_file(GrapeBundlePtr bundlePtr, const char *filename,
                         ssize_t len);
GRAPE_RET grape_dump(const GrapeBundlePtr bundlePtr, const char *outputFile);
void grape_deinit(GrapeBundlePtr bundlePtr);

#ifdef __cplusplus
}
#endif

#endif // GRAPE_H
