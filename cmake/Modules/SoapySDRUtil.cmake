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
## VERSION - specify a version string to build into this module
## When not specified, the util will fall-back to PROJECT_VERSION,
## and scanning the in-tree Changelog.txt file (if available).
##
########################################################################
function(SOAPY_SDR_MODULE_UTIL)

    include(CMakeParseArguments)
    CMAKE_PARSE_ARGUMENTS(MODULE "" "TARGET;DESTINATION;PREFIX;VERSION" "SOURCES;LIBRARIES" ${ARGN})

    #version not specified, try to use project version
    if (NOT MODULE_VERSION AND PROJECT_VERSION)
        set(MODULE_VERSION "${PROJECT_VERSION}")
    endif()

    #version not specified, try to use changelog entry
    if (NOT MODULE_VERSION AND EXISTS "${PROJECT_SOURCE_DIR}/Changelog.txt")
        file(READ "${PROJECT_SOURCE_DIR}/Changelog.txt" changelog_txt)
        string(REGEX MATCH "Release ([-\\._0-9a-zA-Z]*) \\(" CHANGELOG_MATCH "${changelog_txt}")
        if(CHANGELOG_MATCH)
            set(MODULE_VERSION "${CMAKE_MATCH_1}")
        endif(CHANGELOG_MATCH)
    endif()

    #version specified, build into source file
    if (MODULE_VERSION)
        set(version_file "${CMAKE_CURRENT_BINARY_DIR}/version.cpp")
        file(WRITE "${version_file}" "#include <SoapySDR/Modules.hpp>
            static const SoapySDR::ModuleVersion register${MODULE_TARGET}Version(\"${MODULE_VERSION}\");
        ")
        list(APPEND MODULE_SOURCES "${version_file}")
    endif()

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
