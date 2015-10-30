# - Find python interpreter
# This module finds if Python interpreter is installed and determines where the
# executables are. This code sets the following variables:
#
#  PYTHONINTERP3_FOUND - Was the Python executable found
#  PYTHON3_EXECUTABLE  - path to the Python interpreter
#

#=============================================================================
# Copyright 2005-2009 Kitware, Inc.
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distributed this file outside of CMake, substitute the full
#  License text for the above reference.)

FIND_PROGRAM(PYTHON3_EXECUTABLE
  NAMES python3.2mu python3.2m python3.2u python3.2 python3.1 python3.0 python3
  PATHS
  [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\3.2\\InstallPath]
  [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\3.1\\InstallPath]
  [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\3.0\\InstallPath]
  )

FIND_PROGRAM(PYTHON3_DBG_EXECUTABLE
  NAMES python3.2dmu python3.2dm python3.2du python3.2d python3.1-dbg python3.0-dbg python3-dbg
  PATHS
  [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\3.2\\InstallPath]
  [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\3.1\\InstallPath]
  [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\3.0\\InstallPath]
  )


# handle the QUIETLY and REQUIRED arguments and set PYTHONINTERP_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Python3Interp DEFAULT_MSG PYTHON3_EXECUTABLE)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Python3InterpDbg DEFAULT_MSG PYTHON3_DBG_EXECUTABLE)

MARK_AS_ADVANCED(PYTHON3_EXECUTABLE)
MARK_AS_ADVANCED(PYTHON3_DBG_EXECUTABLE)


