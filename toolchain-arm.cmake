set(CMAKE_SYSTEM_NAME Generic)

set(DEVKITPRO_PATH $ENV{DEVKITPRO})
set(DEVKITARM_PATH $ENV{DEVKITARM})

include_directories("${DEVKITPRO_PATH}/libnds/include")

set(CMAKE_C_COMPILER "${DEVKITARM_PATH}/bin/arm-none-eabi-gcc")
set(CMAKE_CXX_COMPILER "${DEVKITARM_PATH}/bin/arm-none-eabi-g++")
set(CMAKE_ASM_COMPILER "${DEVKITARM_PATH}/bin/arm-none-eabi-as")

set(CMAKE_SYSROOT "${DEVKITARM_PATH}")
set(CMAKE_FIND_ROOT_PATH "${DEVKITARM_PATH}")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
