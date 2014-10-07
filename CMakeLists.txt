########################################################################
# Project setup
########################################################################
cmake_minimum_required(VERSION 2.8)
project(SoapySDRLibrary)
enable_language(CXX)
enable_testing()

set(SOAPY_SDR_VERSION "0.1.0")

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)
include(SoapySDRDefaults)

########################################################################
# Install cmake helper modules
########################################################################
install(
    DIRECTORY cmake/Modules
    DESTINATION share/SoapySDR/cmake
)

########################################################################
# Install headers
########################################################################
install(
    DIRECTORY include/SoapySDR
    DESTINATION include
)

########################################################################
# Build subdirs
########################################################################
add_subdirectory(lib)
add_subdirectory(apps)

########################################################################
# in-tree modules
########################################################################

#allows in-tree module util:
set(INCLUDED_FIND_SOAPY_SDR_CMAKE TRUE)
set(SOAPY_SDR_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SOAPY_SDR_LIBRARIES SoapySDR)
include_directories(${SOAPY_SDR_INCLUDE_DIRS}) #local include precedence

add_subdirectory(OsmoSupport)
add_subdirectory(UHDSupport)
