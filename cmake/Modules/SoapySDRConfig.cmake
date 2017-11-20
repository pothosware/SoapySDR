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
# Automatic LIB_SUFFIX detection + configuration option
########################################################################
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(LINUX TRUE)
endif()

if(LINUX AND EXISTS "/etc/debian_version")
    set(DEBIAN TRUE)
endif()

if(LINUX AND EXISTS "/etc/redhat-release")
    set(REDHAT TRUE)
endif()

if(LINUX AND EXISTS "/etc/SuSE-release")
    set(SUSE TRUE)
endif()

if(LINUX AND EXISTS "/etc/slackware-version")
    set(SLACKWARE TRUE)
endif()

if(NOT DEFINED LIB_SUFFIX AND (REDHAT OR SUSE OR SLACKWARE) AND CMAKE_SYSTEM_PROCESSOR MATCHES "64$")
    SET(LIB_SUFFIX 64)
endif()
set(LIB_SUFFIX ${LIB_SUFFIX} CACHE STRING "lib directory suffix")

########################################################################
# Provide add_compile_options() when not available
########################################################################
if(CMAKE_VERSION VERSION_LESS "2.8.12")
    function(add_compile_options)
        add_definitions(${ARGN})
    endfunction(add_compile_options)
endif()

########################################################################
# Helpful compiler flags
########################################################################

#C++11 is a required language feature for this project
set(CMAKE_CXX_STANDARD 11)

if(CMAKE_COMPILER_IS_GNUCXX)

    #enable C++11 on older versions of cmake
    if (CMAKE_VERSION VERSION_LESS "3.1")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
    endif()

    #force a compile-time error when symbols are missing
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined")
    set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Wl,--no-undefined")

    #common warnings to help encourage good coding practices
    add_compile_options(-Wall)
    add_compile_options(-Wextra)

    #symbols are only exported from libraries/modules explicitly
    add_compile_options(-fvisibility=hidden)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility-inlines-hidden")
endif()

if(APPLE)
    #fixes issue with duplicate module registry when using application bundle
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -flat_namespace")
    set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -flat_namespace")
endif()

if(MSVC)
    #C++11 is a required language feature for this project
    if (${MSVC_VERSION} LESS 1700)
        message(FATAL_ERROR "the build requires MSVC 2012 or newer for C++11 support")
    endif()

    #we always want to use multiple cores for compilation
    add_compile_options(/MP)

    #suppress the following warnings which are commonly caused by project headers
    add_compile_options(/wd4251) #disable 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
    add_compile_options(/wd4503) #'identifier' : decorated name length exceeded, name was truncated

    #projects should be cross-platform and standard stl functions should work
    add_definitions(-DNOMINMAX) #enables std::min and std::max
endif()

if ("${CMAKE_SYSTEM_NAME}" STREQUAL "FreeBSD")
    add_compile_options(-stdlib=libc++)
endif()

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
## installation root
########################################################################
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
    PATH_SUFFIXES ${CMAKE_LIBRARY_ARCHITECTURE}
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
_SOAPY_SDR_GET_ABI_VERSION(SOAPY_SDR_ABI_VERSION ${SoapySDR_INCLUDE_DIRS})
