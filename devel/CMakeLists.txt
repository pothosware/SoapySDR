########################################################################
## Feature registration
########################################################################
#include(FeatureSummary)
#include(CMakeDependentOption)
#cmake_dependent_option(ENABLE_TESTS "Enable library unit tests" ON "ENABLE_LIBRARY" OFF)
#add_feature_info(Tests ENABLE_TESTS "library unit tests")
#if (NOT ENABLE_TESTS)
#    return()
#endif()

########################################################################
# Unit tests
########################################################################

add_executable(BufferSim BufferSim.cpp)
target_compile_options(BufferSim PRIVATE -g)
target_link_libraries(BufferSim SoapySDR)

add_executable(TestFormatConvert TestFormatConvert.cpp)
target_link_libraries(TestFormatConvert SoapySDR)
#add_test(BufferSim BufferSim)
