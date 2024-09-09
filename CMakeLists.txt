cmake_minimum_required(VERSION 3.10)
project(libgrape VERSION 0.9.0)

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

# find libgrit
find_library(GRIT_LIB grit)
find_path(GRIT_INCLUDE_DIR grit/grit.h)
if (GRIT_LIB AND GRIT_INCLUDE_DIR)
    message(STATUS "libgrit found!")
    include_directories(${GRIT_INCLUDE_DIR})
    target_link_libraries(grape PRIVATE ${GRIT_LIB})
else()
    message(FATAL_ERROR "libgrit not found!")
endif()

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

# Link CppUTest libraries
target_link_libraries(runTests CppUTest CppUTestExt ${GRIT_LIB})

# Enable tests
add_test(NAME runTests COMMAND runTests)
