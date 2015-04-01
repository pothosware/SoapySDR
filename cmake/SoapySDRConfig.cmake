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
if(CMAKE_COMPILER_IS_GNUCXX)
    #force a compile-time error when symbols are missing
    set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-undefined")
    set(CMAKE_MODULE_LINKER_FLAGS "-Wl,--no-undefined")

    #common warnings to help encourage good coding practices
    add_compile_options(-Wall)
    add_compile_options(-Wextra)

    #symbols are only exported from libraries/modules explicitly
    add_compile_options(-fvisibility=hidden)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility-inlines-hidden")
endif()

if(MSVC)
    add_compile_options(/wd4503) #'identifier' : decorated name length exceeded, name was truncated

    add_definitions(-DNOMINMAX) #enables std::min and std::max
endif()

########################################################################
# In-tree settings
########################################################################
if (SOAPY_SDR_IN_TREE_SOURCE_DIR)
    set(SoapySDR_INCLUDE_DIRS ${SOAPY_SDR_IN_TREE_SOURCE_DIR}/include)
    set(SoapySDR_LIBRARIES SoapySDR)
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
