########################################################################
# Project setup
########################################################################
cmake_minimum_required(VERSION 2.8)
project(SoapySDRPython3 CXX)
enable_testing()

find_package(SoapySDR NO_MODULE REQUIRED)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

########################################################################
# Find SWIG
########################################################################
include(FindSWIG)

if (NOT SWIG_FOUND)
    message(WARNING "Python bindings require SWIG, skipping...")
    return()
endif ()
message(STATUS "SWIG_VERSION: ${SWIG_VERSION}")

include(UseSWIG)

########################################################################
# Find python interp
########################################################################
find_package(Python3Interp)
if (NOT PYTHON3INTERP_FOUND)
    message(WARNING "Python3 bindings require python3 exe, skipping...")
    return()
endif ()
message(STATUS "PYTHON3_EXECUTABLE: ${PYTHON3_EXECUTABLE}")

########################################################################
# Find Python libs
########################################################################
find_package(Python3Libs)
if (NOT PYTHON3LIBS_FOUND)
    message(WARNING "Python3 bindings require python3 dev, skipping...")
    return()
endif ()

#on windows, we require a pythonxx_d.lib in debug mode
#require that the PYTHON_DEBUG_LIBRARY flag is set
#or the build assumes that the debug library DNE
if(WIN32 AND NOT PYTHON3_DEBUG_LIBRARY AND "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    message(WARNING "Debug build: missing Python3 debug library...")
    return()
endif()

message(STATUS "PYTHON3_INCLUDE_DIRS ${PYTHON3_INCLUDE_DIRS}")
message(STATUS "PYTHON3_LIBRARIES ${PYTHON3_LIBRARIES}")

########################################################################
# Build Module
########################################################################
include_directories(${SoapySDR_INCLUDE_DIRS})
include_directories(${PYTHON3_INCLUDE_DIRS})

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/../python/SoapySDR.i
    ${CMAKE_CURRENT_BINARY_DIR}/SoapySDR.i
@ONLY)

set(CMAKE_SWIG_FLAGS -c++)
set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/SoapySDR.i PROPERTIES CPLUSPLUS ON)

SWIG_ADD_MODULE(SoapySDR3 python ${CMAKE_CURRENT_BINARY_DIR}/SoapySDR.i)
SWIG_LINK_LIBRARIES(SoapySDR3 ${SoapySDR_LIBRARIES} ${PYTHON3_LIBRARIES})

set_target_properties(${SWIG_MODULE_SoapySDR3_REAL_NAME} PROPERTIES OUTPUT_NAME _SoapySDR)

########################################################################
# Determine install directory
########################################################################
execute_process(
    COMMAND ${PYTHON3_EXECUTABLE} -c
    "from distutils.sysconfig import get_python_lib; print(get_python_lib(plat_specific=True, prefix=''))"
    OUTPUT_STRIP_TRAILING_WHITESPACE
    OUTPUT_VARIABLE PYTHON3_LIBRARY_DIR
)
message(STATUS "PYTHON3_LIBRARY_DIR: \${prefix}/${PYTHON3_LIBRARY_DIR}")

########################################################################
# Install Module
########################################################################
install(
    TARGETS ${SWIG_MODULE_SoapySDR3_REAL_NAME}
    DESTINATION ${PYTHON3_LIBRARY_DIR}
)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/SoapySDR.py
    DESTINATION ${PYTHON3_LIBRARY_DIR}
)
