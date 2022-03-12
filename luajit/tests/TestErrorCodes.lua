-- Copyright (c) 2021 Nicholas Corgan
-- SPDX-License-Identifier: BSL-1.0

SoapySDR = require("SoapySDR")

luaunit = require("luaunit")

function testErrorCodes()
    luaunit.assertEquals(SoapySDR.Error.ToString(SoapySDR.Error.TIMEOUT), "TIMEOUT")
    luaunit.assertEquals(SoapySDR.Error.ToString(SoapySDR.Error.STREAM_ERROR), "STREAM_ERROR")
    luaunit.assertEquals(SoapySDR.Error.ToString(SoapySDR.Error.CORRUPTION), "CORRUPTION")
    luaunit.assertEquals(SoapySDR.Error.ToString(SoapySDR.Error.OVERFLOW), "OVERFLOW")
    luaunit.assertEquals(SoapySDR.Error.ToString(SoapySDR.Error.NOT_SUPPORTED), "NOT_SUPPORTED")
    luaunit.assertEquals(SoapySDR.Error.ToString(SoapySDR.Error.TIME_ERROR), "TIME_ERROR")
    luaunit.assertEquals(SoapySDR.Error.ToString(SoapySDR.Error.UNDERFLOW), "UNDERFLOW")
    luaunit.assertEquals(SoapySDR.Error.ToString(0), "UNKNOWN")
end

local runner = luaunit.LuaUnit.new()
os.exit(runner:runSuite())
