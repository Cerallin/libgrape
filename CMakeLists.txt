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
    set(GIT_COMMIT_HASH "")
endif()

configure_file(
    ${CMAKE_SOURCE_DIR}/version.h.in
    ${CMAKE_BINARY_DIR}/version.h
    @ONLY
)

# Compile flags
set(CMAKE_C_FLAGS_DEBUG "-g -O0 -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall")
set(CMAKE_C_FLAGS_RELEASE "-O2")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

# Include directory
include_directories(include src)

# Library sources
set(GRAPE_SOURCES
    src/grape/bitmap.hpp
    src/grape/bundle.hpp
    src/grape/exports.cpp
)

add_library(grape_shared SHARED ${GRAPE_SOURCES})
set_target_properties(grape_shared PROPERTIES OUTPUT_NAME "grape")

add_library(grape_static STATIC ${GRAPE_SOURCES})
set_target_properties(grape_static PROPERTIES OUTPUT_NAME "grape")

# Executable sources
add_executable(grape
    src/cli.c
    src/text.c
)
target_include_directories(grape PRIVATE ${CMAKE_BINARY_DIR})
target_link_libraries(grape PRIVATE grape_static)

# find libgrit
find_library(GRIT_LIB grit)
find_path(GRIT_INCLUDE_DIR grit/grit.h)
if (GRIT_LIB AND GRIT_INCLUDE_DIR)
    message(STATUS "libgrit found!")
    include_directories(${GRIT_INCLUDE_DIR})
    target_link_libraries(grape_shared PRIVATE ${GRIT_LIB})
    target_link_libraries(grape_static PRIVATE ${GRIT_LIB})
else()
    message(FATAL_ERROR "libgrit not found!")
endif()

# Installation
install(TARGETS grape_shared grape_static grape
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin)

install(DIRECTORY include/ DESTINATION include)

# Tests
enable_testing()
add_subdirectory(tests)
