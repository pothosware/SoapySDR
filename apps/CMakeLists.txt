########################################################################
# Build utility executable
########################################################################
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)
add_executable(SoapySDRUtil SoapySDRUtil.cpp)
target_link_libraries(SoapySDRUtil SoapySDR)
install(TARGETS SoapySDRUtil DESTINATION bin)
