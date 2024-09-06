cmake_minimum_required(VERSION 3.10)
project(libgrape VERSION 1.0)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_C_FLAGS_DEBUG "-g -O0 -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall")
set(CMAKE_C_FLAGS_RELEASE "-O2")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

include_directories(include src)

set(GRAPE_SOURCES
    src/grape/bitmap.hpp
    src/grape/bundle.hpp
    src/grape/exports.cpp
)

add_library(grape_shared SHARED ${GRAPE_SOURCES})
set_target_properties(grape_shared PROPERTIES OUTPUT_NAME "grape")

add_library(grape_static STATIC ${GRAPE_SOURCES})
set_target_properties(grape_static PROPERTIES OUTPUT_NAME "grape")

add_executable(grape src/cli.c)
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

install(TARGETS grape_shared grape_static grape
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin)

install(DIRECTORY include/ DESTINATION include)

enable_testing()
add_subdirectory(tests)
