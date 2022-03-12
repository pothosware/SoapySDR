-- Copyright (c) 2021 Nicholas Corgan
-- SPDX-License-Identifier: BSL-1.0

SoapySDR = require("SoapySDR")
Utility = require("SoapySDR.Utility")

ffi = require("ffi")

luaunit = require("luaunit")

function ffiUIntToStringTest(testType)
    luaunit.assertEquals(Utility.soapySettingToString(ffi.new(testType, 0)), "0")
    luaunit.assertEquals(Utility.soapySettingToString(ffi.new(testType, 1)), "1")
end

function ffiIntToStringTest(testType)
    ffiUIntToStringTest(testType)
    luaunit.assertEquals(Utility.soapySettingToString(ffi.new(testType, -1)), "-1")
end

function ffiFloatToStringTest(testType)
    luaunit.assertEquals(Utility.soapySettingToString(ffi.new(testType, -0.5)), "-0.5")
    luaunit.assertEquals(Utility.soapySettingToString(ffi.new(testType, 0.0)), "0.0")
    luaunit.assertEquals(Utility.soapySettingToString(ffi.new(testType, 0.5)), "0.5")
end

function testSettingConversion()
    -- Bool
    luaunit.assertEquals(Utility.soapySettingToString(true), "true")
    luaunit.assertEquals(Utility.soapySettingToString(false), "false")
    luaunit.assertEquals(Utility.soapySettingToString(ffi.new("bool", true)), "true")
    luaunit.assertEquals(Utility.soapySettingToString(ffi.new("bool", false)), "false")
    luaunit.assertTrue(Utility.stringToSoapySetting(tostring(true), SoapySDR.ArgInfoType.BOOL))
    luaunit.assertFalse(Utility.stringToSoapySetting(tostring(false), SoapySDR.ArgInfoType.BOOL))
    luaunit.assertTrue(Utility.stringToSoapySetting("1", SoapySDR.ArgInfoType.BOOL))
    luaunit.assertTrue(Utility.stringToSoapySetting("A", SoapySDR.ArgInfoType.BOOL))
    luaunit.assertFalse(Utility.stringToSoapySetting("", SoapySDR.ArgInfoType.BOOL))
    luaunit.assertFalse(Utility.stringToSoapySetting("0", SoapySDR.ArgInfoType.BOOL))

    -- Int
    luaunit.assertEquals(Utility.soapySettingToString(0), "0")
    luaunit.assertEquals(Utility.soapySettingToString(-1), "-1")
    luaunit.assertEquals(Utility.soapySettingToString(1), "1")
    --ffiIntToStringTest("signed char")
    ffiIntToStringTest("short")
    ffiIntToStringTest("int")
    ffiIntToStringTest("long")
    ffiIntToStringTest("long long")
    ffiIntToStringTest("int64_t")
    --ffiUIntToStringTest("unsigned char")
    ffiUIntToStringTest("unsigned short")
    ffiUIntToStringTest("unsigned int")
    ffiUIntToStringTest("unsigned long")
    ffiUIntToStringTest("unsigned long long")
    ffiUIntToStringTest("uint64_t")
    luaunit.assertEquals(Utility.stringToSoapySetting("0", SoapySDR.ArgInfoType.INT), 0)
    luaunit.assertEquals(Utility.stringToSoapySetting("1", SoapySDR.ArgInfoType.INT), 1)
    luaunit.assertEquals(Utility.stringToSoapySetting("-1", SoapySDR.ArgInfoType.INT), -1)

    -- Float
    ffiFloatToStringTest("float")
    ffiFloatToStringTest("double")
    luaunit.assertEquals(Utility.stringToSoapySetting("0.5", SoapySDR.ArgInfoType.FLOAT), 0.5)
    luaunit.assertEquals(Utility.stringToSoapySetting("1.5", SoapySDR.ArgInfoType.FLOAT), 1.5)
    luaunit.assertEquals(Utility.stringToSoapySetting("-1.5", SoapySDR.ArgInfoType.FLOAT), -1.5)

    -- String
    luaunit.assertEquals(Utility.soapySettingToString("abc"), "abc")
    luaunit.assertEquals(Utility.stringToSoapySetting("abc", SoapySDR.ArgInfoType.STRING), "abc")
end

local runner = luaunit.LuaUnit.new()
os.exit(runner:runSuite())
