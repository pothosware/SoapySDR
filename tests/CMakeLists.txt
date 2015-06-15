########################################################################
# Unit tests
########################################################################
add_executable(TestTimeConversion TestTimeConversion.cpp)
target_link_libraries(TestTimeConversion SoapySDR)
add_test(TestTimeConversion TestTimeConversion)
