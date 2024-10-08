#include "bundle.hpp"
#include "grape.h"

static uint8_t _imageFlag;

static constexpr bool isCompressed(image_flag_t imageFlag) {
    return (imageFlag & IMG_COMPRESSED);
}

static constexpr bool is8BitImage(image_flag_t imageFlag) {
    return (imageFlag & IMG_8B_256_COLOR);
}

static constexpr bool is16BitImage(image_flag_t imageFlag) {
    return (imageFlag & IMG_16B_TRUE_COLOR);
}

grape_bundle_ptr_t grape_init(uint16_t imageWidth, uint16_t imageHeight,
                              image_flag_t imageFlag) {
    _imageFlag = imageFlag;

    if (is8BitImage(_imageFlag)) {
        return new Grape::Bundle<uint8_t>(imageWidth, imageHeight);
    } else if (is16BitImage(_imageFlag)) {
        return new Grape::Bundle<uint16_t>(imageWidth, imageHeight);
    } else {
        return nullptr;
    }
}

GRAPE_RET grape_add_file(grape_bundle_ptr_t bundlePtr, const char *filename,
                         ssize_t len) {
    GRAPE_RET ret;

    if (is8BitImage(_imageFlag)) {
        ret = ((Grape::Bundle<uint8_t> *)bundlePtr)->Add(filename, len);
    } else if (is16BitImage(_imageFlag)) {
        ret = ((Grape::Bundle<uint16_t> *)bundlePtr)->Add(filename, len);
    } else {
        ret = GRAPE_ERR;
    }

    return ret;
}

GRAPE_RET grape_add_palette_file(grape_bundle_ptr_t bundlePtr,
                                 const char *palette_file, ssize_t len) {
    GRAPE_RET ret;

    if (is8BitImage(_imageFlag)) {
        ret = ((Grape::Bundle<uint8_t> *)bundlePtr)
                  ->AddPalette(palette_file, len);
    } else if (is16BitImage(_imageFlag)) {
        ret = ((Grape::Bundle<uint16_t> *)bundlePtr)
                  ->AddPalette(palette_file, len);
    } else {
        ret = GRAPE_ERR;
    }

    return ret;
}

GRAPE_RET grape_dump(const grape_bundle_ptr_t bundlePtr,
                     const char *outputFile) {
    auto compressFlag =
        isCompressed(_imageFlag) ? CPRS_LZ77_TAG : CPRS_FAKE_TAG;

    if (is8BitImage(_imageFlag)) {
        ((Grape::Bundle<uint8_t> *)bundlePtr)->Dump(outputFile, compressFlag);
    } else if (is16BitImage(_imageFlag)) {
        ((Grape::Bundle<uint16_t> *)bundlePtr)->Dump(outputFile, compressFlag);
    } else {
        return GRAPE_ERR;
    }

    return GRAPE_OK;
}

void grape_deinit(grape_bundle_ptr_t bundlePtr) {
    if (is8BitImage(_imageFlag)) {
        delete ((Grape::Bundle<uint8_t> *)bundlePtr);
    } else if (is16BitImage(_imageFlag)) {
        delete ((Grape::Bundle<uint16_t> *)bundlePtr);
    } else {
        // Unhandled
    }
}
