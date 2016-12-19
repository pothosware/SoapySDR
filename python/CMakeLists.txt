########################################################################
# Project setup
########################################################################
cmake_minimum_required(VERSION 2.8)
project(SoapySDRPython CXX)
enable_testing()

find_package(SoapySDR NO_MODULE REQUIRED)

########################################################################
# Find SWIG
########################################################################
find_package(SWIG)
message(STATUS "SWIG_FOUND: ${SWIG_FOUND} - ${SWIG_VERSION}")

########################################################################
# Find python interp
########################################################################
find_package(PythonInterp)
message(STATUS "PYTHONINTERP_FOUND: ${PYTHONINTERP_FOUND} - ${PYTHON_VERSION_STRING}")
message(STATUS "PYTHON_EXECUTABLE: ${PYTHON_EXECUTABLE}")

#help find_package(PythonLibs) by setting Python_ADDITIONAL_VERSIONS from PYTHON_VERSION_STRING
if(PYTHONINTERP_FOUND AND DEFINED PYTHON_VERSION_STRING AND NOT DEFINED Python_ADDITIONAL_VERSIONS)
    string(SUBSTRING "${PYTHON_VERSION_STRING}" 0 3 Python_ADDITIONAL_VERSIONS)
endif()

########################################################################
# Determine install directory
########################################################################
execute_process(
    COMMAND ${PYTHON_EXECUTABLE} -c
    "from distutils.sysconfig import get_python_lib; print(get_python_lib(plat_specific=True, prefix=''))"
    OUTPUT_STRIP_TRAILING_WHITESPACE
    OUTPUT_VARIABLE PYTHON_INSTALL_DIR_SYSCONF
)
set(PYTHON_INSTALL_DIR "${PYTHON_INSTALL_DIR_SYSCONF}" CACHE STRING "python install prefix")
message(STATUS "PYTHON_INSTALL_DIR: \${prefix}/${PYTHON_INSTALL_DIR}")

########################################################################
# Find Python libs
########################################################################
set(PYTHON_CONFIG_EXECUTABLE ${PYTHON_EXECUTABLE}-config
    CACHE FILEPATH "Path to python-config executable")
if (USE_PYTHON_CONFIG AND EXISTS ${PYTHON_CONFIG_EXECUTABLE})
    execute_process(
        COMMAND ${PYTHON_CONFIG_EXECUTABLE}  --includes
        OUTPUT_STRIP_TRAILING_WHITESPACE
        OUTPUT_VARIABLE PYTHON_INCLUDE_DIRS)
    string(REGEX REPLACE "^[-I]" "" PYTHON_INCLUDE_DIRS "${PYTHON_INCLUDE_DIRS}")
    string(REGEX REPLACE "[ ]-I" " " PYTHON_INCLUDE_DIRS "${PYTHON_INCLUDE_DIRS}")
    separate_arguments(PYTHON_INCLUDE_DIRS)
    execute_process(
        COMMAND ${PYTHON_CONFIG_EXECUTABLE}  --ldflags
        OUTPUT_STRIP_TRAILING_WHITESPACE
        OUTPUT_VARIABLE PYTHON_LIBRARIES)
    set(PYTHONLIBS_VERSION_STRING ${PYTHON_VERSION_STRING})
    set(PYTHONLIBS_FOUND TRUE)
else()
    find_package(PythonLibs)
endif()

message(STATUS "PYTHONLIBS_FOUND: ${PYTHONLIBS_FOUND} - ${PYTHONLIBS_VERSION_STRING}")
message(STATUS "PYTHON_INCLUDE_DIRS: ${PYTHON_INCLUDE_DIRS}")
message(STATUS "PYTHON_LIBRARIES: ${PYTHON_LIBRARIES}")

#on windows, we require a pythonxx_d.lib in debug mode
#require that the PYTHON_DEBUG_LIBRARY flag is set
#or the build assumes that the debug library DNE
set(PYTHON_DEBUG_OK TRUE)
if(WIN32 AND NOT PYTHON_DEBUG_LIBRARY AND "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    message(WARNING "WIN32 Debug mode requires PYTHON_DEBUG_LIBRARY")
    set(PYTHON_DEBUG_OK FALSE)
endif()

########################################################################
# Python version check
########################################################################
set(PYTHON_VERSION_MATCH TRUE)
if (PYTHON_VERSION_STRING AND PYTHONLIBS_VERSION_STRING)
    if(NOT "${PYTHON_VERSION_STRING}" VERSION_EQUAL "${PYTHONLIBS_VERSION_STRING}")
        message(WARNING "Python interp and library version mismatch")
        set(PYTHON_VERSION_MATCH FALSE)
    endif()
endif()

#set once we know that executable and libs are found and match
#this tells the parent scope to build python3 when this is python2
if(PYTHON_VERSION_STRING AND "${PYTHON_VERSION_STRING}" VERSION_LESS "3.0")
    set(BUILD_PYTHON3 TRUE PARENT_SCOPE)
endif()

########################################################################
## set the swig flags - shared with python3 build
########################################################################
set(CMAKE_SWIG_FLAGS -c++ -threads)

#check for size_t issue on arm 32-bit platforms
include(CheckCXXSourceCompiles)
CHECK_CXX_SOURCE_COMPILES("
    #include <cstddef>
    int main() {
    size_t *x = (unsigned int *)(NULL);
    return 0; }" SIZE_T_IS_UNSIGNED_INT)

if (SIZE_T_IS_UNSIGNED_INT)
    list(APPEND CMAKE_SWIG_FLAGS -DSIZE_T_IS_UNSIGNED_INT)
endif (SIZE_T_IS_UNSIGNED_INT)

set(CMAKE_SWIG_FLAGS ${CMAKE_SWIG_FLAGS} PARENT_SCOPE)

########################################################################
## Feature registration
########################################################################
include(FeatureSummary)
include(CMakeDependentOption)
cmake_dependent_option(ENABLE_PYTHON "Enable python bindings" ON "ENABLE_LIBRARY;SWIG_FOUND;PYTHONINTERP_FOUND;PYTHONLIBS_FOUND;PYTHON_DEBUG_OK;PYTHON_VERSION_MATCH" OFF)
add_feature_info(Python ENABLE_PYTHON "python bindings v${PYTHON_VERSION_STRING}")
if (NOT ENABLE_PYTHON)
    return()
endif()

########################################################################
# Build Module
########################################################################
include(UseSWIG)
include_directories(${SoapySDR_INCLUDE_DIRS})
include_directories(${PYTHON_INCLUDE_DIRS})

message(STATUS "CMAKE_SWIG_FLAGS=${CMAKE_SWIG_FLAGS}")
set_source_files_properties(SoapySDR.i PROPERTIES CPLUSPLUS ON)

SWIG_ADD_MODULE(SoapySDR python SoapySDR.i)
SWIG_LINK_LIBRARIES(SoapySDR ${SoapySDR_LIBRARIES} ${PYTHON_LIBRARIES})

########################################################################
# Install Module
########################################################################
install(
    TARGETS ${SWIG_MODULE_SoapySDR_REAL_NAME}
    DESTINATION ${PYTHON_INSTALL_DIR}
)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/SoapySDR.py
    DESTINATION ${PYTHON_INSTALL_DIR}
)
