# - Find python interpreter
# This module finds if Python interpreter is installed and determines where the
# executables are. This code sets the following variables:
#
#  PYTHONINTERP3_FOUND - Was the Python executable found
#  PYTHON3_EXECUTABLE  - path to the Python interpreter
#

#=============================================================================
# Copyright 2005-2009 Kitware, Inc.
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

FOREACH(_CURRENT_VERSION 3.6 3.5 3.4 3.3 3.2 3.1 3.0 3)
  IF(_CURRENT_VERSION GREATER 3.1)
      SET(_32FLAGS "m" "u" "mu" "dm" "du" "dmu" "")
  ELSE()
      SET(_32FLAGS "")
  ENDIF()
  FOREACH(_COMPILATION_FLAGS ${_32FLAGS})

    FIND_PROGRAM(PYTHON3_EXECUTABLE
      NAMES python${_CURRENT_VERSION}${_COMPILATION_FLAGS}
      PATHS
      [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\${_CURRENT_VERSION}\\InstallPath]
      )

    IF(_CURRENT_VERSION GREATER 3.1)
      FIND_PROGRAM(PYTHON3_DBG_EXECUTABLE
        NAMES python${_CURRENT_VERSION}d${_COMPILATION_FLAGS}
        PATHS
        [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\${_CURRENT_VERSION}\\InstallPath]
        )
    ELSE()
      FIND_PROGRAM(PYTHON3_DBG_EXECUTABLE
        NAMES python${_CURRENT_VERSION}-dbg
        PATHS
        [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\${_CURRENT_VERSION}\\InstallPath]
        )
    ENDIF()

  ENDFOREACH(_COMPILATION_FLAGS)
ENDFOREACH(_CURRENT_VERSION)

# handle the QUIETLY and REQUIRED arguments and set PYTHONINTERP_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Python3Interp DEFAULT_MSG PYTHON3_EXECUTABLE)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Python3InterpDbg DEFAULT_MSG PYTHON3_DBG_EXECUTABLE)

MARK_AS_ADVANCED(PYTHON3_EXECUTABLE)
MARK_AS_ADVANCED(PYTHON3_DBG_EXECUTABLE)


