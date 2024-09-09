#include "config.h"

#include "CppUTest/TestHarness.h"

#include "grape.h"
#include "image.h"

#ifndef BIT
constexpr uint16_t BIT(uint8_t n) { return (1 << (n)); }
#endif

constexpr int IMG_WIDTH = 3;
constexpr int IMG_HEIGHT = 1;

extern void *testing_malloc(size_t size);
extern void testing_free(void *ptr);

static grape_image_t *img;
static const uint16_t ciao[] = {
    'c' | BIT(15),
    'i' | BIT(15),
    'a' | BIT(15),
    'o' | BIT(15),
};

TEST_GROUP(TestGrapeImage) {

    void setup() {
        img = new grape_image_t();
        grape_image_init(img, IMG_WIDTH, IMG_HEIGHT, IMG_16B_TRUE_COLOR,
                         testing_malloc);
        memcpy(img->buffer->as_ptr, ciao, sizeof(ciao));
    };

    void teardown() {
        testing_free(img->buffer->as_ptr);
        delete img;
    };
};

TEST(TestGrapeImage, TestGrapeImageSize) {
    CHECK_EQUAL(0, img->width % 2);
    CHECK_EQUAL(ALIGN4(2 * IMG_WIDTH) / 2, img->width);
}

TEST(TestGrapeImage, TestGrapeImageSizeByte) {
    const uint16_t testing_buffer[(ALIGN4(2 * IMG_WIDTH) / 2) * IMG_HEIGHT] = {
        0,
    };
    CHECK_EQUAL((size_t)(img->width * img->height), grape_image_size(img));
    CHECK_EQUAL(sizeof(testing_buffer), grape_image_size_byte(img));
}

TEST(TestGrapeImage, TestGrapeImageComposite) {
    grape_image_t sm_img[1] = {{.width = 4, .height = 1, .pixel_size = 2}};
    sm_img->buffer->as_ptr = (void *)ciao;

    grape_image_composite(img, sm_img, 0, 0,
                          (grape_area_t){
                              0,
                              0,
                              sm_img->width,
                              sm_img->height,
                          });
    MEMCMP_EQUAL(sm_img->buffer->as_ptr, img->buffer->as_ptr,
                 grape_image_size_byte(sm_img));
}

TEST(TestGrapeImage, TestGrapeImageLoadDiff) {
    grape_pixel_t pixels[2] = {{.color = 'e', .x_off = 1, .y_off = 0},
                               {.color = 'l', .x_off = 2, .y_off = 0}};
    const grape_diff_t diff = {.data = pixels, .len = 2};
    const uint16_t expected[] = {'c' | BIT(15), 'e', 'l', 'o' | BIT(15)};
    CHECK_EQUAL(GRAPE_OK, grape_image_load_diff(img, &diff));
    MEMCMP_EQUAL(expected, img->buffer->as_ptr, sizeof(expected));
}

TEST(TestGrapeImage, TestGrapeImageHorizontalFlip) {
    const uint16_t expected[] = {
        'o' | BIT(15),
        'a' | BIT(15),
        'i' | BIT(15),
        'c' | BIT(15),
    };
    CHECK_EQUAL(GRAPE_OK, grape_image_horizontal_flip(img));
    MEMCMP_EQUAL(expected, img->buffer->as_ptr, sizeof(expected));
}

TEST(TestGrapeImage, TestGrapeImageVerticalFlip) {
    CHECK_EQUAL(GRAPE_OK, grape_image_vertical_flip(img));
    MEMCMP_EQUAL(ciao, img->buffer->as_ptr, sizeof(ciao));
}
