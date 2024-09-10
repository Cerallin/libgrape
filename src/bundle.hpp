#ifndef GRAPE_BUNDLE_H
#define GRAPE_BUNDLE_H

#include "bitmap.hpp"
#include "gidf.h"

#include <fstream>
#include <memory>
#include <unistd.h>
#include <vector>

namespace Grape {

template <typename PIXEL_T> class Bundle {
  private:
    std::vector<std::string> _fileList;
    uint16_t imageWidth;
    uint16_t imageHeight;

    Bitmap<PIXEL_T> *makeBitmap(std::string fileString) const;

  public:
    Bundle(uint16_t imageWidth, uint16_t imageHeight) {
        constexpr size_t pixel_size = sizeof(PIXEL_T);
        this->imageWidth = ALIGN4(pixel_size * imageWidth) / pixel_size;
        this->imageHeight = imageHeight;
    }
    ~Bundle() = default;

    GRAPE_RET Add(std::string filename) {
        if (access(filename.c_str(), F_OK | R_OK) == 0) {
            _fileList.push_back(filename);
            return GRAPE_OK;
        } else {
            return GRAPE_FAIL;
        }
    }

    GRAPE_RET Add(const char *filename, int len) {
        std::string str(filename, len);
        return Add(str);
    }

    GRAPE_RET Dump(std::ostream &outStream,
                   ECprsTag compressTag = CPRS_FAKE_TAG) const;

    GRAPE_RET Dump(std::string fileString,
                   ECprsTag compressTag = CPRS_FAKE_TAG) const {
        return Dump(fileString.c_str(), compressTag);
    }
    GRAPE_RET Dump(const char *filename,
                   ECprsTag compressTag = CPRS_FAKE_TAG) const {
        constexpr auto flags = std::ios::out | std::ios::binary;
        std::ofstream outStream(filename, flags);
        GRAPE_RET ret = Dump(outStream, compressTag);
        outStream.close();

        return ret;
    }

    uint16_t Width(void) const { return imageWidth; }
    uint16_t Height(void) const { return imageHeight; }
};

// Implements

template <typename T> constexpr image_flag_t getImageType(void) {
    image_flag_t flag = 0x00;

    if (std::is_same_v<T, uint8_t>) {
        flag = IMG_8B_256_COLOR;
    } else if (std::is_same_v<T, uint16_t>) {
        flag = IMG_16B_TRUE_COLOR;
    } else {
        //
    }

    return flag;
}

constexpr image_flag_t getCompressType(ECprsTag compressTag) {
    image_flag_t flag = 0x00;

    if (compressTag == CPRS_FAKE_TAG) {
        // do nothing
    } else {
        flag = IMG_COMPRESSED;
    }

    return flag;
}

template <typename PIXEL_T>
Bitmap<PIXEL_T> *Bundle<PIXEL_T>::makeBitmap(std::string fileString) const {
    GRAPE_RET res;
    Bitmap<PIXEL_T> *bitmap = new Bitmap<PIXEL_T>(Width(), Height());
    res = bitmap->LoadFile(fileString);
    if (res != GRAPE_OK) {
        delete bitmap;
        return nullptr;
    }

    return bitmap;
}

template <typename PIXEL_T>
GRAPE_RET Bundle<PIXEL_T>::Dump(std::ostream &outStream,
                                ECprsTag compressTag) const {
    if (_fileList.size() < 2) { // no files to dump
#ifndef NDEBUG
        std::cerr << "Less than 2 files. Abort." << std::endl;
#endif
        return GRAPE_ERR;
    }
    // Dump file header
    {
        GIDF_Header fileHeader = {
            .signatureGIDF = {'G', 'I', 'D', 'F'},
            .imageFlag = getImageType<PIXEL_T>() | getCompressType(compressTag),
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
        if (bitmap.get() == nullptr) {
            return GRAPE_FAIL;
        }
        GIDF_ImageHeader imageHeader = {
            .signatureIMG = {'I', 'M', 'G', ' '},
            .width = bitmap->Width(),
            .height = bitmap->Height(),
            .imageSize = bitmap->FileSize(),
        };
        WriteStruct(outStream, imageHeader);
        bitmap->WriteStream(outStream, compressTag);
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
        if (currBitmap.get() == nullptr) {
            return GRAPE_FAIL;
        }
        if (nextBitmap.get() == nullptr) {
            return GRAPE_FAIL;
        }
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
        {
            GIDF_DiffHeader diffHeader = {
                .signatureDIFF = {'D', 'I', 'F', 'F'},
                .diffSize = diffPixels.size(),
            };
            WriteStruct(outStream, diffHeader);
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
