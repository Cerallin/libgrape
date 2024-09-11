## Libgrape

A "image difference" packing/loading library for NDS.

### What is it for

Suppose you have a series of images, e.g.
```
chara_walk_0.png
chara_walk_1.png
chara_walk_2.png
chara_walk_3.png
```
which describe a set of character walking animations.
When the character walks, these images are sequentially displayed in a loop.

Evidently, loading all these images into memory simultaneously is resource-intensive, and frequent memory copying is highly time-consuming (even with DMA).
To address this issue, libgrape provides a solution: for a series of images, it is sufficient to retain only the first image and the pixel differences between each pair of adjacent images.
When displaying, you just need to modify the pixels to be updated.

The packed file consists of three parts: a file header, a base image, and multiple image differences.
The file header contains information such as image dimensions and the number of differences.
Once the base image is loaded, you can update it by applying `diff 1` to it, then `diff 2`, etc.

The "difference pixels" are those differ between `image (n+1)` and `image n`.
Therefore, we need to store both color and positional information of these pixels.
The definition is as follows:
```c
typedef struct {
    uint16_t color;
    uint8_t x_off;
    uint8_t y_off;
} grape_pixel_t;
```

Given considerations for image compatibility (8-bit/16-bit) and 4K alignment, the pixel color is represented as 16-bit.

For specific usage examples, please see examples in `examples` directory.

### Installation

```
cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/path/to/install \
    /path/to/this-project
```

The header and library files are for DS projects.
The executable `grape` is for bundle generating, running on your machine instead of DS.

Please use `grape --help` to see more.

### Functions

- [x] Data dumper
- [x] Data loader
- [x] CLI executable for dumping
- [x] Some image functions
- [x] Compress option
