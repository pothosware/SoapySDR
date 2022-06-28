########################################################################
# Native bridge code
########################################################################
add_subdirectory(native)

########################################################################
# Build Module
########################################################################
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
include(SWIGJava)

set(SWIG_MODULE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(SWIG_JAVA_INCLUDE_DIRS
    ${SoapySDR_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/native
    ${CMAKE_CURRENT_BINARY_DIR}/native
    ${CMAKE_CURRENT_SOURCE_DIR}/../..)
set(SWIG_JAVA_LIBRARIES
    SoapySDR
    SoapySDRJavaNative)
SWIG_BUILD_JAVA_MODULE(SoapySDRJava Pothosware.SoapySDR)

# Note: we're not installing anything, as this is meant to be an intermediate
# step in a Maven build.
include(UseSWIG)
