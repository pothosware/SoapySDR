########################################################################
# Unit tests
########################################################################
add_executable(TestTimeConversion TestTimeConversion.cpp)
target_link_libraries(TestTimeConversion SoapySDR)
add_test(TestTimeConversion TestTimeConversion)

add_executable(TestFormatParser TestFormatParser.cpp)
target_link_libraries(TestFormatParser SoapySDR)
add_test(TestFormatParser TestFormatParser)
