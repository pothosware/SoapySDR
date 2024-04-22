########################################################################
# Project setup
########################################################################
cmake_minimum_required(VERSION 3.3.0)
project(SoapySDR)
enable_language(CXX)
enable_testing()

# Enable newer CMake policies if available
if(POLICY CMP0077)
    cmake_policy(SET CMP0077 NEW) # option() honors normal variables
endif()
if(POLICY CMP0078)
    cmake_policy(SET CMP0078 NEW) # UseSWIG generates standard target names
endif()
if(POLICY CMP0086)
    cmake_policy(SET CMP0086 NEW) # UseSWIG honors SWIG_MODULE_NAME via -module flag
endif()
if(POLICY CMP0068)
    cmake_policy(SET CMP0068 NEW) # RPATH settings on macOS do not affect install_name
endif()

#C++11 is a required language feature for this project
set(CMAKE_CXX_STANDARD 11)

########################################################################
# gather version information
# packagers may specify -DSOAPY_SDR_EXTVER="foo" to replace the git hash
########################################################################
file(READ "${PROJECT_SOURCE_DIR}/Changelog.txt" changelog_txt)
string(REGEX MATCH "Release ([0-9]+\\.[0-9]+\\.[0-9]+) \\(" CHANGELOG_MATCH "${changelog_txt}")
if(NOT CHANGELOG_MATCH)
    message(FATAL_ERROR "Failed to extract version number from Changelog.txt")
endif(NOT CHANGELOG_MATCH)
set(SOAPY_SDR_LIBVER "${CMAKE_MATCH_1}")

if (NOT SOAPY_SDR_EXTVER)
    include(${PROJECT_SOURCE_DIR}/cmake/Modules/GetGitRevisionDescription.cmake)
    get_git_head_revision(GITREFSPEC GITHASH)
    if (GITHASH)
        string(SUBSTRING "${GITHASH}" 0 8 GITHASH)
        set(SOAPY_SDR_EXTVER "g${GITHASH}")
    else (GITHASH)
        set(SOAPY_SDR_EXTVER "unknown")
    endif (GITHASH)
endif()

set(SOAPY_SDR_VERSION "${SOAPY_SDR_LIBVER}-${SOAPY_SDR_EXTVER}")

#SOAPY_SDR_ROOT is compiled into the library to locate the install base.
#By default, the SOAPY_SDR_ROOT is set to the CMAKE_INSTALL_PREFIX.
#However users may overload this by specifying -DSOAPY_SDR_ROOT=<path>.
set(SOAPY_SDR_ROOT "${CMAKE_INSTALL_PREFIX}" CACHE PATH
    "Installation root for SoapySDR::getRootPath()")
file(TO_CMAKE_PATH "${SOAPY_SDR_ROOT}" SOAPY_SDR_ROOT)

#SOAPY_SDR_ROOT_ENV is the name of the environment variable
#which tells SoapySDR where to find the root installation.
#By default, the environment variable SOAPY_SDR_ROOT is used.
#Example: set -DSOAPY_SDR_ROOT_ENV=SNAP for snappy packages.
set(SOAPY_SDR_ROOT_ENV "SOAPY_SDR_ROOT" CACHE STRING
    "Environment variable for SoapySDR::getRootPath()")

########################################################################
# select the release build type by default to get optimization flags
########################################################################
if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE "Release")
   message(STATUS "Build type not specified: defaulting to release.")
endif(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "")

########################################################################
# rpath setup - http://www.cmake.org/Wiki/CMake_RPATH_handling
########################################################################
# use, i.e. don't skip the full RPATH for the build tree
option(CMAKE_SKIP_BUILD_RPATH "skip rpath build" FALSE)

# when building, don't use the install RPATH already
# (but later on when installing)
option(CMAKE_BUILD_WITH_INSTALL_RPATH "build with install rpath" FALSE)

# the RPATH to be used when installing, but only if it's not a system directory
option(CMAKE_AUTOSET_INSTALL_RPATH TRUE)
if(CMAKE_AUTOSET_INSTALL_RPATH)
LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}" isSystemDir)
IF("${isSystemDir}" STREQUAL "-1")
    SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
ENDIF("${isSystemDir}" STREQUAL "-1")
endif(CMAKE_AUTOSET_INSTALL_RPATH)

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
option(CMAKE_INSTALL_RPATH_USE_LINK_PATH "build with automatic rpath" TRUE)

if(APPLE)
    set(CMAKE_MACOSX_RPATH ON)
endif()

########################################################################
# Allows in-tree module util
########################################################################
set(SoapySDR_DIR ${PROJECT_SOURCE_DIR}/cmake/Modules)
set(SOAPY_SDR_IN_TREE_SOURCE_DIR ${PROJECT_SOURCE_DIR})
find_package(SoapySDR CONFIG REQUIRED)

########################################################################
# Install cmake helper modules
########################################################################
include(CMakePackageConfigHelpers)
if (UNIX OR MSYS OR MINGW)
    set(CMAKE_LIB_DEST ${CMAKE_INSTALL_DATADIR}/cmake/${PROJECT_NAME})
elseif (WIN32)
    set(CMAKE_LIB_DEST cmake)
endif ()

write_basic_package_version_file(
    ${PROJECT_BINARY_DIR}/SoapySDRConfigVersion.cmake
    VERSION ${SOAPY_SDR_VERSION}
    COMPATIBILITY AnyNewerVersion)

install(FILES
    ${PROJECT_SOURCE_DIR}/cmake/Modules/SoapySDRConfig.cmake
    ${PROJECT_SOURCE_DIR}/cmake/Modules/SoapySDRUtil.cmake
    ${PROJECT_BINARY_DIR}/SoapySDRConfigVersion.cmake
    DESTINATION ${CMAKE_LIB_DEST})

########################################################################
# Install headers
########################################################################
install(DIRECTORY include/SoapySDR DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

########################################################################
# Build subdirs
########################################################################
add_subdirectory(lib)
add_subdirectory(apps)
add_subdirectory(tests)
add_subdirectory(docs)

########################################################################
# SWIG wrappers (optional)
########################################################################
add_subdirectory(swig)

########################################################################
# LuaJIT support (optional)
########################################################################
message(STATUS "")
message(STATUS "#############################################")
message(STATUS "## Begin configuration for LuaJIT support...")
message(STATUS "#############################################")
message(STATUS "Enabling optional LuaJIT bindings if possible...")
add_subdirectory(luajit)

########################################################################
# uninstall target
########################################################################
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

#only add uninstall target if this is the top project
if(${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME})
add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif()

#########################################################################
# summary
#########################################################################
include(FeatureSummary)
message(STATUS "")
message(STATUS "######################################################")
message(STATUS "## ${PROJECT_NAME} enabled features")
message(STATUS "######################################################")
feature_summary(WHAT ENABLED_FEATURES)
message(STATUS "######################################################")
message(STATUS "## ${PROJECT_NAME} disabled features")
message(STATUS "######################################################")
feature_summary(WHAT DISABLED_FEATURES)
message(STATUS "SoapySDR version: v${SOAPY_SDR_VERSION}")
message(STATUS "ABI/so version: v${SOAPY_SDR_ABI_VERSION}")
message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")
