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

if (NOT SWIG_FOUND)
    message(WARNING "Python bindings require SWIG, skipping...")
    return()
endif ()
message(STATUS "SWIG_VERSION: ${SWIG_VERSION}")

include(UseSWIG)

########################################################################
# Find python interp
########################################################################
find_package(PythonInterp)
if (NOT PYTHONINTERP_FOUND)
    message(WARNING "Python bindings require python exe, skipping...")
    return()
endif ()
message(STATUS "PYTHON_EXECUTABLE: ${PYTHON_EXECUTABLE}")
message(STATUS "PYTHON_VERSION_STRING: ${PYTHON_VERSION_STRING}")

########################################################################
# Find Python libs
########################################################################
find_package(PythonLibs)
if (NOT PYTHONLIBS_FOUND)
    message(WARNING "Python bindings require python dev, skipping...")
    return()
endif ()

#on windows, we require a pythonxx_d.lib in debug mode
#require that the PYTHON_DEBUG_LIBRARY flag is set
#or the build assumes that the debug library DNE
if(WIN32 AND NOT PYTHON_DEBUG_LIBRARY AND "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    message(WARNING "Debug build: missing Python debug library...")
    return()
endif()

message(STATUS "PYTHONLIBS_VERSION_STRING ${PYTHONLIBS_VERSION_STRING}")
message(STATUS "PYTHON_INCLUDE_DIRS ${PYTHON_INCLUDE_DIRS}")
message(STATUS "PYTHON_LIBRARIES ${PYTHON_LIBRARIES}")

if (PYTHON_VERSION_STRING AND PYTHONLIBS_VERSION_STRING)
    if(NOT "${PYTHON_VERSION_STRING}" VERSION_EQUAL "${PYTHONLIBS_VERSION_STRING}")
        message(WARNING "Python interp and library version mismatch, skipping...")
        return()
    endif()
endif()

#set once we know that executable and libs are found and match
#this tells the parent scope to build python3 when this is python2
if(PYTHON_VERSION_STRING AND "${PYTHON_VERSION_STRING}" VERSION_LESS "3.0")
    set(BUILD_PYTHON3 TRUE PARENT_SCOPE)
endif()

########################################################################
# Build Module
########################################################################
include_directories(${SoapySDR_INCLUDE_DIRS})
include_directories(${PYTHON_INCLUDE_DIRS})

set(CMAKE_SWIG_FLAGS -c++)
set_source_files_properties(SoapySDR.i PROPERTIES CPLUSPLUS ON)

SWIG_ADD_MODULE(SoapySDR python SoapySDR.i)
SWIG_LINK_LIBRARIES(SoapySDR ${SoapySDR_LIBRARIES} ${PYTHON_LIBRARIES})

########################################################################
# Determine install directory
########################################################################
execute_process(
    COMMAND ${PYTHON_EXECUTABLE} -c
    "from distutils.sysconfig import get_python_lib; print(get_python_lib(plat_specific=True, prefix=''))"
    OUTPUT_STRIP_TRAILING_WHITESPACE
    OUTPUT_VARIABLE PYTHON_LIBRARY_DIR
)
message(STATUS "PYTHON_LIBRARY_DIR: \${prefix}/${PYTHON_LIBRARY_DIR}")

########################################################################
# Install Module
########################################################################
install(
    TARGETS ${SWIG_MODULE_SoapySDR_REAL_NAME}
    DESTINATION ${PYTHON_LIBRARY_DIR}
)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/SoapySDR.py
    DESTINATION ${PYTHON_LIBRARY_DIR}
)
