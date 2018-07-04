########################################################################
## Feature registration
########################################################################
include(FeatureSummary)
include(CMakeDependentOption)
cmake_dependent_option(ENABLE_APPS "Enable command line applications" ON "ENABLE_LIBRARY" OFF)
add_feature_info(Apps ENABLE_APPS "command line applications")
if (NOT ENABLE_APPS)
    return()
endif()

########################################################################
# Build utility executable
########################################################################
find_package(SoapySDR NO_MODULE REQUIRED)

set(SOAPY_SDR_UTIL_SOURCES
    SoapySDRUtil.cpp
    SoapySDRProbe.cpp
    SoapyRateTest.cpp
)
include_directories(${SoapySDR_INCLUDE_DIRS})
if (MSVC)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/msvc)
endif ()
add_executable(SoapySDRUtil ${SOAPY_SDR_UTIL_SOURCES})
target_link_libraries(SoapySDRUtil ${SoapySDR_LIBRARIES})
install(TARGETS SoapySDRUtil DESTINATION bin)

#install man pages for the application executable
install(FILES SoapySDRUtil.1 DESTINATION share/man/man1)
