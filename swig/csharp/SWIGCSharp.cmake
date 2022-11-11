#
# Copyright (c) 2016-2018,2021 Nicholas Corgan (n.corgan@gmail.com)
#
# Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
# or copy at http://opensource.org/licenses/MIT)
#

########################################################################
# This file creates the following macros (example usage at end):
#
# SWIG_CSHARP_INIT:         Set the variables needed by later macros.
#
# SWIG_BUILD_CSHARP_MODULE: Build a C# SWIG module and install it in
#                           the desired location.
#
# Parameters:
#  * swig_filename:      The top-level .i file, minus the extension
#  * nativelib_name:     The name for the native library providing the SWIG layer
#                        (minus any prefix or suffix)
#  * csharp_module_name: The C# class associated with the module.
#  * cplusplus:          TRUE or FALSE, whether or not this is a C++ module.
#  * namespace:          The C# namespace this class will be in.
#
# Assumes the following variables are set:
#  * SWIG_MODULE_DIR:          The source directory that contains the .i
#                              file. This is needed so the .i file can
#                              include other local .i files.
#
#  * SWIG_CSHARP_INCLUDE_DIRS: C/C++ include directories needed by the
#                              source files.
#
#  * SWIG_CSHARP_LIBRARIES:    C/C++ libraries the C# module should
#                              link against.
#
#  * SWIG_CSHARP_FLAGS:        Flags to pass into the SWIG command (optional).
########################################################################

macro(SWIG_CSHARP_INIT)
    set(CSHARP_SWIG_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/assembly)
    set(CMAKE_SWIG_OUTDIR       ${CSHARP_SWIG_OUTPUT_DIRECTORY})
    file(MAKE_DIRECTORY         ${CSHARP_SWIG_OUTPUT_DIRECTORY})
    set(CSHARP_BINARY_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endmacro()

macro(SWIG_BUILD_CSHARP_MODULE swig_filename nativelib_name csharp_module_name cplusplus namespace)
    include(UseSWIG)

    set(SWIG_INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${SWIG_MODULE_DIR}
        ${SWIG_CSHARP_INCLUDE_DIRS}
        ${CMAKE_CURRENT_BINARY_DIR}
    )
    include_directories(${SWIG_INCLUDE_DIRS})
    set(SWIG_LIBRARIES ${SWIG_CSHARP_LIBRARIES})

    # Set output directory
    foreach(CMAKE_CONFIGURATION_TYPE ${CMAKE_CONFIGURATION_TYPES})
        string(TOUPPER ${CMAKE_CONFIGURATION_TYPE} CMAKE_CONFIGURATION_TYPE)
        set_target_properties(${SWIG_MODULE_${nativelib_name}_TARGET_NAME}
            PROPERTIES LIBRARY_OUTPUT_DIRECTORY_${CMAKE_CONFIGURATION_TYPE} "${CSHARP_BINARY_DIRECTORY}")
        set_target_properties(${SWIG_MODULE_${nativelib_name}_TARGET_NAME}
            PROPERTIES RUNTIME_OUTPUT_DIRECTORY_${CMAKE_CONFIGURATION_TYPE} "${CSHARP_BINARY_DIRECTORY}")
    endforeach()

    # Set flags to pass into SWIG call
    if(MSVC)
        set(dllimport_name "${nativelib_name}")
    else()
        set(dllimport_name "lib${nativelib_name}")
    endif()
    set(CMAKE_SWIG_FLAGS -module ${csharp_module_name} -dllimport ${dllimport_name} -namespace ${namespace} ${CMAKE_SWIG_GLOBAL_FLAGS} ${CMAKE_GLOBAL_FLAGS})
    foreach(dir ${SWIG_INCLUDE_DIRS})
        list(APPEND CMAKE_SWIG_FLAGS "-I${dir}")
    endforeach()

    # Allows CMake variables to be placed in SWIG .i files
    configure_file(
        ${SWIG_MODULE_DIR}/${swig_filename}.i
        ${CMAKE_CURRENT_BINARY_DIR}/${swig_filename}.i
    @ONLY)

    # Set SWIG's C++ flag if specified by the user
    if(${cplusplus})
        set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/${swig_filename}.i PROPERTIES CPLUSPLUS ON)
    endif()

    # The actual CMake call for SWIG
    set(SWIG_MODULE_RC_NAME ${csharp_module_name})
    if(${CMAKE_VERSION} VERSION_LESS "3.8")
        SWIG_ADD_MODULE(${nativelib_name} csharp ${CMAKE_CURRENT_BINARY_DIR}/${swig_filename}.i)
    else()
        SWIG_ADD_LIBRARY(${nativelib_name} LANGUAGE csharp SOURCES ${CMAKE_CURRENT_BINARY_DIR}/${swig_filename}.i)
    endif()

    list(LENGTH SWIG_LIBRARIES num_swig_libs)
    if(${num_swig_libs} GREATER 0)
        SWIG_LINK_LIBRARIES(${nativelib_name} ${SWIG_LIBRARIES})
    endif()
    
    # For the sake of testing, we'll output the DLL to the C# testing directory.
    set_target_properties(
        ${SWIG_MODULE_${nativelib_name}_REAL_NAME} PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/../tests)

    # Install files
    install(
        TARGETS ${SWIG_MODULE_${nativelib_name}_REAL_NAME}
        DESTINATION bin
        COMPONENT CSharp)
endmacro()