########################################################################
# Project setup
########################################################################
cmake_minimum_required(VERSION 2.8.7)
project(SoapySDR)
enable_language(CXX)
enable_testing()

########################################################################
# gather version information
# packagers may specify -DSOAPY_SDR_EXTVER="foo" to replace the git hash
########################################################################
set(SOAPY_SDR_LIBVER "0.4.1")

if (NOT SOAPY_SDR_EXTVER)
    include(${PROJECT_SOURCE_DIR}/cmake/GetGitRevisionDescription.cmake)
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
if(NOT SOAPY_SDR_ROOT)
    file(TO_CMAKE_PATH ${CMAKE_INSTALL_PREFIX} SOAPY_SDR_ROOT)
endif(NOT SOAPY_SDR_ROOT)

########################################################################
# select the release build type by default to get optimization flags
########################################################################
if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE "Release")
   message(STATUS "Build type not specified: defaulting to release.")
endif(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "")

########################################################################
# Allows in-tree module util
########################################################################
set(SoapySDR_DIR ${PROJECT_SOURCE_DIR}/cmake)
set(SOAPY_SDR_IN_TREE_SOURCE_DIR ${PROJECT_SOURCE_DIR})
find_package(SoapySDR NO_MODULE REQUIRED)
include_directories(${SoapySDR_INCLUDE_DIRS}) #local include precedence

########################################################################
# Install cmake helper modules
########################################################################
configure_file(
    ${PROJECT_SOURCE_DIR}/cmake/SoapySDRConfigVersion.in.cmake
    ${PROJECT_BINARY_DIR}/SoapySDRConfigVersion.cmake
@ONLY)
set(cmake_files
    ${PROJECT_SOURCE_DIR}/cmake/SoapySDRConfig.cmake
    ${PROJECT_SOURCE_DIR}/cmake/SoapySDRUtil.cmake
    ${PROJECT_BINARY_DIR}/SoapySDRConfigVersion.cmake)
if (UNIX)
    install(FILES ${cmake_files} DESTINATION share/cmake/${PROJECT_NAME})
elseif (WIN32)
    install(FILES ${cmake_files} DESTINATION cmake)
endif ()

########################################################################
# Install headers
########################################################################
install(DIRECTORY include/SoapySDR DESTINATION include)

########################################################################
# Build subdirs
########################################################################
add_subdirectory(lib)
add_subdirectory(apps)
add_subdirectory(tests)
add_subdirectory(docs)

########################################################################
# Python support (optional)
########################################################################
option(ENABLE_PYTHON "enable python bindings" ON)
if (ENABLE_PYTHON)
message(STATUS "")
message(STATUS "#############################################")
message(STATUS "## Begin configuration for Python support...")
message(STATUS "#############################################")
message(STATUS "Enabling optional Python bindings if possible...")
add_subdirectory(python)

if(BUILD_PYTHON3)
message(STATUS "")
message(STATUS "#############################################")
message(STATUS "## Begin configuration for Python3 support...")
message(STATUS "#############################################")
message(STATUS "Enabling optional Python3 bindings if possible...")
add_subdirectory(python3)
endif(BUILD_PYTHON3)

endif (ENABLE_PYTHON)

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
message(STATUS "ABI/so version: v${SOAPY_SDR_SOVER}")
message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")
