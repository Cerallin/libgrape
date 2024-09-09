## Libgrape

A diff image packer/loader library for NDS.

### Installation & Usage

```
cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/path/to/install \
    /path/to/this-project
```

The library files are for DS projects.
The executable `/somewhere/grape` is for bundle generating, i.e., running on your machine instead of DS.
Generated bundle file contains 3 parts mostly: info header, base image and diff series.
`libgrape` provides functions to load from the bundle, but no functions for dumping.

Please use `grape --help` to see more.

### Functions

- [x] Data dumper
- [x] Data loader
- [x] CLI executable for dumping
- [x] Some image functions
- [ ] Compress options
