########################################################################
# Feature setup
########################################################################
find_package(SoapySDR CONFIG REQUIRED)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

########################################################################
# Find SWIG
########################################################################
find_package(SWIG)
message(STATUS "SWIG_FOUND: ${SWIG_FOUND} - ${SWIG_VERSION}")

########################################################################
# Find python interp
########################################################################
find_package(Python3Interp)
message(STATUS "PYTHON3INTERP_FOUND: ${PYTHON3INTERP_FOUND}")
message(STATUS "PYTHON3_EXECUTABLE: ${PYTHON3_EXECUTABLE}")

########################################################################
# Determine install directory
########################################################################
execute_process(
    COMMAND ${PYTHON3_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/../python/get_python_lib.py" "${CMAKE_INSTALL_PREFIX}"
    OUTPUT_STRIP_TRAILING_WHITESPACE
    OUTPUT_VARIABLE PYTHON3_INSTALL_DIR_SYSCONF
)
set(PYTHON3_INSTALL_DIR "${PYTHON3_INSTALL_DIR_SYSCONF}" CACHE STRING "python3 install prefix")
message(STATUS "PYTHON3_INSTALL_DIR: \${prefix}/${PYTHON3_INSTALL_DIR}")

########################################################################
# Find Python libs
########################################################################
find_package(Python3Libs)
message(STATUS "PYTHON3LIBS_FOUND: ${PYTHON3LIBS_FOUND}")
message(STATUS "PYTHON3_INCLUDE_DIRS: ${PYTHON3_INCLUDE_DIRS}")
message(STATUS "PYTHON3_LIBRARIES: ${PYTHON3_LIBRARIES}")

#on windows, we require a pythonxx_d.lib in debug mode
#require that the PYTHON_DEBUG_LIBRARY flag is set
#or the build assumes that the debug library DNE
set(PYTHON3_DEBUG_OK TRUE)
if(WIN32 AND NOT PYTHON3_DEBUG_LIBRARY AND "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    message(WARNING "WIN32 Debug mode requires PYTHON3_DEBUG_LIBRARY")
    set(PYTHON3_DEBUG_OK FALSE)
endif()

########################################################################
## Feature registration
########################################################################
include(FeatureSummary)
include(CMakeDependentOption)
cmake_dependent_option(ENABLE_PYTHON3 "Enable python bindings" ON "ENABLE_LIBRARY;SWIG_FOUND;PYTHON3INTERP_FOUND;PYTHON3LIBS_FOUND;PYTHON3_DEBUG_OK;BUILD_PYTHON3" OFF)
add_feature_info(Python3 ENABLE_PYTHON3 "python3 bindings")
if (NOT ENABLE_PYTHON3)
    return()
endif()

########################################################################
# Build Module
########################################################################
include(UseSWIG)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/../python/SoapySDR.in.i
    ${CMAKE_CURRENT_BINARY_DIR}/SoapySDR.i
@ONLY)

message(STATUS "CMAKE_SWIG_FLAGS=${CMAKE_SWIG_FLAGS}")
set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/SoapySDR.i PROPERTIES CPLUSPLUS ON)

if(${CMAKE_VERSION} VERSION_LESS "3.8")
SWIG_ADD_MODULE(SoapySDR3 python ${CMAKE_CURRENT_BINARY_DIR}/SoapySDR.i)
else()
SWIG_ADD_LIBRARY(SoapySDR3 LANGUAGE python SOURCES ${CMAKE_CURRENT_BINARY_DIR}/SoapySDR.i)
endif()

if(APPLE)
    list(APPEND PYTHON3_LIBRARIES "-undefined dynamic_lookup")
endif()

target_include_directories(${SWIG_MODULE_SoapySDR3_REAL_NAME} PRIVATE ${PYTHON3_INCLUDE_DIRS})
SWIG_LINK_LIBRARIES(SoapySDR3 SoapySDR ${PYTHON3_LIBRARIES})

#Changed in version 3.15: Alternate library name (set with the OUTPUT_NAME property, for example)
#will be passed on to Python and CSharp wrapper libraries.
if(${CMAKE_VERSION} VERSION_LESS "3.15")
    set_target_properties(${SWIG_MODULE_SoapySDR3_REAL_NAME} PROPERTIES OUTPUT_NAME _SoapySDR)
else()
    set_target_properties(${SWIG_MODULE_SoapySDR3_REAL_NAME} PROPERTIES OUTPUT_NAME SoapySDR)
endif()

########################################################################
# Install Module
########################################################################
install(
    TARGETS ${SWIG_MODULE_SoapySDR3_REAL_NAME}
    DESTINATION ${PYTHON3_INSTALL_DIR}
)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/SoapySDR.py
    DESTINATION ${PYTHON3_INSTALL_DIR}
)
