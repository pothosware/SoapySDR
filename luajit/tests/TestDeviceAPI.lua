-- Copyright (c) 2021-2022 Nicholas Corgan
-- SPDX-License-Identifier: BSL-1.0

SoapySDR = require("SoapySDR")

bit = require("bit")
ffi = require("ffi")
luaunit = require("luaunit")

function testArgInfo()
    -- We can only get a meaningful ArgInfo from something that actually
    -- returns one, so take advantage of the loopback module if we have it.
    local dev = nil

    -- For the sake of pcall
    local function makeDev()
        dev = SoapySDR.Device.new("driver=loopback")
    end

    -- Hopefully this doesn't change.
    -- https://github.com/JuliaTelecom/SoapyLoopback/blob/main/Streaming.cpp#L51
    if pcall(makeDev) then
        local argInfoList = dev:getStreamArgsInfo(SoapySDR.Direction.RX, 0)
        luaunit.assertEquals(#argInfoList, 3)

        for i = 1,3 do
            luaunit.assertIsTable(argInfoList[i])
            luaunit.assertIsString(argInfoList[i].key)
            luaunit.assertIsNumber(argInfoList[i].value)
            luaunit.assertIsString(argInfoList[i].name)
            luaunit.assertIsString(argInfoList[i].description)
            luaunit.assertIsString(argInfoList[i].units)
            luaunit.assertEquals(argInfoList[i].argType, SoapySDR.ArgInfoType.INT)
            luaunit.assertIsTable(argInfoList[i].range)
            luaunit.assertIsNumber(argInfoList[i].range.minimum)
            luaunit.assertIsNumber(argInfoList[i].range.maximum)
            luaunit.assertIsNumber(argInfoList[i].range.step)
            luaunit.assertIsTable(argInfoList[i].options)
            luaunit.assertIsTable(argInfoList[i].optionNames)
        end
    else
        print "Loopback module not found. Skipping test."
    end
end

local function testDeviceWithDirection(device, direction)
    -- For the null device, functions generally don't do anything or return some
    -- hardcoded value, but we can still make sure the LuaJIT translates values
    -- back and forth as expected.

    --
    -- Channels API
    --
    device:setFrontendMapping(direction, "0:0")
    device:setFrontendMapping(direction, "0:0")
    luaunit.assertIsString(device:getFrontendMapping(direction))
    luaunit.assertIsTable(device:getChannelInfo(direction, 0))
    luaunit.assertIsBoolean(device:getFullDuplex(direction, 0))

    --
    -- Stream API
    --
    luaunit.assertIsTable(device:getStreamFormats(direction, 0))
    local format, fullScale = unpack(device:getNativeStreamFormat(direction, 0))
    luaunit.assertEquals(format, SoapySDR.Format.CS16)
    luaunit.assertEquals(fullScale, bit.lshift(1, 15))
    luaunit.assertIsTable(device:getStreamArgsInfo(direction, 0))

    local format = SoapySDR.Format.CF32
    local channels = {0,1}
    local args = "bufflen=8192,buffers=15"
    local stream = device:setupStream(direction, format, channels) -- Without optional param
    stream = device:setupStream(direction, format, channels, args) -- With optional param

    luaunit.assertEquals(device:getStreamMTU(stream), 1024)

    local flags = bit.bor(SoapySDR.StreamFlags.HAS_TIME, SoapySDR.StreamFlags.END_BURST)
    local timeNs = 1000
    local numElems = 1024
    luaunit.assertEquals(
        device:activateStream(stream, flags, timeNs, numElems),
        SoapySDR.Error.NOT_SUPPORTED)
    -- Default parameter causes specific null device behavior
    luaunit.assertEquals(device:activateStream(stream), 0)

    local cf32Buff = ffi.new("complex float[?]", numElems)
    local cf32Buff2D = ffi.new("complex float*[1]", {cf32Buff})
    local timeoutUs = 100000

    local readOutput = device:readStream(stream, cf32Buff2D, numElems, timeoutUs)
    luaunit.assertEquals(readOutput[1], SoapySDR.Error.NOT_SUPPORTED)
    luaunit.assertEquals(readOutput[2], 0)
    luaunit.assertEquals(readOutput[3], 0)

    readOutput = device:readStream(stream, cf32Buff2D, numElems) -- Without optional parameter
    luaunit.assertEquals(readOutput[1], SoapySDR.Error.NOT_SUPPORTED)
    luaunit.assertEquals(readOutput[2], 0)
    luaunit.assertEquals(readOutput[3], 0)

    local writeOutput = device:writeStream(stream, cf32Buff2D, numElems, flags, timeNs, timeoutUs)
    luaunit.assertEquals(writeOutput[1], SoapySDR.Error.NOT_SUPPORTED)
    luaunit.assertEquals(writeOutput[2], flags)

    writeOutput = device:writeStream(stream, cf32Buff2D, numElems) -- Without optional parameters
    luaunit.assertEquals(writeOutput[1], SoapySDR.Error.NOT_SUPPORTED)
    luaunit.assertEquals(writeOutput[2], 0)

    local readStreamStatusOutput = device:readStreamStatus(stream, timeoutUs)
    luaunit.assertEquals(readStreamStatusOutput[1], SoapySDR.Error.NOT_SUPPORTED)
    luaunit.assertEquals(readStreamStatusOutput[2], 0)
    luaunit.assertEquals(readStreamStatusOutput[3], 0)
    luaunit.assertEquals(readStreamStatusOutput[4], 0)

    local readStreamStatusOutput = device:readStreamStatus(stream) -- Without optional parameter
    luaunit.assertEquals(readStreamStatusOutput[1], SoapySDR.Error.NOT_SUPPORTED)
    luaunit.assertEquals(readStreamStatusOutput[2], 0)
    luaunit.assertEquals(readStreamStatusOutput[3], 0)
    luaunit.assertEquals(readStreamStatusOutput[4], 0)

    luaunit.assertEquals(
        device:deactivateStream(stream, flags, timeNs),
        SoapySDR.Error.NOT_SUPPORTED)
    luaunit.assertEquals(device:closeStream(stream), 0)

    --
    -- Antenna API
    --
    luaunit.assertIsTable(device:listAntennas(direction, 0))
    device:setAntenna(direction, 0, "ANT")
    luaunit.assertIsString(device:getAntenna(direction, 0))

    --
    -- Frontend corrections API
    --
    luaunit.assertIsBoolean(device:hasDCOffsetMode(direction, 0))
    device:setDCOffsetMode(direction, 0, true)
    luaunit.assertIsBoolean(device:getDCOffsetMode(direction, 0))
    luaunit.assertIsBoolean(device:hasDCOffset(direction, 0))

    -- Test all valid types
    device:setDCOffset(direction, 0, ffi.new("complex", 1.0, 0.0))
    device:setDCOffset(direction, 0, ffi.new("complex float", 1.0, 0.0))
    device:setDCOffset(direction, 0, 1.0)
    luaunit.assertEquals(
        ffi.typeof(device:getDCOffset(direction, 0)),
        ffi.typeof("complex"))

    luaunit.assertIsBoolean(device:hasIQBalance(direction, 0))

    -- Test all valid types
    device:setIQBalance(direction, 0, ffi.new("complex", 1.0, 0.0))
    device:setIQBalance(direction, 0, ffi.new("complex float", 1.0, 0.0))
    device:setIQBalance(direction, 0, 1.0)
    luaunit.assertEquals(
        ffi.typeof(device:getIQBalance(direction, 0)),
        ffi.typeof("complex"))

    luaunit.assertIsBoolean(device:hasIQBalanceMode(direction, 0))
    device:setIQBalanceMode(direction, 0, true)
    luaunit.assertIsBoolean(device:getIQBalanceMode(direction, 0))

    luaunit.assertIsBoolean(device:hasFrequencyCorrection(direction, 0))
    device:setFrequencyCorrection(direction, 0, 0.0)
    luaunit.assertIsNumber(device:getFrequencyCorrection(direction, 0))

    --
    -- Gain API
    --
    luaunit.assertIsTable(device:listGains(direction, 0))
    luaunit.assertIsBoolean(device:hasGainMode(direction, 0))
    device:setGainMode(direction, 0, true)
    luaunit.assertIsBoolean(device:getGainMode(direction, 0))

    device:setGain(direction, 0, 0.0)
    device:setGainElement(direction, 0, "", 0.0)
    luaunit.assertIsNumber(device:getGain(direction, 0))
    luaunit.assertIsNumber(device:getGainElement(direction, 0, ""))

    luaunit.assertIsTable(device:getGainRange(direction, 0))
    luaunit.assertIsTable(device:getGainElementRange(direction, 0, ""))

    --
    -- Frequency API
    --
    device:setFrequency(direction, 0, 0.0)
    device:setFrequency(direction, 0, 0.0, {})
    device:setFrequency(direction, 0, 0.0, "")

    device:setFrequencyComponent(direction, 0, "", 0.0)
    device:setFrequencyComponent(direction, 0, "", 0.0, {})
    device:setFrequencyComponent(direction, 0, "", 0.0, "")

    luaunit.assertEquals(device:getFrequency(direction, 0), 0.0)
    luaunit.assertEquals(device:getFrequencyComponent(direction, 0, ""), 0.0)

    luaunit.assertIsTable(device:listFrequencies(direction, 0))

    luaunit.assertIsTable(device:getFrequencyRange(direction, 0))
    luaunit.assertIsTable(device:getFrequencyRangeComponent(direction, 0, ""))
    luaunit.assertIsTable(device:getFrequencyArgsInfo(direction, 0))

    --
    -- Sample rate API
    --
    device:setSampleRate(direction, 0, 0.0)
    luaunit.assertEquals(device:getSampleRate(direction, 0), 0.0)
    luaunit.assertIsTable(device:getSampleRateRange(direction, 0))

    --
    -- Bandwidth API
    --
    device:setBandwidth(direction, 0, 0.0)
    luaunit.assertEquals(device:getBandwidth(direction, 0), 0.0)
    luaunit.assertIsTable(device:getBandwidthRange(direction, 0))

    --
    -- Sensor API
    --
    luaunit.assertIsTable(device:listChannelSensors(direction, 0))
    luaunit.assertIsTable(device:getChannelSensorInfo(direction, 0, ""))
    luaunit.assertIsString(device:readChannelSensor(direction, 0, ""))

    --
    -- Settings API
    --
    luaunit.assertIsTable(device:getChannelSettingInfo(direction, 0))
    luaunit.assertIsTable(device:getChannelSettingInfoWithKey(direction, 0, ""))
    device:writeChannelSetting(direction, 0, "", "")
    luaunit.assertIsString(device:readChannelSetting(direction, 0, ""))
end

function testDevice()
    -- Make sure either method works.
    local device = SoapySDR.Device.new("driver=null,type=null")
    luaunit.assertEquals(tostring(device), "null:null")
    device = SoapySDR.Device.new({driver="null",type="null"})
    luaunit.assertEquals(tostring(device), "null:null")

    --
    -- Identification API
    --
    luaunit.assertEquals("null", device:getDriverKey())
    luaunit.assertEquals("null", device:getHardwareKey())
    luaunit.assertIsTable(device:getHardwareInfo())

    --
    -- Clocking API
    --
    device:setMasterClockRate(0.0)
    luaunit.assertEquals(device:getMasterClockRate(), 0.0)
    luaunit.assertIsTable(device:getMasterClockRates())

    device:setReferenceClockRate(0.0)
    luaunit.assertEquals(device:getReferenceClockRate(), 0.0)
    luaunit.assertIsTable(device:getReferenceClockRates())

    luaunit.assertIsTable(device:listClockSources())
    device:setClockSource("")
    luaunit.assertEquals(device:getClockSource(), "")

    --
    -- Time API
    --
    luaunit.assertIsTable(device:listTimeSources())
    device:setTimeSource("")
    luaunit.assertEquals(device:getTimeSource(), "")

    luaunit.assertIsBoolean(device:hasHardwareTime())
    luaunit.assertIsBoolean(device:hasHardwareTime(""))
    luaunit.assertEquals(device:getHardwareTime(), 0)
    luaunit.assertEquals(device:getHardwareTime(""), 0)
    device:setHardwareTime(0)
    device:setHardwareTime(0, "")

    --
    -- Sensor API
    --
    luaunit.assertIsTable(device:listSensors())
    luaunit.assertIsTable(device:getSensorInfo(""))
    luaunit.assertEquals(device:readSensor(""), "")

    --
    -- Register API
    --
    luaunit.assertIsTable(device:listRegisterInterfaces())

    device:writeRegister("", 0, 0)
    luaunit.assertEquals(device:readRegister("", 0), 0)

    device:writeRegisters("", 0, {})
    luaunit.assertIsTable(device:readRegisters("", 0, 0))

    --
    -- Settings API
    --
    luaunit.assertIsTable(device:getSettingInfo())
    luaunit.assertIsTable(device:getSettingInfoWithKey(""))
    device:writeSetting("", "")
    luaunit.assertEquals(device:readSetting(""), "")

    --
    -- GPIO API
    --
    luaunit.assertIsTable(device:listGPIOBanks())
    device:writeGPIO("", 0)
    device:writeGPIOMasked("", 0, 0)
    luaunit.assertEquals(device:readGPIO(""), 0)
    device:writeGPIODir("", 0)
    device:writeGPIODirMasked("", 0, 0)
    luaunit.assertEquals(device:readGPIODir(""), 0)

    --
    -- I2C API
    --
    device:writeI2C(0, "")
    luaunit.assertEquals(device:readI2C(0, 0), "")

    --
    -- SPI API
    --
    luaunit.assertEquals(device:transactSPI(0, 0, 0), 0)

    --
    -- UART API
    --
    luaunit.assertIsTable(device:listUARTs())
    device:writeUART("", "")
    luaunit.assertEquals(device:readUART(""), "")

    --
    -- Test per-direction API with both directions
    --
    testDeviceWithDirection(device, SoapySDR.Direction.TX)
    testDeviceWithDirection(device, SoapySDR.Direction.RX)
end

local runner = luaunit.LuaUnit.new()
os.exit(runner:runSuite())
