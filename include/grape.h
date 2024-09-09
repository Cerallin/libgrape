#ifndef GRAPE_H
#define GRAPE_H

#include <stddef.h>

#ifndef ALIGN4
#define ALIGN4(n) (((n) + (typeof(n))3) & (typeof(n))~3)
#endif

typedef int GRAPE_RET;

#define GRAPE_OK   ((GRAPE_RET)0)
#define GRAPE_FAIL ((GRAPE_RET)1)
#define GRAPE_ERR  ((GRAPE_RET)(-1))

typedef void *grape_malloc_func(size_t);
typedef void grape_free_func(void *);

#include "gidf.h"
#include "image.h"

#include "bundle_dump.h"
#include "bundle_load.h"

#endif // GRAPE_H
