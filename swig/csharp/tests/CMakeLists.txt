function(CSHARP_UNIT_TEST name)
    set(test_name CSharp_${name})
    add_executable(${test_name} ${name}.cs TestRunner.cs)
    target_link_libraries(${test_name} Pothosware.SoapySDR)
    
    set_property(TARGET ${test_name} PROPERTY COMPILE_FLAGS /unsafe)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set_property(TARGET ${test_name} PROPERTY COMPILE_DEFINITIONS _64BIT)
    endif()

    set_property(TARGET ${test_name} PROPERTY DOTNET_TARGET_FRAMEWORK_VERSION "v4.7.2")
    set_property(
        TARGET ${test_name}
        PROPERTY VS_DOTNET_REFERENCES
        "System"
        "System.Core"
        "System.Numerics"
        "System.Runtime"
        "Microsoft.CSharp")
    set_property(
        TARGET ${test_name}
        PROPERTY VS_PACKAGE_REFERENCES
        "System.Memory_4.5.4"
        "NUnit_3.12.0"
        "NUnitLite_3.12.0")

    # TODO: only works if library installed to PATH
    add_test(CSharp_${name} CSharp_${name}.exe)
endfunction()

CSHARP_UNIT_TEST(TestBuildInfo)
CSHARP_UNIT_TEST(TestDeviceAPI)
CSHARP_UNIT_TEST(TestEnumerateDevices)
CSHARP_UNIT_TEST(TestLogger)
CSHARP_UNIT_TEST(TestSoapyTypes)
CSHARP_UNIT_TEST(TestStreamingAPI)
CSHARP_UNIT_TEST(TestTimeConversion)