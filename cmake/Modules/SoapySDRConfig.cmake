if(DEFINED INCLUDED_SOAPY_SDR_CONFIG_CMAKE)
    return()
endif()
set(INCLUDED_SOAPY_SDR_CONFIG_CMAKE TRUE)

########################################################################
# SoapySDRConfig - cmake project configuration for client clibraries
#
# The following will be set after find_package(SoapySDR):
# SOAPY_SDR_MODULE_UTIL() - utility function to build modules
# SoapySDR_LIBRARIES - SoapySDR development libraries
# SoapySDR_INCLUDE_DIRS - SoapySDR development includes
# An import target named SoapySDR (preferred for new development)
########################################################################
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_CURRENT_LIST_DIR})
include(SoapySDRUtil)

########################################################################
# select the release build type by default to get optimization flags
########################################################################
if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE "Release")
   message(STATUS "Build type not specified: defaulting to release.")
endif(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "")

########################################################################
# Use GNU Install Dirs, but support LIB_SUFFIX if specified
########################################################################
include(GNUInstallDirs)
if(LIB_SUFFIX)
    set(CMAKE_INSTALL_LIBDIR "lib${LIB_SUFFIX}") #support old lib suffix
endif(LIB_SUFFIX)

########################################################################
# extract the ABI version string from the Version.h header
########################################################################
function(_SOAPY_SDR_GET_ABI_VERSION VERSION SOAPY_SDR_INCLUDE_DIR)
    file(READ "${SOAPY_SDR_INCLUDE_DIR}/SoapySDR/Version.h" version_h)
    string(REGEX MATCH "\\#define SOAPY_SDR_ABI_VERSION \"([0-9]+\\.[0-9]+(-[A-Za-z0-9]+)?)\"" SOAPY_SDR_ABI_VERSION_MATCHES "${version_h}")
    if(NOT SOAPY_SDR_ABI_VERSION_MATCHES)
        message(FATAL_ERROR "Failed to extract version number from Version.h")
    endif(NOT SOAPY_SDR_ABI_VERSION_MATCHES)
    set(${VERSION} "${CMAKE_MATCH_1}" PARENT_SCOPE)
endfunction(_SOAPY_SDR_GET_ABI_VERSION)

########################################################################
# In-tree settings
########################################################################
if (SOAPY_SDR_IN_TREE_SOURCE_DIR)
    if(NOT SOAPY_SDR_ROOT)
        set(SOAPY_SDR_ROOT ${CMAKE_INSTALL_PREFIX})
    endif(NOT SOAPY_SDR_ROOT)
    set(SoapySDR_INCLUDE_DIRS ${SOAPY_SDR_IN_TREE_SOURCE_DIR}/include)
    set(SoapySDR_LIBRARIES SoapySDR)
    _SOAPY_SDR_GET_ABI_VERSION(SOAPY_SDR_ABI_VERSION ${SoapySDR_INCLUDE_DIRS})
    return()
endif (SOAPY_SDR_IN_TREE_SOURCE_DIR)

########################################################################
## create import library target
########################################################################
include(SoapySDRExport)

#set old-style variables: used in python swig flags and misc projects
set(SOAPY_SDR_LIBRARY SoapySDR)
get_target_property(SOAPY_SDR_INCLUDE_DIR SoapySDR INTERFACE_INCLUDE_DIRECTORIES)
set(SoapySDR_INCLUDE_DIRS ${SOAPY_SDR_INCLUDE_DIR})
set(SoapySDR_LIBRARIES ${SOAPY_SDR_LIBRARY})
_SOAPY_SDR_GET_ABI_VERSION(SOAPY_SDR_ABI_VERSION ${SOAPY_SDR_INCLUDE_DIR})
