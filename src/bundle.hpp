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
    Bundle(uint16_t imageWidth, uint16_t imageHeight)
        : imageWidth(imageWidth), imageHeight(imageHeight) {}
    ~Bundle() = default;

    GRAPE_RET Add(std::string filename) {
        if (access(filename.c_str(), F_OK) == 0) {
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
        return 0;
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
