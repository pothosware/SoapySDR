macro(LUA_CHECK_MODULE desc mod have)
    execute_process(
        COMMAND ${LUAJIT_INTERPRETER} -l ${mod} -e ""
        RESULT_VARIABLE ${have}
        OUTPUT_QUIET
        ERROR_QUIET
    )
    if(${have} EQUAL 0)
        message(STATUS "Checking for Lua module ${desc} - found")
        set(${have} TRUE)
    else()
        message(STATUS "Checking for Lua module ${desc} - failed")
        set(${have} FALSE)
    endif(${have} EQUAL 0)
ENDMACRO(LUA_CHECK_MODULE)

# Check for unit test framework
LUA_CHECK_MODULE("LuaUnit" "luaunit" LUAUNIT_FOUND)

########################################################################
## Feature registration
########################################################################
include(FeatureSummary)
include(CMakeDependentOption)
cmake_dependent_option(ENABLE_LUAJIT_TESTING "Enable LuaJIT unit tests" ON "ENABLE_LUAJIT;LUAUNIT_FOUND" OFF)
add_feature_info("LuaJIT tests" ENABLE_LUAJIT_TESTING "LuaJIT unit tests")

if (NOT ENABLE_LUAJIT_TESTING)
    return()
endif()

########################################################################
## Tests
########################################################################
set(tests
    TestConvertTypes
    TestDeviceAPI
    TestEnumerateDevices
    TestErrorCodes
    TestFormats
    TestLogger
    TestTimeConversion)

foreach(test ${tests})
    add_test(
        NAME LuaJIT_${test}
        COMMAND ${LUAJIT_INTERPRETER} ${CMAKE_CURRENT_SOURCE_DIR}/${test}.lua)
endforeach()
