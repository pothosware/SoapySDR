########################################################################
# Optional doxygen generation
########################################################################
find_package(Doxygen)

if(DOXYGEN_DOT_FOUND)
    set(HAVE_DOT YES)
else()
    set(HAVE_DOT NO)
endif()

########################################################################
## Feature registration
########################################################################
include(FeatureSummary)
include(CMakeDependentOption)
cmake_dependent_option(ENABLE_CSHARP_DOCS "Enable C# Doxygen documentation" ON "ENABLE_CSHARP;DOXYGEN_FOUND" OFF)
add_feature_info("C# Docs" ENABLE_CSHARP_DOCS "C# Doxygen documentation")
if (NOT ENABLE_CSHARP_DOCS)
    return()
endif()

# Configure doxygen config file
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
    ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
@ONLY)

# Create doxygen generation build rules
set(CMAKE_CURRENT_BINARY_DIR_DOXYGEN ${CMAKE_CURRENT_BINARY_DIR}/html)
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR_DOXYGEN}
    DEPENDS Pothosware.SoapySDR ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    COMMENT "Generating C# documentation with doxygen"
)
add_custom_target(docs_csharp ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR_DOXYGEN})