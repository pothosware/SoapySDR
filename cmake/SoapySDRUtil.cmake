if(DEFINED INCLUDED_SOAPY_SDR_UTIL_CMAKE)
    return()
endif()
set(INCLUDED_SOAPY_SDR_UTIL_CMAKE TRUE)

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

if(LINUX AND EXISTS "/etc/slackware-version")
    set(SLACKWARE TRUE)
endif()

if(NOT DEFINED LIB_SUFFIX AND (REDHAT OR SLACKWARE) AND CMAKE_SYSTEM_PROCESSOR MATCHES "64$")
    SET(LIB_SUFFIX 64)
endif()
set(LIB_SUFFIX ${LIB_SUFFIX} CACHE STRING "lib directory suffix")

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

########################################################################
## SOAPY_SDR_MODULE_UTIL - build and install modules for Soapy SDR
##
## This utility can handle the build and installation operations.
##
## Arguments:
##
## TARGET - the name of the module to build
##
## SOURCES - a list of c++ source files
##
## LIBRARIES - a list of libraries to link the module to
## The module will automatically link to SoapySDR library.
##
########################################################################
function(SOAPY_SDR_MODULE_UTIL)

    include(CMakeParseArguments)
    CMAKE_PARSE_ARGUMENTS(MODULE "" "TARGET" "SOURCES;LIBRARIES" ${ARGN})

    include_directories(${SoapySDR_INCLUDE_DIRS})
    add_library(${MODULE_TARGET} MODULE ${MODULE_SOURCES})
    target_link_libraries(${MODULE_TARGET} ${MODULE_LIBRARIES} ${SoapySDR_LIBRARIES})
    set_target_properties(${MODULE_TARGET} PROPERTIES DEBUG_POSTFIX "") #same name in debug mode
    install(
        TARGETS ${MODULE_TARGET}
        DESTINATION lib${LIB_SUFFIX}/SoapySDR/modules/
    )

endfunction(SOAPY_SDR_MODULE_UTIL)
