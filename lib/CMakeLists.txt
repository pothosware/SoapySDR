########################################################################
# Soapy SDR sources and dependencies
########################################################################
list(APPEND SOAPY_SDR_SOURCES
    Device.cpp
    Factory.cpp
    Registry.cpp
    Types.cpp
    NullDevice.cpp
    Logger.cpp
)

#dl libs used by dlopen in unix
list(APPEND SOAPY_SDR_LIBRARIES
    ${CMAKE_DL_LIBS}
)

include_directories(${PROJECT_SOURCE_DIR}/include)

list(APPEND SOAPY_SDR_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/Modules.cpp)
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/Modules.in.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/Modules.cpp
@ONLY)

list(APPEND SOAPY_SDR_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/Version.cpp)
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/Version.in.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/Version.cpp
@ONLY)

#C API support sources
list(APPEND SOAPY_SDR_SOURCES
    TypesC.cpp
    ModulesC.cpp
    VersionC.cpp
    DeviceC.cpp
    FactoryC.cpp
    LoggerC.cpp
    Time.c
)

#disable MSVC warnings
if (MSVC)
    add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)
endif ()

########################################################################
# Soft-float support library
########################################################################
set(SOFT_FPU_SOURCE_DIR ${PROJECT_SOURCE_DIR}/fpu)
include_directories(${SOFT_FPU_SOURCE_DIR})

include(CheckTypeSize)
CHECK_TYPE_SIZE("long double" SIZE_OF_LONG_DOUBLE BUILTIN_TYPES_ONLY)
message(STATUS "SIZE_OF_LONG_DOUBLE: ${SIZE_OF_LONG_DOUBLE}")

if (SIZE_OF_LONG_DOUBLE EQUAL 16)
    list(APPEND SOAPY_SDR_SOURCES ${SOFT_FPU_SOURCE_DIR}/softfloat-native.c)
else()
    list(APPEND SOAPY_SDR_SOURCES ${SOFT_FPU_SOURCE_DIR}/softfloat.c)
    add_definitions(-DCONFIG_SOFTFLOAT)
endif()

########################################################################
# Build the library
########################################################################
add_library(SoapySDR SHARED ${SOAPY_SDR_SOURCES})
if (SOAPY_SDR_LIBRARIES)
    target_link_libraries(SoapySDR ${SOAPY_SDR_LIBRARIES})
endif()
set_target_properties(SoapySDR PROPERTIES VERSION ${SOAPY_SDR_LIBVER})
set_target_properties(SoapySDR PROPERTIES DEFINE_SYMBOL "SOAPY_SDR_DLL_EXPORTS")

########################################################################
# Install the library
########################################################################
install(TARGETS SoapySDR
    LIBRARY DESTINATION lib${LIB_SUFFIX} # .so file
    ARCHIVE DESTINATION lib${LIB_SUFFIX} # .lib file
    RUNTIME DESTINATION bin              # .dll file
)

########################################################################
# Build pkg config file
########################################################################
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/SoapySDR.in.pc
    ${CMAKE_CURRENT_BINARY_DIR}/SoapySDR.pc
@ONLY)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/SoapySDR.pc
    DESTINATION lib${LIB_SUFFIX}/pkgconfig
)
