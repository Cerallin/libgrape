#ifndef GRAPE_BITMAP_H
#define GRAPE_BITMAP_H

#include "gidf.h"
#include "grape.h"

#include <grit/cprs.h>

#include <cstdint>
#include <iostream>
#include <string>

#ifdef ALIGN4
#undef ALIGN4
template <typename T> constexpr T ALIGN4(T num) {
    return (((num) + (T)3) & (T)~3);
}
#endif

namespace Grape {

template <typename PIXEL_T> class Bitmap {
  private:
    uint16_t width;
    uint16_t height;

    long fileSize;

    uint8_t *buffer;
    bool loaded;

  public:
    Bitmap(uint16_t width, uint16_t height);
    ~Bitmap() {
        if (loaded) {
            delete[] buffer;
        }
    }

    GRAPE_RET LoadFile(std::string fileString) {
        return LoadFile(fileString.c_str());
    }
    GRAPE_RET LoadFile(const char *filename);

    void WriteStream(std::ostream &outStream,
                     ECprsTag compressTag = CPRS_FAKE_TAG) const {
        RECORD src = {
            .width = 1,
            .height = fileSize,
            .data = (BYTE *)buffer,
        };
        RECORD dst = {
            .width = 1,
            .height = fileSize,
            .data = (BYTE *)(new uint8_t[fileSize]),
        };
        if (compressTag == CPRS_FAKE_TAG) {
            // Do not compress
            memcpy(dst.data, src.data, fileSize);
        } else {
            // cprs_compress will free dst.data and "reattach" a new address.
            cprs_compress(&dst, &src, compressTag);
        }
        outStream.write(reinterpret_cast<char *>(dst.data), rec_size(&dst));
        delete[] dst.data;
    }

    long FileSize() const { return fileSize; }
    uint16_t Width() const { return width; }
    uint16_t Height() const { return height; }
    PIXEL_T *Buffer() const { return (PIXEL_T *)buffer; }
};

// Implements
static long fsize(FILE *file) {
    long res;

    fseek(file, 0, SEEK_END);
    res = ftell(file);
    fseek(file, 0, SEEK_SET);

    return res;
}

template <>
Bitmap<uint8_t>::Bitmap(uint16_t width, uint16_t height)
    : width(ALIGN4(width)), height(height), // Image width & height
      fileSize(0),                          // File size
      buffer(nullptr), loaded(false)        // Bitmap buffer
{}

template <>
Bitmap<uint16_t>::Bitmap(uint16_t width, uint16_t height)
    : width(ALIGN4(2 * width) / 2), // Align 2
      height(height), fileSize(0), buffer(nullptr), loaded(false) {}

template <typename PIXEL_T>
GRAPE_RET Bitmap<PIXEL_T>::LoadFile(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == nullptr) {
        goto err_empty;
    }

    fileSize = fsize(file);
    if (fileSize <= 0) {
        goto err_fclose;
    }
    // Read file contents into buffer
    buffer = new uint8_t[fileSize];
    if (fread(buffer, 1, fileSize, file) != fileSize) {
        goto err_allocated;
    }

    loaded = true;

    fclose(file);
    return GRAPE_OK;

err_allocated:
    delete[] buffer;
err_fclose:
    fclose(file);
err_empty:
    return GRAPE_ERR;
}

} // namespace Grape

#endif // GRAPE_BITMAP_H
