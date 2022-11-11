cmake_minimum_required(VERSION 3.3.0)
project(SoapySDRCSharpApps CSharp)

function(CSHARP_BUILD_APP name)
    set(target_name CSharp_${name})
    add_executable(${target_name} ${name}.cs)
    target_link_libraries(${target_name} Pothosware.SoapySDR)

    set_property(
        TARGET ${target_name}
        PROPERTY VS_DOTNET_REFERENCES
        "System"
        "System.Runtime")
    set_property(
        TARGET ${target_name}
        PROPERTY VS_PACKAGE_REFERENCES
        "System.CommandLine_2.0.0-beta1.21308.1")
endfunction()

CSHARP_BUILD_APP(RxSamplesToFileExample)
CSHARP_BUILD_APP(SiggenExample)