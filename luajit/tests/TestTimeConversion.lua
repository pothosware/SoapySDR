-- Copyright (c) 2021 Nicholas Corgan
-- SPDX-License-Identifier: BSL-1.0

SoapySDR = require("SoapySDR")

luaunit = require("luaunit")
math = require("math")
os = require("os")

function RandomInt32()
    return math.floor(math.random() * 2147483647)
end

function testLoopbackTimeToTicks()
    function loopbackTest(timeNs, rate)
        local ticks = SoapySDR.Time.timeNsToTicks(timeNs, rate)
        local output = SoapySDR.Time.ticksToTimeNs(ticks, rate)

        local loopbackError = math.abs(timeNs - output) / 1e9
        luaunit.assertTrue(loopbackError < rate)
    end

    local timeNs = RandomInt32()
    local rates = {1e9, 52e6, 61.44e6, 100e6/3}
    for rateNum = 1, 4 do
        loopbackTest(timeNs, rates[rateNum])
        loopbackTest(-timeNs, rates[rateNum])
    end
end

function testLoopbackTicksToTime()
    function loopbackTest(ticks, rate)
        local timeNs = SoapySDR.Time.ticksToTimeNs(ticks, rate)
        local output = SoapySDR.Time.timeNsToTicks(timeNs, rate)
        luaunit.assertEquals(ticks, output)
    end

    local ticks = RandomInt32()
    local rates = {1e9, 52e6, 61.44e6, 100e6/3}
    for rateNum = 1, 4 do
        loopbackTest(ticks, rates[rateNum])
        loopbackTest(-ticks, rates[rateNum])
    end
end

math.randomseed(os.time())

local runner = luaunit.LuaUnit.new()
os.exit(runner:runSuite())
