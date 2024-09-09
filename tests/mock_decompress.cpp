#include <grit/cprs.h>
#include <nds/arm9/decompress.h>

#include <iostream>

static constexpr ECprsTag convertType(DecompressType type) {
    const ECprsTag decompress_type_map[] = {
        [LZ77] = CPRS_LZ77_TAG,     // GBA LZ77 compression.
        [LZ77Vram] = CPRS_LZ77_TAG, //
        [HUFF] = CPRS_HUFF_TAG,     // GBA Huffman, 8bit.
        [RLE] = CPRS_RLE_TAG,       // GBA RLE compression.
        [RLEVram] = CPRS_RLE_TAG,   //
    };
    return decompress_type_map[type];
}

void decompress(const void *data, void *dst, DecompressType type) {
    RECORD dstRecord = {.data = (BYTE *)dst};
    RECORD srcRecord = {.data = (BYTE *)data};
    bool res = cprs_decompress(&dstRecord, &srcRecord);
    if (res != true) {
        std::cerr << "Warning: decompress failed.";
    }
}
