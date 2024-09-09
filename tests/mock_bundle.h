#ifndef MOCK_BUNDLE_H
#define MOCK_BUNDLE_H

#include "gidf.h"

#include <cstdint>
#include <fstream>
#include <iostream>

#define CONST_EXPR constexpr inline

CONST_EXPR uint16_t IMG_WIDTH = 6;
CONST_EXPR uint16_t IMG_HEIGHT = 2;

CONST_EXPR char hello_world[] = "_hello_world";
CONST_EXPR char ciallo_world[] = "ciallo_world";

CONST_EXPR GIDF_Header expectedHeader = {
    {'G', 'I', 'D', 'F'}, // header
    IMG_8B_256_COLOR,     // image type: 8bit uncompressed
    (uint8_t)2,           // diff count
    IMG_WIDTH,            // width
    IMG_HEIGHT,           // height
    (uint16_t)0x0000,     // preserved
};

inline void dumpFile(const char *filename) {
    std::ofstream stream(filename, std::ios::out);
    stream << filename;
    stream.close();
}

inline void generateTestingFiles() {
    dumpFile(hello_world);
    dumpFile(ciallo_world);
}

#endif // MOCK_BUNDLE_H
