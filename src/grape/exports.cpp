#include "grape.h"
#include "grape/bundle.hpp"

static uint8_t _imageType;

GrapeBundlePtr grape_init(uint8_t imageWidth, uint8_t imageHeight,
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

GRAPE_RET grape_add_file(GrapeBundlePtr bundlePtr, const char *filename,
                         ssize_t len) {
    std::string str(filename, len);

    if (_imageType == IMG_8B_256_COLOR) {
        ((Grape::Bundle<uint8_t> *)bundlePtr)->Add(str);
    } else if (_imageType == IMG_16B_TRUE_COLOR) {
        ((Grape::Bundle<uint16_t> *)bundlePtr)->Add(str);
    } else {
        return GRAPE_ERR;
    }

    return GRAPE_OK;
}

GRAPE_RET grape_dump(const GrapeBundlePtr bundlePtr, const char *outputFile) {
    if (_imageType == IMG_8B_256_COLOR) {
        ((Grape::Bundle<uint8_t> *)bundlePtr)->Dump(outputFile);
    } else if (_imageType == IMG_16B_TRUE_COLOR) {
        ((Grape::Bundle<uint16_t> *)bundlePtr)->Dump(outputFile);
    } else {
        return GRAPE_ERR;
    }

    return GRAPE_OK;
}

void grape_deinit(GrapeBundlePtr bundlePtr) {
    if (_imageType == IMG_8B_256_COLOR) {
        delete ((Grape::Bundle<uint8_t> *)bundlePtr);
    } else if (_imageType == IMG_16B_TRUE_COLOR) {
        delete ((Grape::Bundle<uint16_t> *)bundlePtr);
    } else {
        // Unhandled
    }
}
