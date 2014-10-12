########################################################################
# Project setup
########################################################################
cmake_minimum_required(VERSION 2.8)
project(SoapySDRLibrary)
enable_language(CXX)
enable_testing()

set(SOAPY_SDR_VERSION "0.1.0")

########################################################################
# select the release build type by default to get optimization flags
########################################################################
if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE "Release")
   message(STATUS "Build type not specified: defaulting to release.")
endif(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "")

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

########################################################################
# Allows in-tree module util
########################################################################
set(SoapySDR_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include)
set(SoapySDR_LIBRARIES SoapySDR)
include_directories(${SoapySDR_INCLUDE_DIRS}) #local include precedence
include(SoapySDRUtil) #defines LIB_SUFFIX

########################################################################
# Install cmake helper modules
########################################################################
file(GLOB cmake_files "${PROJECT_SOURCE_DIR}/cmake/*.cmake")
install(FILES ${cmake_files} DESTINATION share/cmake/SoapySDR)

########################################################################
# Install headers
########################################################################
install(DIRECTORY include/SoapySDR DESTINATION include)

########################################################################
# Build subdirs
########################################################################
add_subdirectory(lib)
add_subdirectory(apps)

########################################################################
# in-tree modules
########################################################################
add_subdirectory(OsmoSupport)
add_subdirectory(UHDSupport)
