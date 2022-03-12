-- Copyright (c) 2021 Nicholas Corgan
-- SPDX-License-Identifier: BSL-1.0

SoapySDR = require("SoapySDR")

ffi = require("ffi")
luaunit = require("luaunit")

--
-- Utility
--

local function hasNullDevice(devices)
    for i=1,#devices do
        luaunit.assertIsTable(devices[i])
        luaunit.assertIsString(devices[i]["driver"])
        if(devices[i]["driver"] == "null") then
            return true
        end
    end

    return false
end

--
-- Tests
--

function testEnumerateDevicesNoParam()
    -- We can't guarantee the number of devices connected to the
    -- machine, so just make sure this doesn't error out.
    SoapySDR.enumerateDevices()
end

function testEnumerateDevicesStringParam()
    local devices = SoapySDR.enumerateDevices("type=null")
    luaunit.assertTrue(#devices >= 1)
    luaunit.assertTrue(hasNullDevice(devices))
end

function testEnumerateDevicesTableParam()
    local devices = SoapySDR.enumerateDevices({type="null"})
    luaunit.assertTrue(#devices >= 1)
    luaunit.assertTrue(hasNullDevice(devices))
end

local runner = luaunit.LuaUnit.new()
os.exit(runner:runSuite())
