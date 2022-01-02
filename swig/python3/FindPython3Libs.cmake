# - Find python libraries
# This module finds if Python is installed and determines where the
# include files and libraries are. It also determines what the name of
# the library is. This code sets the following variables:
#
#  PYTHONLIBS3_FOUND       - have the Python libs been found
#  PYTHON3_LIBRARIES       - path to the python library
#  PYTHON3_INCLUDE_DIRS    - path to where Python.h is found
#  PYTHON3_DEBUG_LIBRARIES - path to the debug library
#

#=============================================================================
# Copyright 2001-2009 Kitware, Inc.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# * Redistributions of source code must retain the above copyright
#   notice, this list of conditions and the following disclaimer.
#
# * Redistributions in binary form must reproduce the above copyright
#   notice, this list of conditions and the following disclaimer in the
#   documentation and/or other materials provided with the distribution.
#
# * Neither the names of Kitware, Inc., the Insight Software Consortium,
#   nor the names of their contributors may be used to endorse or promote
#   products derived from this software without specific prior written
#   permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

INCLUDE(CMakeFindFrameworks)
# Is there a python3 framework? How do we search for it?
# Search for the python framework on Apple.
# CMAKE_FIND_FRAMEWORKS(Python)

FOREACH(_CURRENT_VERSION 3.5 3.4 3.3 3.2 3.1 3.0)
  IF(_CURRENT_VERSION GREATER 3.1)
      SET(_32FLAGS "m" "u" "mu" "dm" "du" "dmu" "")
  ELSE()
      SET(_32FLAGS "")
  ENDIF()
  FOREACH(_COMPILATION_FLAGS ${_32FLAGS})
      STRING(REPLACE "." "" _CURRENT_VERSION_NO_DOTS ${_CURRENT_VERSION})
      IF(WIN32)
        IF(_CURRENT_VERSION GREATER 3.1)
            FIND_LIBRARY(PYTHON3_DEBUG_LIBRARY
                NAMES python${_CURRENT_VERSION_NO_DOTS}d${_COMPILATION_FLAGS} python
              PATHS
              [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\${_CURRENT_VERSION}\\InstallPath]/libs/Debug
              [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\${_CURRENT_VERSION}\\InstallPath]/libs )
        ELSE()
            FIND_LIBRARY(PYTHON3_DEBUG_LIBRARY
                NAMES python${_CURRENT_VERSION_NO_DOTS}${_COMPILATION_FLAGS}_d python
              PATHS
              [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\${_CURRENT_VERSION}\\InstallPath]/libs/Debug
              [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\${_CURRENT_VERSION}\\InstallPath]/libs )
        ENDIF()
      ENDIF(WIN32)

      FIND_LIBRARY(PYTHON3_LIBRARY
          NAMES python${_CURRENT_VERSION_NO_DOTS}${_COMPILATION_FLAGS} python${_CURRENT_VERSION}${_COMPILATION_FLAGS}
        PATHS
          [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\${_CURRENT_VERSION}\\InstallPath]/libs
          # Avoid finding the .dll in the PATH.  We want the .lib.
          NO_SYSTEM_ENVIRONMENT_PATH
      )
      # Look for the static library in the Python config directory
      FIND_LIBRARY(PYTHON3_LIBRARY
        NAMES python${_CURRENT_VERSION_NO_DOTS}${_COMPILATION_FLAGS} python${_CURRENT_VERSION}${_COMPILATION_FLAGS}
        # Avoid finding the .dll in the PATH.  We want the .lib.
        NO_SYSTEM_ENVIRONMENT_PATH
        # This is where the static library is usually located
        PATH_SUFFIXES python${_CURRENT_VERSION}/config
      )

      IF(_CURRENT_VERSION GREATER 3.1)
          FIND_LIBRARY(PYTHON3_DEBUG_LIBRARY
              NAMES python${_CURRENT_VERSION_NO_DOTS}d${_COMPILATION_FLAGS} python${_CURRENT_VERSION}d${_COMPILATION_FLAGS}
            PATHS
              [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\${_CURRENT_VERSION}\\InstallPath]/libs
              # Avoid finding the .dll in the PATH.  We want the .lib.
              NO_SYSTEM_ENVIRONMENT_PATH
          )
          # Look for the static library in the Python config directory
          FIND_LIBRARY(PYTHON3_DEBUG_LIBRARY
            NAMES python${_CURRENT_VERSION_NO_DOTS}d${_COMPILATION_FLAGS} python${_CURRENT_VERSION}d${_COMPILATION_FLAGS}
            # Avoid finding the .dll in the PATH.  We want the .lib.
            NO_SYSTEM_ENVIRONMENT_PATH
            # This is where the static library is usually located
            PATH_SUFFIXES python${_CURRENT_VERSION}/config
          )
      ENDIF()

#  SET(PYTHON_FRAMEWORK_INCLUDES)
#  IF(Python_FRAMEWORKS AND NOT PYTHON_INCLUDE_DIR)
#    FOREACH(dir ${Python_FRAMEWORKS})
#      SET(PYTHON_FRAMEWORK_INCLUDES ${PYTHON_FRAMEWORK_INCLUDES}
#        ${dir}/Versions/${_CURRENT_VERSION}/include/python${_CURRENT_VERSION})
#    ENDFOREACH(dir)
#  ENDIF(Python_FRAMEWORKS AND NOT PYTHON_INCLUDE_DIR)

      FIND_PATH(PYTHON3_INCLUDE_DIR
        NAMES Python.h
        PATHS
          ${PYTHON_FRAMEWORK_INCLUDES}
          [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\${_CURRENT_VERSION}\\InstallPath]/include
        PATH_SUFFIXES
        python${_CURRENT_VERSION}${_COMPILATION_FLAGS}
      )

      # For backward compatibility, set PYTHON_INCLUDE_PATH, but make it internal.
      SET(PYTHON_INCLUDE_PATH "${PYTHON_INCLUDE_DIR}" CACHE INTERNAL 
        "Path to where Python.h is found (deprecated)")
  ENDFOREACH(_COMPILATION_FLAGS)
ENDFOREACH(_CURRENT_VERSION)

MARK_AS_ADVANCED(
  PYTHON3_DEBUG_LIBRARY
  PYTHON3_LIBRARY
  PYTHON3_INCLUDE_DIR
)

# We use PYTHON3_INCLUDE_DIR, PYTHON3_LIBRARY and PYTHON3_DEBUG_LIBRARY for the
# cache entries because they are meant to specify the location of a single
# library. We now set the variables listed by the documentation for this
# module.
SET(PYTHON3_INCLUDE_DIRS "${PYTHON3_INCLUDE_DIR}")
SET(PYTHON3_LIBRARIES "${PYTHON3_LIBRARY}")
SET(PYTHON3_DEBUG_LIBRARIES "${PYTHON3_DEBUG_LIBRARY}")

#when all else fails, use the python3-config executable
if (NOT PYTHON3_LIBRARIES OR NOT PYTHON3_INCLUDE_DIRS)
    find_program(PYTHON3_CONFIG_EXECUTABLE python3-config)
    if(PYTHON3_CONFIG_EXECUTABLE)
        execute_process(
            COMMAND ${PYTHON3_CONFIG_EXECUTABLE}  --includes
            OUTPUT_STRIP_TRAILING_WHITESPACE
            OUTPUT_VARIABLE PYTHON3_INCLUDE_DIRS)
        string(REGEX REPLACE "^[-I]" "" PYTHON3_INCLUDE_DIRS "${PYTHON3_INCLUDE_DIRS}")
        string(REGEX REPLACE "[ ]-I" " " PYTHON3_INCLUDE_DIRS "${PYTHON3_INCLUDE_DIRS}")
        separate_arguments(PYTHON3_INCLUDE_DIRS)
        execute_process(
            COMMAND ${PYTHON3_CONFIG_EXECUTABLE}  --ldflags
            OUTPUT_STRIP_TRAILING_WHITESPACE
            OUTPUT_VARIABLE PYTHON3_LIBRARIES)
        string(STRIP "${PYTHON3_LIBRARIES}" PYTHON3_LIBRARIES)
    endif(PYTHON3_CONFIG_EXECUTABLE)
endif()

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Python3Libs DEFAULT_MSG PYTHON3_LIBRARIES PYTHON3_INCLUDE_DIRS)


# PYTHON_ADD_MODULE(<name> src1 src2 ... srcN) is used to build modules for python.
# PYTHON_WRITE_MODULES_HEADER(<filename>) writes a header file you can include 
# in your sources to initialize the static python modules

GET_PROPERTY(_TARGET_SUPPORTS_SHARED_LIBS
  GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS)

FUNCTION(PYTHON3_ADD_MODULE _NAME )
  OPTION(PYTHON3_ENABLE_MODULE_${_NAME} "Add module ${_NAME}" TRUE)
  OPTION(PYTHON3_MODULE_${_NAME}_BUILD_SHARED
    "Add module ${_NAME} shared" ${_TARGET_SUPPORTS_SHARED_LIBS})

  # Mark these options as advanced
  MARK_AS_ADVANCED(PYTHON3_ENABLE_MODULE_${_NAME}
    PYTHON3_MODULE_${_NAME}_BUILD_SHARED)

  IF(PYTHON3_ENABLE_MODULE_${_NAME})
    IF(PYTHON3_MODULE_${_NAME}_BUILD_SHARED)
      SET(PY_MODULE_TYPE MODULE)
    ELSE(PYTHON3_MODULE_${_NAME}_BUILD_SHARED)
      SET(PY_MODULE_TYPE STATIC)
      SET_PROPERTY(GLOBAL  APPEND  PROPERTY  PY_STATIC_MODULES_LIST ${_NAME})
    ENDIF(PYTHON3_MODULE_${_NAME}_BUILD_SHARED)

    SET_PROPERTY(GLOBAL  APPEND  PROPERTY  PY_MODULES_LIST ${_NAME})
    ADD_LIBRARY(${_NAME} ${PY_MODULE_TYPE} ${ARGN})
#    TARGET_LINK_LIBRARIES(${_NAME} ${PYTHON_LIBRARIES})

  ENDIF(PYTHON3_ENABLE_MODULE_${_NAME})
ENDFUNCTION(PYTHON3_ADD_MODULE)

FUNCTION(PYTHON3_WRITE_MODULES_HEADER _filename)

  GET_PROPERTY(PY_STATIC_MODULES_LIST  GLOBAL  PROPERTY PY_STATIC_MODULES_LIST)

  GET_FILENAME_COMPONENT(_name "${_filename}" NAME)
  STRING(REPLACE "." "_" _name "${_name}")
  STRING(TOUPPER ${_name} _nameUpper)

  SET(_filenameTmp "${_filename}.in")
  FILE(WRITE ${_filenameTmp} "/*Created by cmake, do not edit, changes will be lost*/\n")
  FILE(APPEND ${_filenameTmp} 
"#ifndef ${_nameUpper}
#define ${_nameUpper}

#include <Python.h>

#ifdef __cplusplus
extern \"C\" {
#endif /* __cplusplus */

")

  FOREACH(_currentModule ${PY_STATIC_MODULES_LIST})
    FILE(APPEND ${_filenameTmp} "extern void init${PYTHON_MODULE_PREFIX}${_currentModule}(void);\n\n")
  ENDFOREACH(_currentModule ${PY_STATIC_MODULES_LIST})

  FILE(APPEND ${_filenameTmp} 
"#ifdef __cplusplus
}
#endif /* __cplusplus */

")


  FOREACH(_currentModule ${PY_STATIC_MODULES_LIST})
    FILE(APPEND ${_filenameTmp} "int ${_name}_${_currentModule}(void) \n{\n  static char name[]=\"${PYTHON_MODULE_PREFIX}${_currentModule}\"; return PyImport_AppendInittab(name, init${PYTHON_MODULE_PREFIX}${_currentModule});\n}\n\n")
  ENDFOREACH(_currentModule ${PY_STATIC_MODULES_LIST})

  FILE(APPEND ${_filenameTmp} "void ${_name}_LoadAllPythonModules(void)\n{\n")
  FOREACH(_currentModule ${PY_STATIC_MODULES_LIST})
    FILE(APPEND ${_filenameTmp} "  ${_name}_${_currentModule}();\n")
  ENDFOREACH(_currentModule ${PY_STATIC_MODULES_LIST})
  FILE(APPEND ${_filenameTmp} "}\n\n")
  FILE(APPEND ${_filenameTmp} "#ifndef EXCLUDE_LOAD_ALL_FUNCTION\nvoid CMakeLoadAllPythonModules(void)\n{\n  ${_name}_LoadAllPythonModules();\n}\n#endif\n\n#endif\n")
  
# with CONFIGURE_FILE() cmake complains that you may not use a file created using FILE(WRITE) as input file for CONFIGURE_FILE()
  EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy_if_different "${_filenameTmp}" "${_filename}" OUTPUT_QUIET ERROR_QUIET)

ENDFUNCTION(PYTHON3_WRITE_MODULES_HEADER)

