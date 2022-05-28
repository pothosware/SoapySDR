########################################################################
## Feature registration
########################################################################
include(FeatureSummary)
include(CMakeDependentOption)
cmake_dependent_option(ENABLE_LIBRARY "Enable runtime library" ON "TRUE" OFF)
add_feature_info(Library ENABLE_LIBRARY "runtime library v${SOAPY_SDR_VERSION}")
if (NOT ENABLE_LIBRARY)
    return()
endif()

########################################################################
# Setup the library
########################################################################
add_library(SoapySDR SHARED
    Device.cpp
    Factory.cpp
    Registry.cpp
    Types.cpp
    NullDevice.cpp
    Logger.cpp
    Errors.cpp
    Formats.cpp
    ConverterRegistry.cpp
    DefaultConverters.cpp
    #C API support sources
    TypesC.cpp
    ModulesC.cpp
    VersionC.cpp
    DeviceC.cpp
    FactoryC.cpp
    LoggerC.cpp
    TimeC.cpp
    ErrorsC.cpp
    FormatsC.cpp
    ConvertersC.cpp
)
target_link_libraries(SoapySDR PUBLIC ${SoapySDR_LINKER_FLAGS})
target_include_directories(SoapySDR PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(SoapySDR PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:include>)
target_compile_options(SoapySDR PUBLIC ${SoapySDR_COMPILE_OPTIONS})
target_compile_definitions(SoapySDR PUBLIC ${SoapySDR_COMPILE_DEFINITIONS})
set_target_properties(SoapySDR PROPERTIES SOVERSION ${SOAPY_SDR_ABI_VERSION})
set_target_properties(SoapySDR PROPERTIES VERSION ${SOAPY_SDR_LIBVER})
set_target_properties(SoapySDR PROPERTIES DEFINE_SYMBOL "SOAPY_SDR_DLL_EXPORTS")

#symbols are only exported from the library explicitly
set_property(TARGET SoapySDR PROPERTY C_VISIBILITY_PRESET hidden)
set_property(TARGET SoapySDR PROPERTY CXX_VISIBILITY_PRESET hidden)
set_property(TARGET SoapySDR PROPERTY VISIBILITY_INLINES_HIDDEN ON)

########################################################################
# compiler specifics
########################################################################

#dl libs used by dlopen in unix
if (CMAKE_DL_LIBS)
    target_link_libraries(SoapySDR PRIVATE ${CMAKE_DL_LIBS})
endif (CMAKE_DL_LIBS)

if (WIN32)
    set(MODULE_EXT "dll")
elseif (UNIX)
    set(MODULE_EXT "so")
else()
    message(FATAL_ERROR "not win32 or unix")
endif()

if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    #support threaded client code
    #notice that -pthread is not the same as -lpthread
    target_link_libraries(SoapySDR PUBLIC -pthread)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
    #support threaded client code
    #notice that -pthread is not the same as -lpthread
    target_link_libraries(SoapySDR PUBLIC -pthread)

    #common warnings to help encourage good coding practices
    target_compile_options(SoapySDR PUBLIC -Wall)
    target_compile_options(SoapySDR PUBLIC -Wextra)
    target_compile_options(SoapySDR PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-Wnon-virtual-dtor>)
endif()

if (MSVC)
    #we always want to use multiple cores for compilation
    target_compile_options(SoapySDR PUBLIC /MP)

    #disable MSVC warnings
    target_compile_definitions(SoapySDR PRIVATE -D_CRT_NONSTDC_NO_DEPRECATE) #strdup
    target_compile_definitions(SoapySDR PRIVATE -D_CRT_SECURE_NO_WARNINGS) #strncpy

    #disable MSVC warnings that come from public API
    target_compile_options(SoapySDR PUBLIC /wd4251) #disable 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
    target_compile_options(SoapySDR PUBLIC /wd4503) #'identifier' : decorated name length exceeded, name was truncated

    #projects should be cross-platform and standard stl functions should work
    target_compile_definitions(SoapySDR PUBLIC -DNOMINMAX) #enables std::min and std::max
endif ()

if(APPLE)
    #fixes issue with duplicate module registry when using application bundle
    target_link_libraries(SoapySDR PUBLIC "-flat_namespace")
endif()

if ("${CMAKE_SYSTEM_NAME}" STREQUAL "FreeBSD")
    target_compile_options(SoapySDR PUBLIC -stdlib=libc++)
endif()

########################################################################
# Configure sources
########################################################################

#get a relative CMAKE_INSTALL_LIBDIR for the the module loader logic
file(RELATIVE_PATH CMAKE_INSTALL_LIBDIR "${CMAKE_INSTALL_PREFIX}" "${CMAKE_INSTALL_FULL_LIBDIR}")

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/Modules.in.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/Modules.cpp
@ONLY)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/Version.in.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/Version.cpp
@ONLY)

target_sources(SoapySDR PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}/Modules.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/Version.cpp
)

########################################################################
# Install the library
########################################################################
install(TARGETS SoapySDR
    EXPORT SoapySDRExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} # .so file
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} # .lib file
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} # .dll file
)

#export target to SoapySDR project config
install(EXPORT SoapySDRExport DESTINATION ${CMAKE_LIB_DEST})

########################################################################
# Build pkg config file
########################################################################
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/SoapySDR.in.pc
    ${CMAKE_CURRENT_BINARY_DIR}/SoapySDR.pc
@ONLY)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/SoapySDR.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)
