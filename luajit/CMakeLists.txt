########################################################################
# Project setup
########################################################################
cmake_minimum_required(VERSION 3.1.0)
project(SoapySDRLuaJIT)
enable_testing()

list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
find_package(SoapySDR CONFIG REQUIRED)

########################################################################
# Find LuaJIT
########################################################################
find_package(LuaJIT)

########################################################################
## Feature registration
########################################################################
include(FeatureSummary)
include(CMakeDependentOption)
cmake_dependent_option(ENABLE_LUAJIT "Enable LuaJIT bindings" ON "ENABLE_LIBRARY;LUAJIT_FOUND" OFF)
add_feature_info(LuaJIT ENABLE_LUAJIT "LuaJIT bindings")

if (NOT ENABLE_LUAJIT)
    return()
endif()

message(STATUS "LuaJIT interpreter: ${LUAJIT_INTERPRETER}")

configure_file(
    init.in.lua
    ${CMAKE_CURRENT_BINARY_DIR}/init.lua
@ONLY)

set(files
    Device.lua
    ${CMAKE_CURRENT_BINARY_DIR}/init.lua
    Lib.lua
    Logger.lua
    Time.lua
    Utility.lua)

install(
    FILES ${files}
    DESTINATION share/lua/5.1/SoapySDR)

add_subdirectory(docs)
add_subdirectory(tests)
