########################################################################
# Project setup
########################################################################
cmake_minimum_required(VERSION 2.8)
project(SoapySDRPython CXX)
enable_testing()

find_package(SoapySDR CONFIG REQUIRED)

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
    COMMAND ${PYTHON_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/get_python_lib.py" "${CMAKE_INSTALL_PREFIX}"
    OUTPUT_STRIP_TRAILING_WHITESPACE
    OUTPUT_VARIABLE PYTHON_INSTALL_DIR_SYSCONF
)
set(PYTHON_INSTALL_DIR "${PYTHON_INSTALL_DIR_SYSCONF}" CACHE STRING "python install prefix")
message(STATUS "PYTHON_INSTALL_DIR: \${prefix}/${PYTHON_INSTALL_DIR}")

########################################################################
# Find Python libs
########################################################################
option(USE_PYTHON_CONFIG "use python-config to locate development files" TRUE)
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
    string(STRIP "${PYTHON_LIBRARIES}" PYTHON_LIBRARIES)
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

########################################################################
## set the swig flags - shared with python3 build
########################################################################
set(CMAKE_SWIG_FLAGS -c++ -threads -I${SoapySDR_INCLUDE_DIRS})

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

########################################################################
## Export variables for python3 subdirectory
########################################################################

#this directory used as a stand-alone build since the library is pulled in externally
#set ENABLE_LIBRARY for cmake_dependent_option() used in the feature setup below
if ("${PROJECT_SOURCE_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}")
    set(ENABLE_LIBRARY ${SoapySDR_FOUND})

#otherwise export enable and swig variables used in the python3 directory
else ()

    #set once we know that executable and libs are found and match
    #this tells the parent scope to build python3 when this is python2
    if(PYTHON_VERSION_STRING AND "${PYTHON_VERSION_STRING}" VERSION_LESS "3.0")
        set(BUILD_PYTHON3 TRUE PARENT_SCOPE)
    endif()

    #or enable search for python3 when this directory failed to find
    #a full set of python interpreter and devel files of any version
    if(NOT PYTHONINTERP_FOUND OR NOT PYTHONLIBS_FOUND)
        set(BUILD_PYTHON3 TRUE PARENT_SCOPE)
    endif()

    set(CMAKE_SWIG_FLAGS ${CMAKE_SWIG_FLAGS} PARENT_SCOPE)

endif ()

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

configure_file(
    SoapySDR.in.i
    ${CMAKE_CURRENT_BINARY_DIR}/SoapySDR.i
@ONLY)

message(STATUS "CMAKE_SWIG_FLAGS=${CMAKE_SWIG_FLAGS}")
set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/SoapySDR.i PROPERTIES CPLUSPLUS ON)

if(${CMAKE_VERSION} VERSION_LESS "3.8")
SWIG_ADD_MODULE(SoapySDR python ${CMAKE_CURRENT_BINARY_DIR}/SoapySDR.i)
else()
SWIG_ADD_LIBRARY(SoapySDR LANGUAGE python SOURCES ${CMAKE_CURRENT_BINARY_DIR}/SoapySDR.i)
endif()

if(APPLE)
    list(APPEND PYTHON_LIBRARIES "-undefined dynamic_lookup")
endif()

target_include_directories(${SWIG_MODULE_SoapySDR_REAL_NAME} PRIVATE ${PYTHON_INCLUDE_DIRS})
SWIG_LINK_LIBRARIES(SoapySDR SoapySDR ${PYTHON_LIBRARIES})

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
