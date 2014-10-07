# Copyright 2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

########################################################################
# Project setup
########################################################################
cmake_minimum_required(VERSION 2.6)
project(gr-osmosdr CXX C)
enable_testing()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)

#select the release build type by default to get optimization flags
if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE "Release")
   message(STATUS "Build type not specified: defaulting to release.")
endif(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "")

# Set the version information here
set(VERSION_INFO_MAJOR_VERSION 0)
set(VERSION_INFO_API_COMPAT    1)
set(VERSION_INFO_MINOR_VERSION 4)
set(VERSION_INFO_MAINT_VERSION git)
include(GrVersion) #setup version info

########################################################################
# Compiler specific setup
########################################################################

IF(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64|x86")
 SET(USE_SIMD "SSE2" CACHE STRING "Use SIMD instructions")
ELSE()
 SET(USE_SIMD "no" CACHE STRING "Use SIMD instructions")
ENDIF()
 SET(USE_SIMD_VALUES "no" "SSE2" "AVX")
 SET_PROPERTY(CACHE USE_SIMD PROPERTY STRINGS ${USE_SIMD_VALUES})
 LIST(FIND USE_SIMD_VALUES ${USE_SIMD} USE_SIMD_INDEX)
 IF(${USE_SIMD_INDEX} EQUAL -1)
   message(FATAL_ERROR "Option ${USE_SIMD} not supported, valid entries are ${USE_SIMD_VALUES}")
 ENDIF()

IF(CMAKE_CXX_COMPILER MATCHES ".*clang")
    SET(CMAKE_COMPILER_IS_CLANGXX 1)
ENDIF()

IF(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANGXX)
    ADD_DEFINITIONS(-Wall)
    ADD_DEFINITIONS(-Wextra)
    ADD_DEFINITIONS(-Wno-unused-parameter)
    ADD_DEFINITIONS(-Wsign-compare)
    #ADD_DEFINITIONS(-Wconversion)
    #ADD_DEFINITIONS(-pedantic)
    #ADD_DEFINITIONS(-ansi)
    IF(NOT WIN32)
        #only export symbols that are declared to be part of the api (non dll platforms)
        ADD_DEFINITIONS(-fvisibility=hidden)
        ADD_DEFINITIONS(-fvisibility-inlines-hidden)
    ENDIF(NOT WIN32)
    IF(USE_SIMD MATCHES SSE2)
        ADD_DEFINITIONS(-msse2)
        ADD_DEFINITIONS(-DUSE_SSE2)
    ENDIF()
    IF(USE_SIMD MATCHES AVX)
        ADD_DEFINITIONS(-march=native)
        ADD_DEFINITIONS(-DUSE_AVX)
    ENDIF()
ELSEIF(MSVC)
    IF(USE_SIMD MATCHES SSE2)
        ADD_DEFINITIONS(/arch:SSE2)
        ADD_DEFINITIONS(-DUSE_SSE2)
    ENDIF()
    IF(USE_SIMD MATCHES AVX)
        ADD_DEFINITIONS(/arch:AVX)
        ADD_DEFINITIONS(-DUSE_AVX)
    ENDIF()
ENDIF()

########################################################################
# Setup boost
########################################################################
MESSAGE(STATUS "Configuring Boost C++ Libraries...")

# Although not required on my system, some users have linking issues without
SET(BOOST_REQUIRED_COMPONENTS
    thread
    system
)

if(UNIX AND NOT BOOST_ROOT AND EXISTS "/usr/lib64")
    list(APPEND BOOST_LIBRARYDIR "/usr/lib64") #fedora 64-bit fix
endif(UNIX AND NOT BOOST_ROOT AND EXISTS "/usr/lib64")

set(Boost_ADDITIONAL_VERSIONS
    "1.35.0" "1.35" "1.36.0" "1.36" "1.37.0" "1.37" "1.38.0" "1.38" "1.39.0" "1.39"
    "1.40.0" "1.40" "1.41.0" "1.41" "1.42.0" "1.42" "1.43.0" "1.43" "1.44.0" "1.44"
    "1.45.0" "1.45" "1.46.0" "1.46" "1.47.0" "1.47" "1.48.0" "1.48" "1.49.0" "1.49"
    "1.50.0" "1.50" "1.51.0" "1.51" "1.52.0" "1.52" "1.53.0" "1.53" "1.54.0" "1.54"
    "1.55.0" "1.55" "1.56.0" "1.56" "1.57.0" "1.57" "1.58.0" "1.58" "1.59.0" "1.59"
    "1.60.0" "1.60" "1.61.0" "1.61" "1.62.0" "1.62" "1.63.0" "1.63" "1.64.0" "1.64"
    "1.65.0" "1.65" "1.66.0" "1.66" "1.67.0" "1.67" "1.68.0" "1.68" "1.69.0" "1.69"
)

find_package(Boost COMPONENTS ${BOOST_REQUIRED_COMPONENTS})

if(NOT Boost_FOUND)
    message(FATAL_ERROR "Boost required to build " ${CMAKE_PROJECT_NAME})
endif()

ADD_DEFINITIONS(-DBOOST_ALL_DYN_LINK)

########################################################################
# Install directories
########################################################################
include(GrPlatform) #define LIB_SUFFIX
set(GR_RUNTIME_DIR      bin)
set(GR_LIBRARY_DIR      lib${LIB_SUFFIX})
set(GR_INCLUDE_DIR      include)
set(GR_DATA_DIR         share)
set(GR_PKG_DATA_DIR     ${GR_DATA_DIR}/${CMAKE_PROJECT_NAME})
set(GR_DOC_DIR          ${GR_DATA_DIR}/doc)
if (NOT GR_PKG_DOC_DIR)
    set(GR_PKG_DOC_DIR      ${GR_DOC_DIR}/${CMAKE_PROJECT_NAME})
endif()
set(GR_CONF_DIR         etc)
set(GR_PKG_CONF_DIR     ${GR_CONF_DIR}/${CMAKE_PROJECT_NAME}/conf.d)
set(GR_LIBEXEC_DIR      libexec)
set(GR_PKG_LIBEXEC_DIR  ${GR_LIBEXEC_DIR}/${CMAKE_PROJECT_NAME})
set(GRC_BLOCKS_DIR      ${GR_PKG_DATA_DIR}/grc/blocks)

########################################################################
# Find build dependencies
########################################################################
set(GR_REQUIRED_COMPONENTS RUNTIME PMT BLOCKS)
find_package(Gnuradio 3.7.3 REQUIRED)
find_package(GnuradioIQBalance)
find_package(UHD)
find_package(GnuradioUHD)
find_package(GnuradioFCD)
find_package(GnuradioFCDPP)
find_package(LibOsmoSDR)
find_package(LibRTLSDR)
find_package(LibMiriSDR)
find_package(LibHackRF)
find_package(LibAIRSPY)
find_package(LibbladeRF)
find_package(Doxygen)

if(NOT GNURADIO_RUNTIME_FOUND)
    message(FATAL_ERROR "GnuRadio Runtime required to build " ${CMAKE_PROJECT_NAME})
endif()

########################################################################
# Setup the include and linker paths
########################################################################
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/lib
    ${Boost_INCLUDE_DIRS}
    ${GNURADIO_ALL_INCLUDE_DIRS}
)

link_directories(
    ${Boost_LIBRARY_DIRS}
    ${GNURADIO_ALL_LIBRARY_DIRS}
)

# Set component parameters
set(GR_OSMOSDR_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include CACHE INTERNAL "" FORCE)
set(GR_OSMOSDR_SWIG_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/swig CACHE INTERNAL "" FORCE)

########################################################################
# Create uninstall target
########################################################################
configure_file(
    ${CMAKE_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
@ONLY)

add_custom_target(uninstall
    ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
)

########################################################################
# Enable python component
########################################################################
find_package(PythonLibs 2)
find_package(SWIG)

if(SWIG_FOUND)
    message(STATUS "Minimum SWIG version required is 1.3.31")
    set(SWIG_VERSION_CHECK FALSE)
    if("${SWIG_VERSION}" VERSION_GREATER "1.3.30")
        set(SWIG_VERSION_CHECK TRUE)
    endif()
endif(SWIG_FOUND)

include(GrComponent)
GR_REGISTER_COMPONENT("Python support" ENABLE_PYTHON
    PYTHONLIBS_FOUND
    SWIG_FOUND
    SWIG_VERSION_CHECK
)

########################################################################
# Add subdirectories
########################################################################
add_subdirectory(include/osmosdr)
add_subdirectory(lib)
if(ENABLE_PYTHON)
    add_subdirectory(swig)
    add_subdirectory(python)
    add_subdirectory(grc)
    add_subdirectory(apps)
endif(ENABLE_PYTHON)
add_subdirectory(docs)

########################################################################
# Create Pkg Config File
########################################################################
FOREACH(inc ${Boost_INCLUDE_DIRS})
    LIST(APPEND GR_OSMOSDR_PC_CFLAGS "-I${inc}")
ENDFOREACH(inc)

FOREACH(lib ${Boost_LIBRARY_DIRS})
    LIST(APPEND GR_OSMOSDR_PC_LIBS "-L${lib}")
ENDFOREACH(lib)

# use space-separation format for the pc file
STRING(REPLACE ";" " " GR_OSMOSDR_PC_REQUIRES "${GR_OSMOSDR_PC_REQUIRES}")
STRING(REPLACE ";" " " GR_OSMOSDR_PC_CFLAGS "${GR_OSMOSDR_PC_CFLAGS}")
STRING(REPLACE ";" " " GR_OSMOSDR_PC_LIBS "${GR_OSMOSDR_PC_LIBS}")

# unset these vars to avoid hard-coded paths to cross environment
IF(CMAKE_CROSSCOMPILING)
    UNSET(GR_OSMOSDR_PC_CFLAGS)
    UNSET(GR_OSMOSDR_PC_LIBS)
ENDIF(CMAKE_CROSSCOMPILING)

# fake gnuradio cpack behavior as long as we don't use it directly
set(CPACK_PACKAGE_NAME "gnuradio-osmosdr")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "GNU Radio block for various radio hardware")
set(CPACK_PACKAGE_VERSION ${VERSION})

CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-osmosdr.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-osmosdr.pc
@ONLY)

INSTALL(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-osmosdr.pc
    DESTINATION ${GR_LIBRARY_DIR}/pkgconfig
)

########################################################################
# Print Summary
########################################################################
GR_PRINT_COMPONENT_SUMMARY()
MESSAGE(STATUS "Building for version: ${VERSION} / ${LIBVER}")
MESSAGE(STATUS "Using install prefix: ${CMAKE_INSTALL_PREFIX}")
