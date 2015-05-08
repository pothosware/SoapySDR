if(DEFINED INCLUDED_SOAPY_SDR_UTIL_CMAKE)
    return()
endif()
set(INCLUDED_SOAPY_SDR_UTIL_CMAKE TRUE)

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
## PREFIX - custom install prefix or unspecified for automatic
## The automatic prefix defaults to the SoapySDR install root,
## or CMAKE_INSTALL_PREFIX when the SoapySDR install root is /usr.
##
########################################################################
function(SOAPY_SDR_MODULE_UTIL)

    include(CMakeParseArguments)
    CMAKE_PARSE_ARGUMENTS(MODULE "" "TARGET" "SOURCES;LIBRARIES" ${ARGN})

    include_directories(${SoapySDR_INCLUDE_DIRS})
    add_library(${MODULE_TARGET} MODULE ${MODULE_SOURCES})
    target_link_libraries(${MODULE_TARGET} ${MODULE_LIBRARIES} ${SoapySDR_LIBRARIES})
    set_target_properties(${MODULE_TARGET} PROPERTIES DEBUG_POSTFIX "") #same name in debug mode

    #determine user-specified or automatic install prefix
    if (MODULE_PREFIX)
    elseif ("${SOAPY_SDR_ROOT}" STREQUAL "/usr")
        set(MODULE_PREFIX ${CMAKE_INSTALL_PREFIX})
    else()
        set(MODULE_PREFIX ${SOAPY_SDR_ROOT})
    endif()

    install(
        TARGETS ${MODULE_TARGET}
        DESTINATION ${MODULE_PREFIX}/lib${LIB_SUFFIX}/SoapySDR/modules/
    )

endfunction(SOAPY_SDR_MODULE_UTIL)
