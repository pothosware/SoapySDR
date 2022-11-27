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
## Packagers can pass PROJECT_VERSION_EXTRA for additional version info.
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

    #additional version information when specified
    if (PROJECT_VERSION_EXTRA)
        if (MODULE_VERSION)
            set(MODULE_VERSION "${MODULE_VERSION}-${PROJECT_VERSION_EXTRA}")
        else()
            set(MODULE_VERSION "${PROJECT_VERSION_EXTRA}")
        endif()
    endif()

    #add git hash when possible
    if (EXISTS "${PROJECT_SOURCE_DIR}/.git")
        find_package(Git)
        if(GIT_FOUND)
            execute_process(
                COMMAND ${GIT_EXECUTABLE} -C "${PROJECT_SOURCE_DIR}" rev-parse --short HEAD
                OUTPUT_STRIP_TRAILING_WHITESPACE
                OUTPUT_VARIABLE GIT_HASH)
            if (GIT_HASH)
                if (MODULE_VERSION)
                    set(MODULE_VERSION "${MODULE_VERSION}-${GIT_HASH}")
                else()
                    set(MODULE_VERSION "${GIT_HASH}")
                endif()
            endif()
        endif(GIT_FOUND)
    endif()

    add_library(${MODULE_TARGET} MODULE ${MODULE_SOURCES})
    target_link_libraries(${MODULE_TARGET} PRIVATE ${MODULE_LIBRARIES} SoapySDR)
    set_target_properties(${MODULE_TARGET} PROPERTIES DEBUG_POSTFIX "") #same name in debug mode

    #symbols are only exported from the module explicitly
    set_property(TARGET ${MODULE_TARGET} PROPERTY C_VISIBILITY_PRESET hidden)
    set_property(TARGET ${MODULE_TARGET} PROPERTY CXX_VISIBILITY_PRESET hidden)
    set_property(TARGET ${MODULE_TARGET} PROPERTY VISIBILITY_INLINES_HIDDEN ON)

    #version specified, build into source file
    if (MODULE_VERSION)
        message(STATUS "Module ${MODULE_TARGET} configured with version: ${MODULE_VERSION}")
        set(version_file "${CMAKE_CURRENT_BINARY_DIR}/Version.cpp")
        file(WRITE "${version_file}" "#include <SoapySDR/Modules.hpp>
            static const SoapySDR::ModuleVersion register${MODULE_TARGET}Version(\"${MODULE_VERSION}\");
        ")
        target_sources(${MODULE_TARGET} PRIVATE "${version_file}")
    endif()

    if(CMAKE_COMPILER_IS_GNUCXX)
        #force a compile-time error when symbols are missing
        #otherwise modules will cause a runtime error on load
        if(APPLE)
            target_link_libraries(${MODULE_TARGET} PRIVATE "-Wl,-undefined,error")
        else()
            target_link_libraries(${MODULE_TARGET} PRIVATE "-Wl,--no-undefined")
        endif()
    endif()

    if (NOT MODULE_DESTINATION)
        set(MODULE_DESTINATION ${CMAKE_INSTALL_LIBDIR}/SoapySDR/modules${SOAPY_SDR_ABI_VERSION}/)
    endif()

    if (MODULE_PREFIX)
        set(MODULE_DESTINATION ${MODULE_PREFIX}/${MODULE_DESTINATION})
    endif()

    install(
        TARGETS ${MODULE_TARGET}
        DESTINATION ${MODULE_DESTINATION}
    )

endfunction(SOAPY_SDR_MODULE_UTIL)
