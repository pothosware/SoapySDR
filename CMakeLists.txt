########################################################################
# Project setup
########################################################################
cmake_minimum_required(VERSION 2.8)
project(SoapySDR)
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

########################################################################
# Allows in-tree module util
########################################################################
set(SoapySDR_DIR ${PROJECT_SOURCE_DIR}/cmake)
set(SOAPY_SDR_IN_TREE_SOURCE_DIR ${PROJECT_SOURCE_DIR})
include_directories(${SoapySDR_INCLUDE_DIRS}) #local include precedence
find_package(SoapySDR CONFIG REQUIRED)

########################################################################
# Install cmake helper modules
########################################################################
file(GLOB cmake_files "${PROJECT_SOURCE_DIR}/cmake/*.cmake")
if (UNIX)
    install(FILES ${cmake_files} DESTINATION share/cmake/${CMAKE_PROJECT_NAME})
elseif (WIN32)
    install(FILES ${cmake_files} DESTINATION cmake)
endif ()

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
