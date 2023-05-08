-- Copyright (c) 2021-2022 Nicholas Corgan
-- SPDX-License-Identifier: BSL-1.0

SoapySDR = require("SoapySDR")

ffi = require("ffi")
io = require("io")
luaunit = require("luaunit")

function testLogger()
    local tmpFile = io.tmpfile()

    function logFunction(logLevel, message)
        tmpFile:write(message, "\n")
    end

    SoapySDR.Logger.registerHandler(logFunction)
    SoapySDR.Logger.setLevel(SoapySDR.Logger.Level.INFO)
    luaunit.assertEquals(SoapySDR.Logger.getLevel(), SoapySDR.Logger.Level.INFO)

    SoapySDR.Logger.log(SoapySDR.Logger.Level.WARNING, "Warning")
    SoapySDR.Logger.log(SoapySDR.Logger.Level.ERROR, "Error")
    SoapySDR.Logger.logf(SoapySDR.Logger.Level.INFO, "Info")
    SoapySDR.Logger.logf(SoapySDR.Logger.Level.INFO, "Info %s %d", "foo", 418)

    -- Reset to default logger, make sure the next call doesn't log to the file
    SoapySDR.Logger.registerHandler(nil)
    SoapySDR.Logger.log(SoapySDR.Logger.Level.INFO, "Info")

    tmpFile:seek("set")
    local logContents = tmpFile:read("*a")

    luaunit.assertEquals(logContents, "Warning\nError\nInfo\nInfo foo 418\n")
end

-- Make sure when LuaJIT throws an error inside the C callback, everything is
-- cleaned up nicely.
function testErrorDuringLog()
    local tmpFile = io.tmpfile()

    local function errorLogFunction(logLevel, message)
        -- Do something so we know it ran, otherwise any other error when
        -- invoking pcall will lead to a false positive.
        tmpFile:write(message, "\n")
        error(message)
    end

    SoapySDR.Logger.registerHandler(errorLogFunction)
    SoapySDR.Logger.setLevel(SoapySDR.Logger.Level.NOTICE)
    luaunit.assertEquals(SoapySDR.Logger.getLevel(), SoapySDR.Logger.Level.NOTICE)

    luaunit.assertFalse(pcall(SoapySDR.Logger.log, SoapySDR.Logger.Level.NOTICE, "Notice"))

    tmpFile:seek("set")
    local logContents = tmpFile:read("*a")

    luaunit.assertEquals(logContents, "Notice\n")
end

local runner = luaunit.LuaUnit.new()
os.exit(runner:runSuite())
