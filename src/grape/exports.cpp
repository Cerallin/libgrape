#include "grape.h"
#include "grape/bundle.hpp"

static uint8_t _imageType;

grape_bundle_ptr_t grape_init(uint8_t imageWidth, uint8_t imageHeight,
                              uint8_t imageType) {
    _imageType = imageType;

    if (_imageType == IMG_8B_256_COLOR) {
        return new Grape::Bundle<uint8_t>(imageWidth, imageHeight);
    } else if (_imageType == IMG_16B_TRUE_COLOR) {
        return new Grape::Bundle<uint16_t>(imageWidth, imageHeight);
    } else {
        return nullptr;
    }
}

GRAPE_RET grape_add_file(grape_bundle_ptr_t bundlePtr, const char *filename,
                         ssize_t len) {
    GRAPE_RET ret;

    if (_imageType == IMG_8B_256_COLOR) {
        ret = ((Grape::Bundle<uint8_t> *)bundlePtr)->Add(filename, len);
    } else if (_imageType == IMG_16B_TRUE_COLOR) {
        ret = ((Grape::Bundle<uint16_t> *)bundlePtr)->Add(filename, len);
    } else {
        ret = GRAPE_ERR;
    }

    return ret;
}

GRAPE_RET grape_dump(const grape_bundle_ptr_t bundlePtr,
                     const char *outputFile) {
    if (_imageType == IMG_8B_256_COLOR) {
        ((Grape::Bundle<uint8_t> *)bundlePtr)->Dump(outputFile);
    } else if (_imageType == IMG_16B_TRUE_COLOR) {
        ((Grape::Bundle<uint16_t> *)bundlePtr)->Dump(outputFile);
    } else {
        return GRAPE_ERR;
    }

    return GRAPE_OK;
}

void grape_deinit(grape_bundle_ptr_t bundlePtr) {
    if (_imageType == IMG_8B_256_COLOR) {
        delete ((Grape::Bundle<uint8_t> *)bundlePtr);
    } else if (_imageType == IMG_16B_TRUE_COLOR) {
        delete ((Grape::Bundle<uint16_t> *)bundlePtr);
    } else {
        // Unhandled
    }
}
