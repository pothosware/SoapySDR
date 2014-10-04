########################################################################
# Project setup
########################################################################
cmake_minimum_required(VERSION 2.8)
project(SoapySDRLibrary)
enable_language(CXX)
enable_testing()

########################################################################
# Install headers
########################################################################
install(
    DIRECTORY include/SoapySDR
    DESTINATION include
)

########################################################################
# Build the library
########################################################################
set(SOAPY_SDR_SOURCES
    lib/Interface.cpp
    lib/Factory.cpp
    lib/Registry.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(SoapySDR SHARED ${SOAPY_SDR_SOURCES})
#target_link_libraries(SoapySDR ...)
#set_target_properties(SoapySDR PROPERTIES VERSION ${SOAPY_SDR_VERSION})
set_target_properties(SoapySDR PROPERTIES DEFINE_SYMBOL "SOAPY_SDR_DLL_EXPORTS")

########################################################################
# Install the library
########################################################################
install(TARGETS SoapySDR
    LIBRARY DESTINATION lib${LIB_SUFFIX} # .so file
    ARCHIVE DESTINATION lib${LIB_SUFFIX} # .lib file
    RUNTIME DESTINATION bin              # .dll file
)
