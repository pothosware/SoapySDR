########################################################################
# Project setup
########################################################################
cmake_minimum_required(VERSION 3.1)
project(SoapySDRJava CXX)

find_package(SoapySDR CONFIG REQUIRED)

########################################################################
# Find SWIG
########################################################################
find_package(SWIG)
message(STATUS "SWIG_FOUND: ${SWIG_FOUND} - ${SWIG_VERSION}")

########################################################################
# Find Java
########################################################################
find_package(Java)
find_package(JNI)

########################################################################
## Feature registration
########################################################################
include(FeatureSummary)
include(CMakeDependentOption)
cmake_dependent_option(ENABLE_JAVA "Enable Java bindings" OFF "ENABLE_LIBRARY;SWIG_FOUND;JAVA_FOUND;JNI_FOUND" OFF)
add_feature_info(Java ENABLE_JAVA "Java bindings v${Java_VERSION_STRING}")
if (NOT ENABLE_JAVA)
    return()
endif()

########################################################################
# Build Module
########################################################################
include(SWIGJava)

set(SWIG_MODULE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(SWIG_JAVA_INCLUDE_DIRS ${SoapySDR_INCLUDE_DIRS})
set(SWIG_JAVA_LIBRARIES SoapySDR)
SWIG_BUILD_JAVA_MODULE(SoapySDRJava PothosWare.SoapySDR)

# Note: we're not installing anything, as this is meant to be an intermediate
# step in a Gradle build.
