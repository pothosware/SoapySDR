########################################################################
# Optional LDoc generation for LuaJIT API
########################################################################
find_program(LDOC_EXECUTABLE NAMES ldoc)

########################################################################
## Feature registration
########################################################################
include(FeatureSummary)
include(CMakeDependentOption)
cmake_dependent_option(ENABLE_LDOC "Enable LuaJIT API documentation" ON "LDOC_EXECUTABLE" OFF)
add_feature_info(LDoc ENABLE_LDOC "LuaJIT API documentation")
if (NOT ENABLE_LDOC)
    return()
endif()

set(SOAPYSDR_LUAJIT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/..)
file(RELATIVE_PATH SOAPYSDR_LUAJIT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} ${SOAPYSDR_LUAJIT_DIRECTORY})
file(TO_NATIVE_PATH SOAPYSDR_LUAJIT_DIRECTORY ${SOAPYSDR_LUAJIT_DIRECTORY})

# Configure LDoc config file
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config.ld.in
    ${CMAKE_CURRENT_BINARY_DIR}/config.ld
@ONLY)

# TODO: is "." cross-platform?
set(LDOC_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/doc)
add_custom_command(
    OUTPUT ${LDOC_OUTPUT_DIR}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/.. ${CMAKE_CURRENT_SOURCE_DIR}/../apps ${CMAKE_CURRENT_BINARY_DIR}/config.ld
    COMMAND ${LDOC_EXECUTABLE} .
    COMMENT "Generating LuaJIT documentation with LDoc")
add_custom_target(ldoc_docs ALL DEPENDS ${LDOC_OUTPUT_DIR})
