########################################################################
# Project setup
########################################################################
cmake_minimum_required(VERSION 2.8.8)
project(SoapySDR)
enable_language(CXX)
enable_testing()

########################################################################
# gather version information
# packagers may specify -DSOAPY_SDR_EXTVER="foo" to replace the git hash
########################################################################
set(SOAPY_SDR_LIBVER "0.2.0")

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
find_package(SoapySDR CONFIG REQUIRED)
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
    install(FILES ${cmake_files} DESTINATION share/cmake/${CMAKE_PROJECT_NAME})
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
add_subdirectory(docs)

########################################################################
# Python support (optional)
########################################################################
message(STATUS "")
message(STATUS "#############################################")
message(STATUS "## Begin configuration for Python support...")
message(STATUS "#############################################")
message(STATUS "Enabling optional Python bindings if possible...")
add_subdirectory(python)

########################################################################
# uninstall target
########################################################################
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

########################################################################
# in-tree modules
########################################################################
if (NOT DEFINED GPLOK)
    message(WARNING "Enabling GPL support modules by default")
    set(GPLOK TRUE CACHE BOOL "GPL support modules ok?" FORCE)
endif ()

if (GPLOK)
    if (NOT WIN32)
        string(ASCII 27 Esc)
        set(ColourReset "${Esc}[m")
        set(ColourBold  "${Esc}[1m")
        set(Yellow      "${Esc}[33m")
    endif ()
    message(STATUS
"${ColourBold}${Yellow}\n"
"\n"
"#############################################\n"
"## GPL vendor support modules are enabled.\n"
"## Disable GPL modules with -DGPLOK=FALSE\n"
"#############################################\n"
"${ColourReset}")

    if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/SoapyOsmo/CMakeLists.txt)
    message(STATUS "")
    message(STATUS "#############################################")
    message(STATUS "## Begin configuration for Osmo support...")
    message(STATUS "#############################################")
    add_subdirectory(SoapyOsmo)
    endif()

    if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/SoapyUHD/CMakeLists.txt)
    message(STATUS "")
    message(STATUS "#############################################")
    message(STATUS "## Begin configuration for UHD support...")
    message(STATUS "#############################################")
    add_subdirectory(SoapyUHD)
    endif()

endif ()

########################################################################
# summary
########################################################################
message(STATUS "")
message(STATUS "######################################################")
message(STATUS "## SoapySDR v${SOAPY_SDR_VERSION}")
message(STATUS "######################################################")
message(STATUS "Using install prefix: ${CMAKE_INSTALL_PREFIX}")
