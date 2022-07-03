########################################################################
# There's enough native bridge code to warrant a separate library.
########################################################################
set(SoapySDRJavaNative_srcs
    BufferHandling.cpp
    Stream.cpp)

configure_file(
    Versions.in.hpp
    ${CMAKE_CURRENT_BINARY_DIR}/Versions.hpp
@ONLY)

add_library(SoapySDRJavaNative STATIC ${SoapySDRJavaNative_srcs})
target_include_directories(SoapySDRJavaNative PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${SoapySDR_INCLUDE_DIRS}
    ${JAVA_INCLUDE_PATH}
    ${JNI_INCLUDE_DIRS})
target_link_libraries(SoapySDRJavaNative PUBLIC
    ${SoapySDR_LIBRARIES}
    ${JAVA_AWT_LIBRARY}
    ${JAVA_JVM_LIBRARY})
set_target_properties(SoapySDRJavaNative
    PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
