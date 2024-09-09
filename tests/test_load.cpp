#include "config.h"

#include <unistd.h>

#include "CppUTest/TestHarness.h"

#include "grape.h"
#include "mock_bundle.h"

extern void *testing_malloc(size_t size);
extern void testing_free(void *ptr);

void *testing_malloc(size_t size) {
    void *buffer = malloc(size);
    return buffer;
}

void testing_free(void *ptr) { free(ptr); }

TEST_GROUP(TestGrapeBundleLoad){};

TEST(TestGrapeBundleLoad, TestLoad) {
    grape_bundle_t bundle[1];
    const char filename[] = "test.bundle";
    CHECK_EQUAL(0, access(filename, F_OK));

    auto file = fopen(filename, "rb");
    CHECK_EQUAL(GRAPE_OK, grape_bundle_load(bundle, file, 0, &testing_malloc));
    fclose(file);

    CHECK_EQUAL(expectedHeader.diffCount, bundle->diff_count);

    grape_bundle_free(bundle, &testing_free);
}
