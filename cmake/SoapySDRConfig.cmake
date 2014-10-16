if(DEFINED INCLUDED_SOAPY_SDR_CONFIG_CMAKE)
    return()
endif()
set(INCLUDED_SOAPY_SDR_CONFIG_CMAKE TRUE)

########################################################################
## Configure installation variables
########################################################################
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})
include(SoapySDRUtil) #defines LIB_SUFFIX

if (UNIX)
    get_filename_component(SOAPY_SDR_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)
elseif (WIN32)
    get_filename_component(SOAPY_SDR_ROOT "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
endif ()

########################################################################
## locate the library
########################################################################
find_library(
    SOAPY_SDR_LIBRARY SoapySDR SoapySDRd
    PATHS ${SOAPY_SDR_ROOT}/lib${LIB_SUFFIX}
    NO_DEFAULT_PATH
)
if(NOT SOAPY_SDR_LIBRARY)
    message(FATAL_ERROR "cannot find SoapySDR library in ${SOAPY_SDR_ROOT}/lib${LIB_SUFFIX}")
endif()
set(SoapySDR_LIBRARIES ${SOAPY_SDR_LIBRARY})

########################################################################
## locate the includes
########################################################################
find_path(
    SOAPY_SDR_INCLUDE_DIR SoapySDR/Config.hpp
    PATHS ${SOAPY_SDR_ROOT}/include
    NO_DEFAULT_PATH
)
if(NOT SOAPY_SDR_INCLUDE_DIR)
    message(FATAL_ERROR "cannot find SoapySDR includes in ${SOAPY_SDR_ROOT}/include")
endif()
set(SoapySDR_INCLUDE_DIRS ${SOAPY_SDR_INCLUDE_DIR})
