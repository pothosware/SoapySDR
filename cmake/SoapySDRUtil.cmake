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
## DESTINATION - override the default install path when specified
## The default destination is a relative path (<lib>/SoapySDR/modules).
## This argument specifies an alternative relative or absolute path,
## and can be used standalone or in conjunction with PREFIX.
##
## PREFIX - override the default install prefix when specified
## The prefix modifies the destination with an absolute path
## to replace the typical CMAKE_INSTALL_PREFIX install rules.
##
########################################################################
function(SOAPY_SDR_MODULE_UTIL)

    include(CMakeParseArguments)
    CMAKE_PARSE_ARGUMENTS(MODULE "" "TARGET;DESTINATION;PREFIX" "SOURCES;LIBRARIES" ${ARGN})

    include_directories(${SoapySDR_INCLUDE_DIRS})
    add_library(${MODULE_TARGET} MODULE ${MODULE_SOURCES})
    target_link_libraries(${MODULE_TARGET} ${MODULE_LIBRARIES} ${SoapySDR_LIBRARIES})
    set_target_properties(${MODULE_TARGET} PROPERTIES DEBUG_POSTFIX "") #same name in debug mode

    if (NOT MODULE_DESTINATION)
        set(MODULE_DESTINATION lib${LIB_SUFFIX}/SoapySDR/modules${SOAPY_SDR_ABI_VERSION}/)
    endif()

    if (MODULE_PREFIX)
        set(MODULE_DESTINATION ${MODULE_PREFIX}/${MODULE_DESTINATION})
    endif()

    install(
        TARGETS ${MODULE_TARGET}
        DESTINATION ${MODULE_DESTINATION}
    )

endfunction(SOAPY_SDR_MODULE_UTIL)
