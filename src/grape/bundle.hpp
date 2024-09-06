#ifndef GRAPE_BUNDLE_H
#define GRAPE_BUNDLE_H

#include "gidf.h"
#include "grape.h"
#include "grape/bitmap.hpp"

#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace Grape {

template <typename PIXEL_T> class Bundle {
  private:
    std::vector<std::string> _fileList;
    uint8_t imageWidth;
    uint8_t imageHeight;

    Bitmap<PIXEL_T> *makeBitmap(std::string fileString) const;

  public:
    Bundle(uint8_t imageWidth, uint8_t imageHeight)
        : imageWidth(imageWidth), imageHeight(imageHeight) {}
    ~Bundle() = default;

    void Add(std::string filename) { _fileList.push_back(filename); }

    GRAPE_RET Dump(std::ostream &outStream) const;

    GRAPE_RET Dump(std::string fileString) const {
        return Dump(fileString.c_str());
    }
    GRAPE_RET Dump(const char *filename) const {
        constexpr auto flags = std::ios::out | std::ios::binary;
        std::ofstream outStream(filename, flags);

        return Dump(outStream);
    }

    uint8_t Width(void) const { return imageWidth; }
    uint8_t Height(void) const { return imageHeight; }
};

// Implements

template <typename T> constexpr uint8_t getImageType(void) {
    if (std::is_same_v<T, uint8_t>) {
        return IMG_8B_256_COLOR;
    } else if (std::is_same_v<T, uint16_t>) {
        return IMG_16B_TRUE_COLOR;
    } else {
        return NULL;
    }
}

template <typename PIXEL_T>
Bitmap<PIXEL_T> *Bundle<PIXEL_T>::makeBitmap(std::string fileString) const {
    GRAPE_RET res;
    Bitmap<PIXEL_T> *bitmap = new Bitmap<PIXEL_T>(Width(), Height());
    res = bitmap->LoadFile(fileString);
    if (res != GRAPE_OK) {
        return nullptr;
    }

    return bitmap;
}

template <typename PIXEL_T>
GRAPE_RET Bundle<PIXEL_T>::Dump(std::ostream &outStream) const {
#ifndef NDEBUG
    for (auto str : _fileList) {
        fprintf(stderr, "%s\n", str.c_str());
    }
#endif
    // Dump file header
    {
        GIDF_Header fileHeader = {
            .signatureGIDF = {'G', 'I', 'D', 'F'},
            .imageType = getImageType<PIXEL_T>(),
            .diffCount = _fileList.size(),
            .imageWidth = imageWidth,
            .imageHeight = imageHeight,
        };
        WriteStruct(outStream, fileHeader);
    }
    // Save first image
    {
        auto curr = _fileList.begin();
        auto bitmap = std::unique_ptr<Bitmap<PIXEL_T>>(makeBitmap(curr[0]));
        GIDF_ImageHeader imageHeader = {
            .signatureIMG = {'I', 'M', 'G', ' '},
            .imageSize = bitmap->FileSize(),
        };
        WriteStruct(outStream, imageHeader);
        bitmap->WriteStream(outStream);
    }
    // Iterate pixels
    for (auto curr = _fileList.begin(); curr != _fileList.end(); curr++) {
        auto next = std::next(curr);
        if (next == _fileList.end()) {
            next = _fileList.begin();
        }

        // Load bitmaps
        auto currBitmap = std::unique_ptr<Bitmap<PIXEL_T>>(makeBitmap(curr[0]));
        auto nextBitmap = std::unique_ptr<Bitmap<PIXEL_T>>(makeBitmap(next[0]));
        auto currBuffer = currBitmap->Buffer();
        auto nextBuffer = nextBitmap->Buffer();
        auto offsetSize = currBitmap->FileSize() / sizeof(PIXEL_T);
        auto width = currBitmap->Width();
        // Generate diff
        std::vector<grape_pixel_t> diffPixels;
        for (ssize_t offset = 0; offset < offsetSize; offset++) {
            uint8_t i = offset % width;
            uint8_t j = offset / width;
            if (currBuffer[offset] != nextBuffer[offset]) {
                diffPixels.push_back((grape_pixel_t){
                    .color = nextBuffer[offset],
                    .x_off = i,
                    .y_off = j,
                });
            }
        }
        // Write to out stream
        for (auto pixel : diffPixels) {
            WriteStruct(outStream, pixel);
        }
    }

    return GRAPE_OK;
}

} // namespace Grape

#endif // GRAPE_BUNDLE_H
