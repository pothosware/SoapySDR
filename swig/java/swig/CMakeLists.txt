########################################################################
# Build Module
########################################################################
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
include(SWIGJava)

configure_file(
    Versions.in.hpp
    ${CMAKE_CURRENT_BINARY_DIR}/Versions.hpp
@ONLY)

set(SWIG_MODULE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(SWIG_JAVA_INCLUDE_DIRS
    ${SoapySDR_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/../..)
set(SWIG_JAVA_LIBRARIES SoapySDR)
SWIG_BUILD_JAVA_MODULE(SoapySDRJava PothosWare.SoapySDR)

# Note: we're not installing anything, as this is meant to be an intermediate
# step in a Gradle build.
include(UseSWIG)
