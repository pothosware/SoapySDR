########################################################################
# Project setup
########################################################################
cmake_minimum_required(VERSION 3.3)
project(SoapySDRPython CXX)
enable_testing()

find_package(SoapySDR CONFIG REQUIRED)

########################################################################
# Find SWIG
########################################################################
find_package(SWIG)
message(STATUS "SWIG_FOUND: ${SWIG_FOUND} - ${SWIG_VERSION}")

########################################################################
# Find Python
########################################################################
list(PREPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

########################################################################
## set the swig flags
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
set(CMAKE_SWIG_FLAGS -threads -I${SoapySDR_INCLUDE_DIRS} -I${CMAKE_CURRENT_SOURCE_DIR}/.. -I${CMAKE_CURRENT_SOURCE_DIR})

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
## Feature registration
########################################################################
include(FeatureSummary)
include(CMakeDependentOption)

message(STATUS "")
message(STATUS "#############################################")
message(STATUS "## Begin configuration for Python 2 support...")
message(STATUS "#############################################")
message(STATUS "Enabling optional Python 2 bindings if possible...")
find_package(Python2 COMPONENTS Interpreter Development)
if(${Python2_FOUND})
    message(STATUS " * Interpreter: ${Python2_EXECUTABLE} (${Python2_INTERPRETER_ID})")
    message(STATUS " * Include: ${Python2_INCLUDE_DIRS}")
    message(STATUS " * Library: ${Python2_LIBRARIES}")
endif()
cmake_dependent_option(ENABLE_PYTHON2 "Enable Python2 bindings" ON "ENABLE_LIBRARY;SWIG_FOUND;Python2_FOUND" OFF)
add_feature_info(Python2 ENABLE_PYTHON2 "Python2 bindings v${Python2_VERSION}")

message(STATUS "")
message(STATUS "#############################################")
message(STATUS "## Begin configuration for Python 3 support...")
message(STATUS "#############################################")
message(STATUS "Enabling optional Python 3 bindings if possible...")
find_package(Python3 COMPONENTS Interpreter Development)
if(${Python3_FOUND})
    message(STATUS " * Interpreter: ${Python3_EXECUTABLE} (${Python3_INTERPRETER_ID})")
    message(STATUS " * Include: ${Python3_INCLUDE_DIRS}")
    message(STATUS " * Library: ${Python3_LIBRARIES}")
endif()
cmake_dependent_option(ENABLE_PYTHON3 "Enable Python3 bindings" ON "ENABLE_LIBRARY;SWIG_FOUND;Python3_FOUND" OFF)
add_feature_info(Python3 ENABLE_PYTHON3 "Python3 bindings v${Python3_VERSION}")

########################################################################
# Build and install module
########################################################################
include(UseSWIG)
set(SOAPYSDR_PYTHON_DIR ${CMAKE_CURRENT_SOURCE_DIR})

function(BUILD_PYTHON_MODULE PYTHON_VERSION)
    configure_file(
        ${SOAPYSDR_PYTHON_DIR}/SoapySDR.in.i
        ${CMAKE_CURRENT_BINARY_DIR}/SoapySDR.i
    @ONLY)
    set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/SoapySDR.i PROPERTIES CPLUSPLUS ON)

    if(${CMAKE_VERSION} VERSION_LESS "3.8")
        SWIG_ADD_MODULE(SoapySDR${PYTHON_VERSION} python ${CMAKE_CURRENT_BINARY_DIR}/SoapySDR.i)
    else()
        SWIG_ADD_LIBRARY(SoapySDR${PYTHON_VERSION} LANGUAGE python SOURCES ${CMAKE_CURRENT_BINARY_DIR}/SoapySDR.i)
    endif()

    set(python_includes ${SoapySDR_INCLUDE_DIRS})
    set(python_libraries SoapySDR Python${PYTHON_VERSION}::Module)

    set_target_properties(${SWIG_MODULE_SoapySDR${PYTHON_VERSION}_REAL_NAME} PROPERTIES OUTPUT_NAME _SoapySDR)

    if(${SWIG_VERSION} VERSION_GREATER "4.0.0")
        set_target_properties(${SWIG_MODULE_SoapySDR${PYTHON_VERSION}_REAL_NAME} PROPERTIES SWIG_COMPILE_OPTIONS -doxygen)
    endif()

    if(MINGW)
        # https://stackoverflow.com/a/50792585
        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
            target_compile_definitions(${SWIG_MODULE_SoapySDR${PYTHON_VERSION}_REAL_NAME} PRIVATE -DMS_WIN64=1)
        else()
            target_compile_definitions(${SWIG_MODULE_SoapySDR${PYTHON_VERSION}_REAL_NAME} PRIVATE -DMS_WIN32=1)
        endif()

        # As of Python 3.8, DLL dependencies are no longer searched for in the PATH, so statically link against
        # the GCC runtime so we don't have to worry about that.
        list(APPEND python_libraries -static-libgcc -static-libstdc++)
    endif()

    target_include_directories(${SWIG_MODULE_SoapySDR${PYTHON_VERSION}_REAL_NAME} PRIVATE ${python_includes})
    SWIG_LINK_LIBRARIES(SoapySDR${PYTHON_VERSION} ${python_libraries})

    set(get_python_lib ${SOAPYSDR_PYTHON_DIR}/get_python_lib.py)
    if (${PYTHON_VERSION} EQUAL 2)
        set(get_python_lib ${SOAPYSDR_PYTHON_DIR}/get_python2_lib.py)
    endif()

    execute_process(
        COMMAND ${Python${PYTHON_VERSION}_EXECUTABLE} ${get_python_lib} ${CMAKE_INSTALL_PREFIX}
        OUTPUT_STRIP_TRAILING_WHITESPACE
        OUTPUT_VARIABLE PYTHON_INSTALL_DIR)

    install(
        TARGETS ${SWIG_MODULE_SoapySDR${PYTHON_VERSION}_REAL_NAME}
        DESTINATION ${PYTHON_INSTALL_DIR}
    )

    install(
        FILES ${CMAKE_CURRENT_BINARY_DIR}/SoapySDR.py
        DESTINATION ${PYTHON_INSTALL_DIR}
    )
endfunction()

# TODO: Windows has full Python installations in different directories, so all Python
# versions install file in the same subpaths. CMake doesn't catch this for some reason,
# so should we error out if both Python versions are detected or prioritize one over the
# other with a warning?

if(ENABLE_PYTHON2)
    add_subdirectory(python2)
endif()

if(ENABLE_PYTHON3)
    add_subdirectory(python3)
endif()
