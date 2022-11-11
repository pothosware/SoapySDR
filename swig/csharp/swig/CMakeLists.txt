########################################################################
# Build Module
########################################################################
include(UseSWIG)
include(SWIGCSharp) # Convenient SWIG CMake stuff
SWIG_CSHARP_INIT()

configure_file(
    Versions.in.hpp
    ${CMAKE_CURRENT_BINARY_DIR}/Versions.hpp
@ONLY)

set(SWIG_MODULE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(SWIG_CSHARP_INCLUDE_DIRS
    ${SoapySDR_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/../..)
set(SWIG_CSHARP_LIBRARIES SoapySDR)

SWIG_BUILD_CSHARP_MODULE(SoapySDR SoapySDRCSharpSWIG SoapySDR TRUE Pothosware.SoapySDR)

# We need these for the assembly sources
set(CSHARP_SWIG_OUTPUT_DIRECTORY ${CSHARP_SWIG_OUTPUT_DIRECTORY} PARENT_SCOPE)
set(SWIG_TARGET_NAME ${SWIG_MODULE_SoapySDRCSharpSWIG_REAL_NAME} PARENT_SCOPE)