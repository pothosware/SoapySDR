if(DEFINED INCLUDED_FIND_SOAPY_SDR_CMAKE)
    return()
endif()
set(INCLUDED_FIND_SOAPY_SDR_CMAKE TRUE)

########################################################################
## Configure installation variables
########################################################################
include(SoapySDRDefaults) #defines LIB_SUFFIX
set(SOAPY_SDR_ROOT ${CMAKE_INSTALL_PREFIX}) #same as install prefix

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
list(APPEND SOAPY_SDR_LIBRARIES ${SOAPY_SDR_LIBRARY})

########################################################################
## locate the includes
########################################################################
find_path(
    SOAPY_SDR_INCLUDE_DIR SoapySDR/Config.hpp
    PATHS ${SOAPY_SDR_ROOT}/include
    NO_DEFAULT_PATH
)
if(NOT SOAPY_SDR_LIBRARY)
    message(FATAL_ERROR "cannot find SoapySDR includes in ${SOAPY_SDR_ROOT}/include")
endif()
list(APPEND SOAPY_SDR_INCLUDE_DIRS ${SOAPY_SDR_INCLUDE_DIR})
