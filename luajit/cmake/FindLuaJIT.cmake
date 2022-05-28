find_program(LUAJIT_INTERPRETER
    NAMES luajit
)

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LUAJIT_FOUND to TRUE if
# all listed variables are TRUE
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LuaJIT REQUIRED_VARS LUAJIT_INTERPRETER)

mark_as_advanced(LUAJIT_INTERPRETER)
