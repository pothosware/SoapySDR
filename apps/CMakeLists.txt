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
add_executable(SoapySDRUtil
    SoapySDRUtil.cpp
    SoapySDRProbe.cpp
    SoapyRateTest.cpp
)
if (MSVC)
    target_include_directories(SoapySDRUtil PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/msvc)
endif ()
target_link_libraries(SoapySDRUtil SoapySDR)
install(TARGETS SoapySDRUtil DESTINATION ${CMAKE_INSTALL_BINDIR})

#install man pages for the application executable
install(FILES SoapySDRUtil.1 DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)
