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

# SWIG doesn't deal well with (s)size_t and (u)intptr_t
include(CheckCXXSourceCompiles)
CHECK_CXX_SOURCE_COMPILES("
    #include <cstddef>
    int main() {
    size_t *x = (unsigned int *)(NULL);
    return 0; }" SIZE_T_IS_UNSIGNED_INT)

if (SIZE_T_IS_UNSIGNED_INT)
    list(APPEND CMAKE_SWIG_FLAGS -DSIZE_T_IS_UNSIGNED_INT)
endif (SIZE_T_IS_UNSIGNED_INT)

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
add_subdirectory(swig)
