include(${CMAKE_SOURCE_DIR}/toolchain-arm.cmake)

add_compile_definitions(ARM9)

# Library sources
set(GRAPE_SOURCES
    bundle_load.c
    image.c
)

if(NOT DEFINED ENV{DEVKITARM})
    message(FATAL_ERROR "Environment variable DEVKITARM is not set. Please set it to the path of your devkitARM installation.")
endif()

add_library(grape_shared SHARED ${GRAPE_SOURCES})
set_target_properties(grape_shared PROPERTIES
    OUTPUT_NAME "grape"
    C_COMPILER ${CMAKE_C_COMPILER_ARM}
    CXX_COMPILER ${CMAKE_CXX_COMPILER_ARM}
    SYSROOT ${CMAKE_SYSROOT}
)

add_library(grape_static STATIC ${GRAPE_SOURCES})
set_target_properties(grape_static PROPERTIES
    OUTPUT_NAME "grape"
    C_COMPILER ${CMAKE_C_COMPILER_ARM}
    CXX_COMPILER ${CMAKE_CXX_COMPILER_ARM}
    SYSROOT ${CMAKE_SYSROOT}
)

# Installation
install(TARGETS grape_shared grape_static
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
