# Toolchain file for Poky Fido 13.0 (Yocto Fido 1.8) builds
#
# Define POKY_SYSROOT in your environment and point it to
# the sysroot directory after a Yocto build. Typically, this
# will be $BUILD_DIR/tmp/sysroots/$MACHINE/.

if("$ENV{POKY_SYSROOT}" STREQUAL "")
    message(FATAL_ERROR "POKY_SYSROOT is not defined in your environment. This is required.")
else()
    set(POKY_SYSROOT "$ENV{POKY_SYSROOT}")
endif("$ENV{POKY_SYSROOT}" STREQUAL "")

if(NOT EXISTS ${POKY_SYSROOT})
    message(FATAL_ERROR "Location does not exist: POKY_SYSROOT=${POKY_SYSROOT}")
endif(NOT EXISTS ${POKY_SYSROOT})

set(CMAKE_SYSTEM_NAME Linux)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(CMAKE_SYSROOT "${POKY_SYSROOT}")
get_filename_component(CROSS_PREFIX "${POKY_SYSROOT}/../x86_64-linux/usr/bin/arm-poky-linux-gnueabi/arm-poky-linux-gnueabi-" ABSOLUTE)

set(CMAKE_C_COMPILER "${CROSS_PREFIX}gcc")
set(CMAKE_CXX_COMPILER "${CROSS_PREFIX}g++")
