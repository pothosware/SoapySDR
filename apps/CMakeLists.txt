########################################################################
# Build utility executable
########################################################################
set(SOAPY_SDR_UTIL_SOURCES
    SoapySDRUtil.cpp
    SoapySDRProbe.cpp
)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)
if (MSVC)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/msvc)
endif ()
add_executable(SoapySDRUtil ${SOAPY_SDR_UTIL_SOURCES})
target_link_libraries(SoapySDRUtil SoapySDR)
install(TARGETS SoapySDRUtil DESTINATION bin)
