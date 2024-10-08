#ifndef MOCK_BUNDLE_H
#define MOCK_BUNDLE_H

#include "gidf.h"

#include <cstdint>
#include <fstream>
#include <iostream>

#define CONST_EXPR constexpr inline

CONST_EXPR uint16_t IMG_WIDTH = 8;
CONST_EXPR uint16_t IMG_HEIGHT = 2;

CONST_EXPR char hello_world[] = "hello_world";
CONST_EXPR char ciallo_world[] = "ciallo_world";
CONST_EXPR char palette_file[] = "test_palette";

CONST_EXPR GIDF_Header expectedHeader = {
    {'G', 'I', 'D', 'F'}, // header
    IMG_8B_256_COLOR,     // image type: 8bit uncompressed
    (uint8_t)2,           // diff count
    IMG_WIDTH,            // width
    IMG_HEIGHT,           // height
    (uint16_t)0x0000,     // preserved
};

inline void dumpFile(const char *filename, const char *content) {
    std::ofstream stream(filename, std::ios::out);
    stream << content;
    stream.close();
}

inline void generateTestingFiles() {
    dumpFile(hello_world, "_helloxx_worldxx");
    dumpFile(ciallo_world, "cialloxx_worldxx");
    dumpFile(palette_file, "tests_palette");
}

#endif // MOCK_BUNDLE_H
