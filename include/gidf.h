#ifndef GRAPE_GIDF_H
#define GRAPE_GIDF_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define IMG_8B_256_COLOR   ((uint8_t)0x0F)
#define IMG_16B_TRUE_COLOR ((uint8_t)0xFF)

typedef struct _GIDF_DiffHeader {
    uint8_t signatureDIFF[4];
    uint32_t diffSize;
} GIDF_DiffHeader;

typedef struct _GIDF_ImageHeader {
    uint8_t signatureIMG[4];
    uint32_t imageSize;
} GIDF_ImageHeader;

typedef struct GrapeImageDiffFormatHeader {
    uint8_t signatureGIDF[4];
    uint8_t imageType;
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
inline void WriteStruct(std::ostream &outStream, T header) {
    outStream.write(reinterpret_cast<char *>(&header), sizeof(T));
}
#endif

#endif // GRAPE_GIDF_H
