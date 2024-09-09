cmake_minimum_required(VERSION 3.10)
project(libgrape VERSION 0.9.0)

include(ExternalProject)

# ExternalProject to build grit
ExternalProject_Add(grit
    PREFIX ${CMAKE_BINARY_DIR}/grit
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/grit
    CONFIGURE_COMMAND cd ${CMAKE_SOURCE_DIR}/grit && ./autogen.sh
    BUILD_COMMAND ${CMAKE_SOURCE_DIR}/grit/configure CXXFLAGS=-O2\ -flto --prefix=<INSTALL_DIR> && make -j$(nproc)
    INSTALL_COMMAND make install
    BUILD_IN_SOURCE 0
)

# Standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Version strings
set(SOFTWARE_NAME "Grape")
set(SOFTWARE_DESCRIPTION "CLI helper to generate image diff bundles")
set(BUG_REPORT_URL "https://github.com/Cerallin/libgrape/issues")
set(SOFTWARE_LONG_DESCRIPTION
    "This software is part of libgrape.\\n\\nGenerated bundle is of GIFF format, please see Github for more information."
)

find_package(Git QUIET)
if(GIT_FOUND)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_COMMIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
else()
    set(GIT_COMMIT_HASH "Release")
endif()

configure_file(
    ${CMAKE_SOURCE_DIR}/config.h.in
    ${CMAKE_BINARY_DIR}/config.h
    @ONLY
)

# Compile flags
set(CMAKE_C_FLAGS_DEBUG "-g -O0 -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall")
set(CMAKE_C_FLAGS_RELEASE "-O2 -flto")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -flto")

# Include directory
include_directories(include src ${CMAKE_BINARY_DIR})

# Library
add_subdirectory(lib)

# Executable sources, compiled locally
add_executable(grape
    src/bitmap.hpp
    src/bundle.hpp
    src/cli.c
    src/exports.cpp
    src/text.c
)
target_compile_definitions(grape PUBLIC GRAPE_DUMP)
target_include_directories(grape PRIVATE "${CMAKE_SOURCE_DIR}/grit/libgrit")
target_include_directories(grape PRIVATE "${CMAKE_SOURCE_DIR}/grit/cldib")
target_link_libraries(grape "${CMAKE_BINARY_DIR}/grit/src/grit-build/.libs/libgrit.a")
add_dependencies(grape grit)

install(DIRECTORY include/ DESTINATION include)

# Tests
enable_testing()

# CppUTest
add_subdirectory(tests/CppUTest)
add_executable(runTests
    lib/bundle_load.c
    lib/image.c
    tests/test_main.cpp
    tests/test_dump.cpp
    tests/test_load.cpp
    tests/test_image.cpp
    tests/mock_decompress.cpp
)
target_compile_definitions(runTests PUBLIC GRAPE_DUMP GRAPE_LOAD)
set(DEVKITPRO_PATH $ENV{DEVKITPRO})
target_include_directories(runTests PRIVATE "${DEVKITPRO_PATH}/libnds/include")
target_include_directories(runTests PRIVATE "${CMAKE_SOURCE_DIR}/grit/libgrit")
target_include_directories(runTests PRIVATE "${CMAKE_SOURCE_DIR}/grit/cldib")
target_link_libraries(runTests "${CMAKE_BINARY_DIR}/grit/src/grit-build/.libs/libgrit.a")
add_dependencies(runTests grit)

# Link CppUTest libraries
target_link_libraries(runTests CppUTest CppUTestExt)

# Enable tests
add_test(NAME runTests COMMAND runTests)
