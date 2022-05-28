#
# Copyright (c) 2014-2015,2021 Nicholas Corgan (n.corgan@gmail.com)
#
# Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
# or copy at http://opensource.org/licenses/MIT)
#

########################################################################
# This file creates the following macros (example usage at end):
#
# SWIG_BUILD_JAVA_MODULE: Build a Java SWIG module and install it in
#                         the desired location.
#
# Parameters:
#  * module_name:  The module filename, minus the .i extension.
#  * package_name: The name of this module's Java package.
#
# Assumes the following variables are set:
#  * SWIG_MODULE_DIR:          The source directory that contains the .i
#                              file. This is needed so the .i file can
#                              include other local .i files.
#
#  * SWIG_JAVA_INCLUDE_DIRS: C/C++ include directories needed by the
#                            source files.
#
#  * SWIG_JAVA_LIBRARIES:    C/C++ libraries the Java module should
#                            link against.
#
#  * SWIG_JAVA_FLAGS:        Flags to pass into the SWIG command (optional).
#
# Example (mymodule1.i, mymodule2.i):
#  * In CMake:
#        SWIG_BUILD_JAVA_MODULE(mymodule1 nc.MyModule)
#        SWIG_BUILD_JAVA_MODULE(mymodule2 nc.MyModule)
#
#  * From Java:
#        import nc.MyModule.*;
########################################################################

macro(SWIG_BUILD_JAVA_MODULE module_name package_name)
    include(UseSWIG)

    set(SWIG_INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${SWIG_MODULE_DIR}
        ${SWIG_JAVA_INCLUDE_DIRS}
        ${JAVA_INCLUDE_PATH}
        ${JNI_INCLUDE_DIRS}
    )
    include_directories(${SWIG_INCLUDE_DIRS})

    set(SWIG_LIBRARIES
        ${SWIG_JAVA_LIBRARIES}
        ${JAVA_AWT_LIBRARY}
        ${JAVA_JVM_LIBRARY}
    )

    # Set flags to pass into SWIG call
    set(CMAKE_SWIG_FLAGS -module ${module_name} -package ${package_name} ${SWIG_JAVA_FLAGS})
    foreach(dir ${SWIG_INCLUDE_DIRS})
        LIST(APPEND CMAKE_SWIG_FLAGS "-I${dir}")
    endforeach()

    # Allows CMake variables to be placed in SWIG .i files
    configure_file(
        ${SWIG_MODULE_DIR}/${module_name}.i
        ${CMAKE_CURRENT_BINARY_DIR}/${module_name}.i
    @ONLY)

    # Set SWIG's C++ flag if specified by the user
    set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/${module_name}.i PROPERTIES CPLUSPLUS ON)

    # The actual CMake call for SWIG
    if(${CMAKE_VERSION} VERSION_LESS "3.8")
        SWIG_ADD_MODULE(${module_name} java ${CMAKE_CURRENT_BINARY_DIR}/${module_name}.i)
    else()
        SWIG_ADD_LIBRARY(${module_name} LANGUAGE java SOURCES ${CMAKE_CURRENT_BINARY_DIR}/${module_name}.i)
    endif()
    SWIG_LINK_LIBRARIES(${module_name} ${SWIG_LIBRARIES})
endmacro()
