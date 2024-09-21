#ifndef GRAPE_GIDF_H
#define GRAPE_GIDF_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define IMG_8B_256_COLOR   ((uint8_t)0x03)
#define IMG_16B_TRUE_COLOR ((uint8_t)0x0C)
#define IMG_COMPRESSED     ((uint8_t)0x10)

typedef uint8_t image_flag_t;

typedef struct _GIDF_DiffHeader {
    uint8_t signatureDIFF[4];
    uint32_t diffSize;
} GIDF_DiffHeader;

typedef struct _GIDF_PaletteHeader {
    uint8_t signaturePalette[4];
    uint32_t paletteSize;
} GIDF_PaletteHeader;

typedef struct _GIDF_ImageHeader {
    uint8_t signatureIMG[4];
    uint16_t width;
    uint16_t height;
    uint32_t imageSize;
} GIDF_ImageHeader;

typedef struct GrapeImageDiffFormatHeader {
    uint8_t signatureGIDF[4];
    image_flag_t imageFlag;
    uint8_t diffCount;
    uint16_t imageWidth;
    uint16_t imageHeight;
    uint16_t preserved;
} GIDF_Header;

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

#include <iostream>

template <typename T>
inline void WriteStruct(std::ostream &outStream, const T &header) {
    outStream.write(reinterpret_cast<const char *>(&header), sizeof(T));
}
#endif

#endif // GRAPE_GIDF_H
