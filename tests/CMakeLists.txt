########################################################################
## Feature registration
########################################################################
include(FeatureSummary)
include(CMakeDependentOption)
cmake_dependent_option(ENABLE_TESTS "Enable library unit tests" ON "ENABLE_LIBRARY" OFF)
add_feature_info(Tests ENABLE_TESTS "library unit tests")
if (NOT ENABLE_TESTS)
    return()
endif()

########################################################################
# Unit tests
########################################################################
add_executable(TestTimeConversion TestTimeConversion.cpp)
target_link_libraries(TestTimeConversion SoapySDR)
add_test(TestTimeConversion TestTimeConversion)

add_executable(TestFormatParser TestFormatParser.cpp)
target_link_libraries(TestFormatParser SoapySDR)
add_test(TestFormatParser TestFormatParser)

add_executable(TestKwargsMarkup TestKwargsMarkup.cpp)
target_link_libraries(TestKwargsMarkup SoapySDR)
add_test(TestKwargsMarkup TestKwargsMarkup)

add_executable(TestConvertTypes TestConvertTypes.cpp)
target_link_libraries(TestConvertTypes SoapySDR)
add_test(TestConvertTypes TestConvertTypes)
