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
cmake_dependent_option(ENABLE_DOCS "Enable doxygen documentation" ON "DOXYGEN_FOUND" OFF)
add_feature_info(Docs ENABLE_DOCS "doxygen documentation")
if (NOT ENABLE_DOCS)
    return()
endif()

#where to look for headers
set(SOAPY_SDR_INCLUDE_ROOT ${PROJECT_SOURCE_DIR}/include/SoapySDR)

#header sources used in the generate rule
file(GLOB SOAPY_INCLUDES ${SOAPY_SDR_INCLUDE_ROOT}/*.h*)

#Configure doxygen config file
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
    ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
@ONLY)

#Create doxygen generation build rules
if (DOXYGEN_FOUND)
    set(CMAKE_CURRENT_BINARY_DIR_DOXYGEN ${CMAKE_CURRENT_BINARY_DIR}/html)
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR_DOXYGEN}
        DEPENDS ${SOAPY_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        COMMENT "Generating documentation with doxygen"
    )
    add_custom_target(docs ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR_DOXYGEN})
endif()