-- Copyright (c) 2021-2022 Nicholas Corgan
-- SPDX-License-Identifier: BSL-1.0

---
-- Abstraction for an SDR transceiver device
-- @module SoapySDR.Device

local ffi = require("ffi")
local lib = require("SoapySDR.Lib")
local Utility = require("SoapySDR.Utility")

--
-- Device-specific utility
--

-- Note: lengthPtr is only needed for lists
local function processDeviceOutput(ret, lengthPtr)
    -- See if an exception was caught in the last function call.
    local lastError = ffi.string(lib.SoapySDRDevice_lastError())
    if #lastError > 0 then
        error(lastError)
    end

    return Utility.processOutput(ret, lengthPtr)
end

local function processDeviceSetting(settingRet, settingName, allSettingInfo)
    local settingStr = processDeviceOutput(settingRet)
    local nativeSetting = ""

    for i=1,#allSettingInfo do
        local settingInfo = allSettingInfo[i]
        if settingInfo["name"] == settingName then
            nativeSetting = Utility.stringToSoapySetting(settingStr, settingInfo["argType"])
            break
        end
    end

    return nativeSetting
end

--
-- Device enumeration
--

local function enumerateDevices(args)
    local devs = nil
    local lengthPtr = ffi.new("size_t[1]")

    -- No parameter means no args
    args = args or ""

    -- Abstract away different functions
    local argsType = tostring(type(args))
    if argsType == "string" then
        devs = Utility.processRawKwargsList(
            lib.SoapySDRDevice_enumerateStrArgs(args, lengthPtr),
            lengthPtr)
    else
        devs = Utility.processRawKwargsList(
            lib.SoapySDRDevice_enumerate(Utility.toKwargs(args), lengthPtr),
            lengthPtr)
    end

    return devs
end

---
-- Abstraction for an SDR transceiver device.
-- @type Device
Device = {}
Device_mt =
{
    __index = Device,

    __tostring = function(self)
        return string.format("%s:%s", self:getDriverKey(), self:getHardwareKey())
    end
}

--- Make a new device object given device construction args.
--
-- @param[opt=""] args A table or markup string specifying construction args
-- @usage
-- local dev1 = SoapySDR.Device.new()
-- local dev2 = SoapySDR.Device.new("driver=rtlsdr,serial=00000001")
-- local dev3 = SoapySDR.Device.new({driver="audio",device_id=0})
function Device.new(args)
    -- No parameter means no args
    args = args or ""

    -- Abstract away different C constructor functions
    local dev = nil
    local argsType = tostring(type(args))
    if argsType == "string" then
        dev = ffi.gc(
            lib.SoapySDRDevice_makeStrArgs(args),
            lib.SoapySDRDevice_unmake)
    else
        dev = ffi.gc(
            lib.SoapySDRDevice_make(Utility.toKwargs(args)),
            lib.SoapySDRDevice_unmake)
    end

    if dev == nil then
        error("Invalid device args")
    end

    return setmetatable({__deviceHandle = dev}, Device_mt)
end

--
-- Identification API
--

---
-- A key that uniquely identifies the device driver.
-- This key identifies the underlying implementation.
-- Several variants of a product may share a driver.
--
-- @treturn string Driver key
function Device:getDriverKey()
    return processDeviceOutput(lib.SoapySDRDevice_getDriverKey(self.__deviceHandle))
end

---
-- A key that uniquely identifies the hardware.
-- This key should be meaningful to the user
-- to optimize for the underlying hardware.
--
-- @treturn string Hardware key
function Device:getHardwareKey()
    return processDeviceOutput(lib.SoapySDRDevice_getHardwareKey(self.__deviceHandle))
end

---
-- Query a dictionary of available device information.
-- This dictionary can any number of values like
-- vendor name, product name, revisions, serials...
--
-- @treturn table A dictionary-like table with hardware info
function Device:getHardwareInfo()
    return processDeviceOutput(lib.SoapySDRDevice_getHardwareInfo(self.__deviceHandle))
end

--
-- Channels API
--

---
-- Set the frontend mapping of available DSP units to RF frontends.
-- This mapping controls channel mapping and channel availability.
--
-- @tparam SoapySDR.Direction direction the channel direction (RX or TX)
-- @tparam string mapping a vendor-specific mapping string
function Device:setFrontendMapping(direction, mapping)
    return processDeviceOutput(lib.SoapySDRDevice_setFrontendMapping(
        self.__deviceHandle,
        direction,
        Utility.toString(mapping)))
end

---
-- Get the mapping configuration string.
--
-- @tparam SoapySDR.Direction direction the channel direction (RX or TX)
-- @treturn string The vendor-specific mapping string
function Device:getFrontendMapping(direction)
    return processDeviceOutput(lib.SoapySDRDevice_getFrontendMapping(
        self.__deviceHandle,
        direction))
end

---
-- Get a number of channels given the streaming direction
--
-- @tparam SoapySDR.Direction direction the channel direction (RX or TX)
function Device:getNumChannels(direction)
    return processDeviceOutput(lib.SoapySDRDevice_getNumChannels(
        self.__deviceHandle,
        direction))
end

---
-- Query a dictionary of available channel information.
-- This dictionary can any number of values like
-- decoder type, version, available functions...
--
-- @tparam SoapySDR.Direction direction the channel direction (RX or TX)
-- @tparam uint channel an available channel on the device
-- @treturn table A dictionary-like table with channel information
function Device:getChannelInfo(direction, channel)
    return processDeviceOutput(lib.SoapySDRDevice_getChannelInfo(
        self.__deviceHandle,
        direction,
        channel))
end

---
-- Find out if the specified channel is full or half duplex.
--
-- @tparam SoapySDR.Direction direction the channel direction RX or TX
-- @tparam uint channel an available channel on the device
-- @treturn boolean True for full duplex, false for half duplex
function Device:getFullDuplex(direction, channel)
    return processDeviceOutput(lib.SoapySDRDevice_getFullDuplex(
        self.__deviceHandle,
        direction,
        channel))
end

--
-- Stream API
--

---
-- Query a list of the available stream formats.
--
-- @tparam SoapySDR.Direction direction the channel direction (RX or TX)
-- @tparam uint channel an available channel on the device
-- @treturn table A list of allowed @{SoapySDR.Format} strings.
function Device:getStreamFormats(direction, channel)
    local lengthPtr = ffi.new("size_t[1]")
    return processDeviceOutput(
        lib.SoapySDRDevice_getStreamFormats(
            self.__deviceHandle,
            direction,
            channel,
            lengthPtr),
        lengthPtr)
end

---
-- Get the hardware's native stream format for this channel.
-- This is the format used by the underlying transport layer.
--
-- @tparam SoapySDR.Direction direction the channel direction (RX or TX)
-- @tparam uint channel an available channel on the device
-- @treturn string The device's native stream buffer @{SoapySDR.Format} string
-- @treturn double The device's native fullscale conversion value
--
-- @usage
-- local format, fullScale = unpack(sdr:getNativeStreamFormat(SoapySDR.Direction.TX, 0))
function Device:getNativeStreamFormat(direction, channel)
    local fullScalePtr = ffi.new("double[1]")

    local format = processDeviceOutput(
        lib.SoapySDRDevice_getNativeStreamFormat(
            self.__deviceHandle,
            direction,
            channel,
            fullScalePtr),
        fullScalePtr)

    return {format, tonumber(fullScalePtr[0])}
end

---
-- Query the valid stream arguments for the given chain.
--
-- @tparam SoapySDR.Direction direction the channel direction (RX or TX)
-- @tparam uint channel an available channel on the device
-- @treturn table A list of @{SoapySDR.ArgInfo} tables
function Device:getStreamArgsInfo(direction, channel)
    local lengthPtr = ffi.new("size_t[1]")
    return processDeviceOutput(
        lib.SoapySDRDevice_getStreamArgsInfo(
            self.__deviceHandle,
            direction,
            channel,
            lengthPtr),
        lengthPtr)
end

---
-- Initialize a stream given a list of channels and stream arguments.
-- The implementation may change switches or power-up components.
-- All stream API calls should be usable with the new stream object
-- after setupStream() is complete, regardless of the activity state.
--
-- The API allows any number of simultaneous TX and RX streams, but many dual-channel
-- devices are limited to one stream in each direction, using either one or both channels.
-- This call will return an error if an unsupported combination is requested,
-- or if a requested channel in this direction is already in use by another stream.
--
-- When multiple channels are added to a stream, they are typically expected to have
-- the same sample rate.
--
-- @tparam SoapySDR.Direction direction the channel direction (RX or TX)
-- @tparam string format A supported @{SoapySDR.Format} string
-- @tparam table channels A list of device channels
-- @param[opt=""] args Stream arguments (can be omitted or passed as string or table)
-- @return An opaque stream handle to be passed into other functions
-- @see Device:getStreamArgsInfo
--
-- @usage
-- local direction = SoapySDR.Direction.RX
-- local format = SoapySDR.Format.CF32
-- local channels = {0,1}
-- local args = "bufflen=8192,buffers=15"
--
-- local stream = sdr:setupStream(direction, format, channels, args)
function Device:setupStream(direction, format, channels, args)
    local ret = processDeviceOutput(lib.SoapySDRDevice_setupStream(
        self.__deviceHandle,
        direction,
        Utility.toString(format),
        Utility.luaArrayToFFIArray(channels, "size_t"),
        #channels,
        Utility.toKwargs(args)))

    return ret
end

---
-- Close an open stream created by setupStream.
-- The implementation may change switches or power-down components.
--
-- @param stream stream handle returned by @{Device:setupStream}
function Device:closeStream(stream)
    return processDeviceOutput(lib.SoapySDRDevice_closeStream(
        self.__deviceHandle,
        stream))
end

---
-- Get the stream's maximum transmission unit (MTU) in number of elements.
-- The MTU specifies the maximum payload transfer in a stream operation.
-- This value can be used as a stream buffer allocation size that can
-- best optimize throughput given the underlying stream implementation.
--
-- @param stream stream handle returned by @{Device:setupStream}
-- @treturn uint The MTU in number of stream elements (always > 0)
function Device:getStreamMTU(stream)
    return processDeviceOutput(lib.SoapySDRDevice_getStreamMTU(
        self.__deviceHandle,
        stream))
end

---
-- Activate a stream.
-- Call activate to prepare a stream before using read/write().
-- The implementations control switches or stimulate data flow.
--
-- The timeNs is only valid when the flags have HAS_TIME.
-- The numElems count can be used to request a finite burst size.
-- The END_BURST flag can signal end on the finite burst.
-- Not all implementations will support the full range of options.
-- In this case, the implementation returns NOT_SUPPORTED.
--
-- @param stream stream handle returned by @{Device:setupStream}
-- @tparam[opt=0] SoapySDR.StreamFlags flags optional flag indicators about the stream.
-- @tparam[opt=0] uint timeNs optional activation time in nanoseconds
-- @tparam[opt=0] uint numElems optional element count for burst control
-- @return 0 for success or @{SoapySDR.Error} on failure.
function Device:activateStream(stream, flags, timeNs, numElems)
    -- To allow for optional parameters
    flags = flags or 0
    timeNs = timeNs or 0
    numElems = numElems or 0

    return processDeviceOutput(lib.SoapySDRDevice_activateStream(
        self.__deviceHandle,
        stream,
        flags,
        timeNs,
        numElems))
end

---
-- Deactivate a stream.
-- Call deactivate when not using using read/write().
-- The implementation control switches or halt data flow.
--
-- The timeNs is only valid when the flags have HAS_TIME.
-- Not all implementations will support the full range of options.
-- In this case, the implementation returns NOT_SUPPORTED.
--
-- @param stream stream handle returned by @{Device:setupStream}
-- @tparam[opt=0] SoapySDR.StreamFlags flags optional flag indicators about the stream.
-- @tparam[opt=0] uint timeNs optional deactivation time in nanoseconds.
-- @return 0 for success or @{SoapySDR.Error} on failure
function Device:deactivateStream(stream, flags, timeNs)
    -- To allow for optional parameters
    flags = flags or 0
    timeNs = timeNs or 0

    return processDeviceOutput(lib.SoapySDRDevice_deactivateStream(
        self.__deviceHandle,
        stream,
        flags,
        timeNs))
end

---
-- Read elements from a stream for reception.
--
-- @param stream stream handle returned by @{Device:setupStream}
-- @param buffs a LuaJIT FFI array of pointers to buffers of size numElems
-- @tparam uint numElems the number of elements in each buffer
-- @tparam[opt=100000] uint timeoutUs the timeout in microseconds
-- @return The number of elements read or @{SoapySDR.Error} on failure
-- @treturn SoapySDR.StreamFlags Metadata flags returned by the read operation
-- @treturn uint The buffer's timestamp in nanoseconds
--
-- @usage
-- local cf32Buff0 = ffi.new("complex float[?]", numElems)
-- local cf32Buff1 = ffi.new("complex float[?]", numElems)
-- local cf32Buffs = ffi.new("complex float*[2]", {cf32Buff0, cf32Buff1})
--
-- local ret, flags, timeNs = unpack(sdr:readStream(stream, cf32Buffs, numElems, timeoutUs))
function Device:readStream(stream, buffs, numElems, timeoutUs)
    -- To allow for optional parameters
    timeoutUs = timeoutUs or 100000

    local flagsPtr = ffi.new("int[1]")
    local timeNsPtr = ffi.new("long long[1]")

    local ret = processDeviceOutput(lib.SoapySDRDevice_readStream(
        self.__deviceHandle,
        stream,
        ffi.cast("void* const*", buffs),
        numElems,
        flagsPtr,
        timeNsPtr,
        timeoutUs))

    return {ret, tonumber(flagsPtr[0]), tonumber(timeNsPtr[0])}
end

---
-- Write elements to a stream for transmission.
--
-- @param stream stream handle returned by @{Device:setupStream}
-- @param buffs a LuaJIT FFI array of pointers to buffers of size numElems
-- @tparam uint numElems the number of elements in each buffer
-- @tparam[opt=0] SoapySDR.StreamFlags flagsIn input flags for the write operation
-- @tparam[opt=0] uint timeNs the buffer's timestamp in nanoseconds
-- @tparam[opt=100000] uint timeoutUs the timeout in microseconds
-- @return The number of elements written or @{SoapySDR.Error} on failure
-- @treturn SoapySDR.StreamFlags Metadata flags returned by the write operation
--
-- @usage
-- local cf32Buff0 = ffi.new("complex float[?]", numElems)
-- local cf32Buff1 = ffi.new("complex float[?]", numElems)
-- local cf32Buffs = ffi.new("complex float*[2]", {cf32Buff0, cf32Buff1})
--
-- local ret, flagsOut = unpack(sdr:writeStream(stream, cf32Buffs, numElems, flagsIn, timeNs, timeoutUs))
function Device:writeStream(stream, buffs, numElems, flagsIn, timeNs, timeoutUs)
    -- To allow for optional parameters
    flagsIn = flagsIn or 0
    timeNs = timeNs or 0
    timeoutUs = timeoutUs or 100000

    local flagsPtr = ffi.new("int[1]", {flagsIn})

    local ret = processDeviceOutput(lib.SoapySDRDevice_writeStream(
        self.__deviceHandle,
        stream,
        ffi.cast("const void* const*", buffs),
        numElems,
        flagsPtr,
        timeNs,
        timeoutUs))

    return {ret, tonumber(flagsPtr[0])}
end

---
-- Readback status information about a stream.
-- This call is typically used on a transmit stream
-- to report time errors, underflows, and burst completion.
--
-- @param stream stream handle returned by @{Device:setupStream}
-- @tparam[opt=100000] uint timeoutUs the timeout in microseconds
-- @return The number of elements written or @{SoapySDR.Error} on failure
-- @treturn SoapySDR.StreamFlags Metadata flags returned by the write operation
-- @treturn uint The buffer's timestamp in nanoseconds
function Device:readStreamStatus(stream, timeoutUs)
    -- To allow for optional parameters
    timeoutUs = timeoutUs or 100000

    local chanMaskPtr = ffi.new("size_t[1]")
    local flagsPtr = ffi.new("int[1]")
    local timeNsPtr = ffi.new("long long[1]")

    local ret = processDeviceOutput(lib.SoapySDRDevice_readStreamStatus(
        self.__deviceHandle,
        stream,
        chanMaskPtr,
        flagsPtr,
        timeNsPtr,
        timeoutUs))

    return {ret, tonumber(chanMaskPtr[0]), tonumber(flagsPtr[0]), tonumber(timeNsPtr[0])}
end

--
-- Antenna API
--

---
-- Get a list of available antennas to select on a given chain.
--
-- @tparam SoapySDR.Direction direction the channel direction (RX or TX)
-- @tparam uint channel an available channel on the device
-- @treturn table A list of available antenna names.
function Device:listAntennas(direction, channel)
    local lengthPtr = ffi.new("size_t[1]")
    return processDeviceOutput(
        lib.SoapySDRDevice_listAntennas(
            self.__deviceHandle,
            direction,
            channel,
            lengthPtr),
        lengthPtr)
end

---
-- Set the selected antenna on a chain.
--
-- @tparam SoapySDR.Direction direction the channel direction (RX or TX)
-- @tparam uint channel an available channel on the device
-- @tparam string name the name of an available antenna
-- @see Device:listAntennas
function Device:setAntenna(direction, channel, name)
    return processDeviceOutput(lib.SoapySDRDevice_setAntenna(
        self.__deviceHandle,
        direction,
        channel,
        Utility.toString(name)))
end

---
-- Get the selected antenna on a chain.
--
-- @tparam SoapySDR.Direction direction the channel direction (RX or TX)
-- @tparam uint channel an available channel on the device
-- @return The name of an available antenna.
-- @see Device:listAntennas
function Device:getAntenna(direction, channel)
    return processDeviceOutput(lib.SoapySDRDevice_getAntenna(
        self.__deviceHandle,
        direction,
        channel))
end

--
-- Frontend corrections API
--

---
-- Does the device support automatic DC offset corrections?
--
-- @tparam SoapySDR.Direction direction the channel direction (RX or TX)
-- @tparam uint channel an available channel on the device
-- @treturn boolean True if automatic corrections are supported
function Device:hasDCOffsetMode(direction, channel)
    return processDeviceOutput(lib.SoapySDRDevice_hasDCOffsetMode(
        self.__deviceHandle,
        direction,
        channel))
end

---
-- Set the automatic DC offset corrections mode.
--
-- @tparam SoapySDR.Direction direction the channel direction (RX or TX)
-- @tparam uint channel an available channel on the device
-- @tparam boolean automatic true for automatic offset correction
function Device:setDCOffsetMode(direction, channel, automatic)
    return processDeviceOutput(lib.SoapySDRDevice_setDCOffsetMode(
        self.__deviceHandle,
        direction,
        channel,
        automatic))
end

---
-- Get the automatic DC offset corrections mode.
--
-- @tparam SoapySDR.Direction direction the channel direction (RX or TX)
-- @tparam uint channel an available channel on the device
-- @treturn boolean True for automatic offset correction
function Device:getDCOffsetMode(direction, channel)
    return processDeviceOutput(lib.SoapySDRDevice_getDCOffsetMode(
        self.__deviceHandle,
        direction,
        channel))
end

---
-- Does the device support frontend DC offset correction?
--
-- @tparam SoapySDR.Direction direction the channel direction (RX or TX)
-- @tparam uint channel an available channel on the device
-- @treturn boolean true if DC offset corrections are supported
function Device:hasDCOffset(direction, channel)
    return processDeviceOutput(lib.SoapySDRDevice_hasDCOffset(
        self.__deviceHandle,
        direction,
        channel))
end

---
-- Set the frontend DC offset correction.
--
-- @tparam SoapySDR.Direction direction the channel direction (RX or TX)
-- @tparam uint channel an available channel on the device
-- @param offset new DC offset correction (can be either numeric or LuaJIT complex types)
--
-- @usage
-- sdr:setDCOffset(SoapySDR.Direction.RX, 0, 1) -- Becomes 1+0i
-- sdr:setDCOffset(SoapySDR.Direction.RX, 0, ffi.new("complex float", 1, 0))
-- sdr:setDCOffset(SoapySDR.Direction.RX, 0, ffi.new("complex", 1, 0))
function Device:setDCOffset(direction, channel, offset)
    local complexOffset = Utility.toComplex(offset)

    return processDeviceOutput(lib.SoapySDRDevice_setDCOffset(
        self.__deviceHandle,
        direction,
        channel,
        complexOffset.re,
        complexOffset.im))
end

---
-- Get the frontend DC offset correction.
--
-- @tparam SoapySDR.Direction direction the channel direction (RX or TX)
-- @tparam uint channel an available channel on the device
-- @treturn complex The frontend DC offset correction
function Device:getDCOffset(direction, channel)
    local iPtr = ffi.new("double[1]")
    local qPtr = ffi.new("double[1]")

    processDeviceOutput(lib.SoapySDRDevice_getDCOffset(
        self.__deviceHandle,
        direction,
        channel,
        iPtr,
        qPtr))

    return ffi.new("complex", iPtr[0], qPtr[0])
end

---
-- Does the device support automatic frontend IQ balance correction?
--
-- @tparam SoapySDR.Direction direction the channel direction (RX or TX)
-- @tparam uint channel an available channel on the device
-- @treturn boolean True if IQ balance corrections are supported
function Device:hasIQBalance(direction, channel)
    return processDeviceOutput(lib.SoapySDRDevice_hasIQBalance(
        self.__deviceHandle,
        direction,
        channel))
end

---
-- Set the frontend IQ balance correction.
-- 
-- @tparam SoapySDR.Direction direction the channel direction (RX or TX)
-- @tparam uint channel an available channel on the device
-- @param balance new IQ balance correction (can be either numeric or LuaJIT complex types)
--
-- @usage
-- sdr:setIQBalance(SoapySDR.Direction.RX, 0, 1) -- Becomes 1+0i
-- sdr:setIQBalance(SoapySDR.Direction.RX, 0, ffi.new("complex float", 1, 0))
-- sdr:setIQBalance(SoapySDR.Direction.RX, 0, ffi.new("complex", 1, 0))
function Device:setIQBalance(direction, channel, balance)
    local complexBalance = Utility.toComplex(balance)

    return processDeviceOutput(lib.SoapySDRDevice_setIQBalance(
        self.__deviceHandle,
        direction,
        channel,
        complexBalance.re,
        complexBalance.im))
end

---
-- Get the frontend IQ balance correction.
--
-- @tparam SoapySDR.Direction direction the channel direction (RX or TX)
-- @tparam uint channel an available channel on the device
-- @treturn complex The frontend IQ balance correction
function Device:getIQBalance(direction, channel)
    local iPtr = ffi.new("double[1]")
    local qPtr = ffi.new("double[1]")

    processDeviceOutput(lib.SoapySDRDevice_getIQBalance(
        self.__deviceHandle,
        direction,
        channel,
        iPtr,
        qPtr))

    return ffi.new("complex", iPtr[0], qPtr[0])
end

---
-- Does the device support automatic frontend IQ balance correction?
--
-- @tparam SoapySDR.Direction direction the channel direction (RX or TX)
-- @tparam uint channel an available channel on the device
-- @treturn boolean True if IQ balance corrections are supported
function Device:hasIQBalanceMode(direction, channel)
    return processDeviceOutput(lib.SoapySDRDevice_hasIQBalanceMode(
        self.__deviceHandle,
        direction,
        channel))
end

---
-- Set the automatic frontend IQ balance correction mode.
--
-- @tparam SoapySDR.Direction direction the channel direction (RX or TX)
-- @tparam uint channel an available channel on the device
-- @tparam boolean automatic true for automatic correction
function Device:setIQBalanceMode(direction, channel, automatic)
    return processDeviceOutput(lib.SoapySDRDevice_setIQBalanceMode(
        self.__deviceHandle,
        direction,
        channel,
        automatic))
end

---
-- Test the automatic front IQ balance correction mode.
--
-- @tparam SoapySDR.Direction direction the channel direction (RX or TX)
-- @tparam uint channel an available channel on the device
-- @treturn boolean true for automatic correction
function Device:getIQBalanceMode(direction, channel)
    return processDeviceOutput(lib.SoapySDRDevice_getIQBalanceMode(
        self.__deviceHandle,
        direction,
        channel))
end

---
-- Does the device support frontend frequency correction?
--
-- @tparam SoapySDR.Direction direction the channel direction (RX or TX)
-- @tparam uint channel an available channel on the device
-- @treturn boolean True if frequency corrections are supported
function Device:hasFrequencyCorrection(direction, channel)
    return processDeviceOutput(lib.SoapySDRDevice_hasFrequencyCorrection(
        self.__deviceHandle,
        direction,
        channel))
end

---
-- Set the frontend frequency correction value.
--
-- @tparam SoapySDR.Direction direction the channel direction (RX or TX)
-- @tparam uint channel an available channel on the device
-- @tparam double value the correction in PPM
function Device:setFrequencyCorrection(direction, channel, value)
    return processDeviceOutput(lib.SoapySDRDevice_setFrequencyCorrection(
        self.__deviceHandle,
        direction,
        channel,
        value))
end

---
-- Get the frontend frequency correction value.
--
-- @tparam SoapySDR.Direction direction the channel direction (RX or TX)
-- @tparam uint channel an available channel on the device
-- @treturn double The correction value in PPM
function Device:getFrequencyCorrection(direction, channel)
    return processDeviceOutput(lib.SoapySDRDevice_getFrequencyCorrection(
        self.__deviceHandle,
        direction,
        channel))
end

---
-- List available amplification elements.
-- Elements should be in order RF to baseband.
--
-- @tparam SoapySDR.Direction direction the channel direction (RX or TX)
-- @tparam uint channel an available channel
-- @return A list of gain string names
function Device:listGains(direction, channel)
    local lengthPtr = ffi.new("size_t[1]")
    return processDeviceOutput(
        lib.SoapySDRDevice_listGains(
            self.__deviceHandle,
            direction,
            channel,
            lengthPtr),
        lengthPtr)
end

---
-- Does the device support automatic gain control?
--
-- @tparam SoapySDR.Direction direction the channel direction (RX or TX)
-- @tparam uint channel an available channel on the device
-- @treturn boolean True for automatic gain control
function Device:hasGainMode(direction, channel)
    return processDeviceOutput(lib.SoapySDRDevice_hasGainMode(
        self.__deviceHandle,
        direction,
        channel))
end

---
-- Set the automatic gain mode on the chain.
--
-- @tparam SoapySDR.Direction direction the channel direction (RX or TX)
-- @tparam uint channel an available channel on the device
-- @tparam boolean automatic True for automatic gain setting
function Device:setGainMode(direction, channel, automatic)
    return processDeviceOutput(lib.SoapySDRDevice_setGainMode(
        self.__deviceHandle,
        direction,
        channel,
        automatic))
end

---
-- Get the automatic gain mode on the chain.
--
-- @tparam SoapySDR.Direction direction the channel direction (RX or TX)
-- @tparam uint channel an available channel on the device
-- @treturn boolean True for automatic gain setting
function Device:getGainMode(direction, channel)
    return processDeviceOutput(lib.SoapySDRDevice_getGainMode(
        self.__deviceHandle,
        direction,
        channel))
end

---
-- Set the overall amplification in a chain.
-- The gain will be distributed automatically across available element.
--
-- @tparam SoapySDR.Direction direction the channel direction (RX or TX)
-- @tparam uint channel an available channel on the device
-- @tparam double value The new amplification value in dB
function Device:setGain(direction, channel, value)
    return processDeviceOutput(lib.SoapySDRDevice_setGain(
        self.__deviceHandle,
        direction,
        channel,
        value))
end

---
-- Set the value of a amplification element in a chain.
--
-- @tparam SoapySDR.Direction direction the channel direction (RX or TX)
-- @tparam uint channel an available channel on the device
-- @tparam string name the name of an amplification element
-- @tparam double value the new amplification value in dB
-- @see Device:listGains
function Device:setGainElement(direction, channel, name, value)
    return processDeviceOutput(lib.SoapySDRDevice_setGainElement(
        self.__deviceHandle,
        direction,
        channel,
        Utility.toString(name),
        value))
end

---
-- Get the overall value of the gain elements in a chain.
--
-- @tparam SoapySDR.Direction direction the channel direction (RX or TX)
-- @tparam uint channel an available channel on the device
-- @treturn double The value of the gain in dB
function Device:getGain(direction, channel)
    return processDeviceOutput(lib.SoapySDRDevice_getGain(
        self.__deviceHandle,
        direction,
        channel))
end

---
-- Get the value of an individual amplification element in a chain.
--
-- @tparam SoapySDR.Direction direction the channel direction (RX or TX)
-- @tparam uint channel an available channel on the device
-- @tparam string name the name of an amplification element
-- @treturn double The value of the gain in dB
-- @see Device:listGains
function Device:getGainElement(direction, channel, name)
    return processDeviceOutput(lib.SoapySDRDevice_getGainElement(
        self.__deviceHandle,
        direction,
        channel,
        Utility.toString(name)))
end

---
-- Get the overall range of possible gain values.
--
-- @tparam SoapySDR.Direction direction the channel direction (RX or TX)
-- @tparam uint channel an available channel on the device
-- @treturn SoapySDR.Range The range of possible values in dB
function Device:getGainRange(direction, channel)
    return processDeviceOutput(lib.SoapySDRDevice_getGainRange(
        self.__deviceHandle,
        direction,
        channel))
end

---
-- Get the range of possible gain values for a specific element.
--
-- @tparam SoapySDR.Direction direction the channel direction (RX or TX)
-- @tparam uint channel an available channel on the device
-- @tparam string name the name of an amplification element
-- @treturn SoapySDR.Range The range of possible values in dB
-- @see Device:listGains
function Device:getGainElementRange(direction, channel, name)
    return processDeviceOutput(lib.SoapySDRDevice_getGainElementRange(
        self.__deviceHandle,
        direction,
        channel,
        Utility.toString(name)))
end

---
-- Set the center frequency of the chain.
-- <ul>
-- <li>For RX, this specifies the down-conversion frequency.</li>
-- <li>For TX, this specifies the up-conversion frequency.</li>
-- </ul>
--
-- The default implementation of setFrequency() will tune the "RF"
-- component as close as possible to the requested center frequency.
-- Tuning inaccuracies will be compensated for with the "BB" component.
--
-- The args can be used to augment the tuning algorithm.
-- <ul>
-- <li> Use "OFFSET" to specify an "RF" tuning offset,
--      usually with the intention of moving the LO out of the passband.
--      The offset will be compensated for using the "BB" component.</li>
-- <li> Use the name of a component for the key and a frequency in Hz
--      as the value (any format) to enforce a specific frequency.
--      The other components will be tuned with compensation
--      to achieve the specified overall frequency.</li>
-- <li> Use the name of a component for the key and the value "IGNORE"
--      so that the tuning algorithm will avoid altering the component.</li>
-- <li> Vendor specific implementations can also use the same args to augment
--      tuning in other ways such as specifying fractional vs integer N tuning.</li>
-- </ul>
--
-- @tparam SoapySDR.Direction direction the channel direction (RX or TX)
-- @tparam uint channel an available channel on the device
-- @tparam double frequency the center frequency in Hz
-- @param[opt=""] args optional frequency args passed as string or table
-- @see Device:getFrequencyArgsInfo
function Device:setFrequency(direction, channel, frequency, args)
    return processDeviceOutput(lib.SoapySDRDevice_setFrequency(
        self.__deviceHandle,
        direction,
        channel,
        frequency,
        Utility.toKwargs(args)))
end

---
-- Tune the center frequency of the specified element.
-- <ul>
-- <li>For RX, this specifies the down-conversion frequency.</li>
-- <li>For TX, this specifies the up-conversion frequency.</li>
-- </ul>
--
-- Recommended names used to represent tunable components:
-- <ul>
-- <li>"CORR" - freq error correction in PPM</li>
-- <li>"RF" - frequency of the RF frontend</li>
-- <li>"BB" - frequency of the baseband DSP</li>
-- </ul>
--
-- @tparam SoapySDR.Direction direction the channel direction (RX or TX)
-- @tparam uint channel an available channel on the device
-- @tparam string name the name of a tunable element
-- @tparam double frequency the center frequency in Hz
-- @param[opt=""] args optional frequency args passed as string or table
-- @see Device:getFrequencyArgsInfo
-- @see Device:listFrequencies
function Device:setFrequencyComponent(direction, channel, name, frequency, args)
    return processDeviceOutput(lib.SoapySDRDevice_setFrequencyComponent(
        self.__deviceHandle,
        direction,
        channel,
        Utility.toString(name),
        frequency,
        Utility.toKwargs(args)))
end

---
-- Get the overall center frequency of the chain.
-- <ul>
-- <li>For RX, this specifies the down-conversion frequency.</li>
-- <li>For TX, this specifies the up-conversion frequency.</li>
-- </ul>
--
-- @tparam SoapySDR.Direction direction the channel direction (RX or TX)
-- @tparam uint channel an available channel on the device
-- @treturn double The center frequency in Hz
function Device:getFrequency(direction, channel)
    return processDeviceOutput(lib.SoapySDRDevice_getFrequency(
        self.__deviceHandle,
        direction,
        channel))
end

---
-- Get the frequency of a tunable element in the chain.
-- <ul>
-- <li>For RX, this specifies the down-conversion frequency.</li>
-- <li>For TX, this specifies the up-conversion frequency.</li>
-- </ul>
--
-- @tparam SoapySDR.Direction direction the channel direction (RX or TX)
-- @tparam uint channel an available channel on the device
-- @tparam string name the name of a tunable element
-- @treturn double The tunable element's frequency in Hz
-- @see Device:listFrequencies
function Device:getFrequencyComponent(direction, channel, name)
    return processDeviceOutput(lib.SoapySDRDevice_getFrequencyComponent(
        self.__deviceHandle,
        direction,
        channel,
        Utility.toString(name)))
end

---
-- Get the range of overall frequency values.
--
-- @tparam SoapySDR.Direction direction the channel direction (RX or TX)
-- @tparam uint channel an available channel on the device
-- @treturn table A list of @{SoapySDR.Range} frequency ranges in Hz
function Device:getFrequencyRange(direction, channel)
    local lengthPtr = ffi.new("size_t[1]")
    return processDeviceOutput(
        lib.SoapySDRDevice_getFrequencyRange(
            self.__deviceHandle,
            direction,
            channel,
            lengthPtr),
        lengthPtr)
end

---
-- Get the range of tunable values for the specified element.
--
-- @tparam SoapySDR.Direction direction the channel direction (RX or TX)
-- @tparam uint channel an available channel on the device
-- @tparam string name the name of a tunable element
-- @treturn table A list of @{SoapySDR.Range} frequency ranges in Hz
-- @see Device:listFrequencies
function Device:getFrequencyRangeComponent(direction, channel, name)
    local lengthPtr = ffi.new("size_t[1]")
    return processDeviceOutput(
        lib.SoapySDRDevice_getFrequencyRangeComponent(
            self.__deviceHandle,
            direction,
            channel,
            Utility.toString(name),
            lengthPtr),
        lengthPtr)
end

---
-- List available tunable elements in the chain.
-- Elements should be in order RF to baseband.
--
-- @tparam SoapySDR.Direction direction the channel direction (RX or TX)
-- @tparam uint channel an available channel
-- @treturn table A list of tunable elements by name
function Device:listFrequencies(direction, channel)
    local lengthPtr = ffi.new("size_t[1]")
    return processDeviceOutput(
        lib.SoapySDRDevice_listFrequencies(
            self.__deviceHandle,
            direction,
            channel,
            lengthPtr),
        lengthPtr)
end

---
-- Query the valid arguments for tune arguments.
--
-- @tparam SoapySDR.Direction direction the channel direction (RX or TX)
-- @tparam uint channel an available channel on the device
-- @treturn table A list of @{SoapySDR.ArgInfo} tables
function Device:getFrequencyArgsInfo(direction, channel)
    local lengthPtr = ffi.new("size_t[1]")
    return processDeviceOutput(
        lib.SoapySDRDevice_getFrequencyArgsInfo(
            self.__deviceHandle,
            direction,
            channel,
            lengthPtr),
        lengthPtr)
end

---
-- Set the baseband sample rate of the chain.
--
-- @tparam SoapySDR.Direction direction the channel direction (RX or TX)
-- @tparam uint channel an available channel on the device
-- @tparam double rate the sample rate in samples per second
function Device:setSampleRate(direction, channel, rate)
    return processDeviceOutput(lib.SoapySDRDevice_setSampleRate(
        self.__deviceHandle,
        direction,
        channel,
        rate))
end

---
-- Get the baseband sample rate of the chain.
--
-- @tparam SoapySDR.Direction direction the channel direction (RX or TX)
-- @tparam uint channel an available channel on the device
-- @treturn double The sample rate in samples per second
function Device:getSampleRate(direction, channel)
    return processDeviceOutput(lib.SoapySDRDevice_getSampleRate(
        self.__deviceHandle,
        direction,
        channel))
end

---
-- Get the range of possible baseband sample rates.
--
-- @tparam SoapySDR.Direction direction the channel direction (RX or TX)
-- @tparam uint channel an available channel on the device
-- @treturn table A list of @{SoapySDR.Range} sample rate ranges in samples per second
function Device:getSampleRateRange(direction, channel)
    local lengthPtr = ffi.new("size_t[1]")
    return processDeviceOutput(
        lib.SoapySDRDevice_getSampleRateRange(
            self.__deviceHandle,
            direction,
            channel,
            lengthPtr),
        lengthPtr)
end

---
-- Set the baseband filter width of the chain.
--
-- @tparam SoapySDR.Direction direction the channel direction (RX or TX)
-- @tparam uint channel an available channel on the device
-- @tparam double bw the baseband filter width in Hz
function Device:setBandwidth(direction, channel, bw)
    return processDeviceOutput(lib.SoapySDRDevice_setBandwidth(
        self.__deviceHandle,
        direction,
        channel,
        bw))
end

---
-- Get the baseband filter width of the chain.
--
-- @tparam SoapySDR.Direction direction the channel direction (RX or TX)
-- @tparam uint channel an available channel on the device
-- @treturn double The baseband filter width in Hz
function Device:getBandwidth(direction, channel)
    return processDeviceOutput(lib.SoapySDRDevice_getBandwidth(
        self.__deviceHandle,
        direction,
        channel))
end

---
-- Get the range of possible baseband filter widths.
--
-- @tparam SoapySDR.Direction direction the channel direction (RX or TX)
-- @tparam uint channel an available channel on the device
-- @treturn table A list of @{SoapySDR.Range} bandwidth ranges in Hz
function Device:getBandwidthRange(direction, channel)
    local lengthPtr = ffi.new("size_t[1]")
    return processDeviceOutput(
        lib.SoapySDRDevice_getBandwidthRange(
            self.__deviceHandle,
            direction,
            channel,
            lengthPtr),
        lengthPtr)
end

---
-- Set the master clock rate of the device.
--
-- @tparam double rate The clock rate in Hz
function Device:setMasterClockRate(rate)
    return processDeviceOutput(lib.SoapySDRDevice_setMasterClockRate(
        self.__deviceHandle,
        rate))
end

---
-- Get the master clock rate of the device.
--
-- @treturn double The clock rate in Hz
function Device:getMasterClockRate()
    return processDeviceOutput(lib.SoapySDRDevice_getMasterClockRate(self.__deviceHandle))
end

---
-- Get the range of available master clock rates.
--
-- @treturn table A list of clock rate ranges in Hz
function Device:getMasterClockRates()
    local lengthPtr = ffi.new("size_t[1]")
    return processDeviceOutput(
        lib.SoapySDRDevice_getMasterClockRates(self.__deviceHandle, lengthPtr),
        lengthPtr)
end

---
-- Set the reference clock rate of the device.
--
-- @tparam double rate the clock rate in Hz
function Device:setReferenceClockRate(rate)
    return processDeviceOutput(lib.SoapySDRDevice_setReferenceClockRate(
        self.__deviceHandle,
        rate))
end

---
-- Get the reference clock rate of the device.
--
-- @treturn double The clock rate in Hz
function Device:getReferenceClockRate()
    return processDeviceOutput(lib.SoapySDRDevice_getReferenceClockRate(self.__deviceHandle))
end

---
-- Get the range of available reference clock rates.
--
-- @treturn table A list of @{SoapySDR.Range} clock rate ranges in Hz
function Device:getReferenceClockRates()
    local lengthPtr = ffi.new("size_t[1]")
    return processDeviceOutput(
        lib.SoapySDRDevice_getReferenceClockRates(self.__deviceHandle, lengthPtr),
        lengthPtr)
end

---
-- Get the list of available clock sources.
--
-- @treturn table A list of clock source names
function Device:listClockSources()
    local lengthPtr = ffi.new("size_t[1]")
    return processDeviceOutput(
        lib.SoapySDRDevice_listClockSources(self.__deviceHandle, lengthPtr),
        lengthPtr)
end

---
-- Set the clock source on the device.
--
-- @tparam string source the name of a clock source
-- @see Device:listClockSources
function Device:setClockSource(source)
    return processDeviceOutput(lib.SoapySDRDevice_setClockSource(
        self.__deviceHandle,
        Utility.toString(source)))
end

---
-- Get the clock source of the device.
--
-- @treturn string The name of a clock source
-- @see Device:listClockSources
function Device:getClockSource()
    return processDeviceOutput(lib.SoapySDRDevice_getClockSource(self.__deviceHandle))
end

---
-- Get the list of available time sources.
--
-- @treturn table A list of time source names
function Device:listTimeSources()
    local lengthPtr = ffi.new("size_t[1]")
    return processDeviceOutput(
        lib.SoapySDRDevice_listTimeSources(
            self.__deviceHandle,
            lengthPtr),
        lengthPtr)
end

---
-- Set the time source on the device.
--
-- @tparam string source the name of a time source
-- @see Device:listTimeSources
function Device:setTimeSource(source)
    return processDeviceOutput(lib.SoapySDRDevice_setTimeSource(
        self.__deviceHandle,
        Utility.toString(source)))
end

---
-- Get the time source of the device.
--
-- @treturn string The name of a time source
-- @see Device:listTimeSources
function Device:getTimeSource()
    return processDeviceOutput(lib.SoapySDRDevice_getTimeSource(self.__deviceHandle))
end

---
-- Does this device have a hardware clock?
-- The "what" argument can refer to a specific time counter.
--
-- @tparam[opt=""] string what specific hardware clock to check
-- @treturn boolean True if the hardware clock exists
function Device:hasHardwareTime(what)
    return processDeviceOutput(lib.SoapySDRDevice_hasHardwareTime(
        self.__deviceHandle,
        Utility.toString(what)))
end

---
-- Read the time from the hardware clock on the device.
-- The "what" argument can refer to a specific time counter.
--
-- @tparam[opt=""] string what specific hardware clock to read
-- @treturn uint The time in nanoseconds
-- @see Device:hasHardwareTime
function Device:getHardwareTime(what)
    return processDeviceOutput(lib.SoapySDRDevice_getHardwareTime(
        self.__deviceHandle,
        Utility.toString(what)))
end

---
-- Write the time to the hardware clock on the device.
-- The "what" argument can refer to a specific time counter.
--
-- @tparam uint timeNs time in nanoseconds
-- @param[opt=""] what specific hardware clock to write
-- @see Device:hasHardwareTime
function Device:setHardwareTime(timeNs, what)
    return processDeviceOutput(lib.SoapySDRDevice_setHardwareTime(
        self.__deviceHandle,
        timeNs,
        Utility.toString(what)))
end

---
-- List the available global readback sensors.
-- A sensor can represent a reference lock, RSSI, temperature, etc.
--
-- @treturn table A list of available sensor string names
function Device:listSensors()
    local lengthPtr = ffi.new("size_t[1]")
    return processDeviceOutput(
        lib.SoapySDRDevice_listSensors(self.__deviceHandle, lengthPtr),
        lengthPtr)
end

---
-- Get meta-information about a global sensor.
--
-- @tparam string key the ID name of an available sensor
-- @treturn SoapySDR.ArgInfo Global sensor meta-information
-- @see Device:listSensors
function Device:getSensorInfo(key)
    return processDeviceOutput(
        lib.SoapySDRDevice_getSensorInfo(
            self.__deviceHandle,
            Utility.toString(key)))
end

---
-- Readback a global sensor given the name.
--
-- @tparam string key the ID name of an available sensor
-- @return The value of the sensor (type depends on sensor)
-- @see Device:listSensors
function Device:readSensor(key)
    return processDeviceOutput(
        lib.SoapySDRDevice_readSensor(
            self.__deviceHandle,
            Utility.toString(key)))
end

---
-- List the available channel readback sensors.
-- A sensor can represent a reference lock, RSSI, temperature.
--
-- @tparam SoapySDR.Direction direction the channel direction (RX or TX)
-- @tparam uint channel an available channel on the device
-- @treturn table A list of available sensor string names
function Device:listChannelSensors(direction, channel)
    local lengthPtr = ffi.new("size_t[1]")
    return processDeviceOutput(
        lib.SoapySDRDevice_listChannelSensors(
            self.__deviceHandle,
            direction,
            channel,
            lengthPtr),
        lengthPtr)
end

---
-- Get meta-information about a channel sensor.
--
-- @tparam SoapySDR.Direction direction the channel direction (RX or TX)
-- @tparam uint channel an available channel on the device
-- @tparam string key the ID name of an available channel sensor
-- @treturn SoapySDR.ArgInfo Channel sensor meta-information
-- @see Device:listChannelSensors
function Device:getChannelSensorInfo(direction, channel, key)
    return processDeviceOutput(lib.SoapySDRDevice_getChannelSensorInfo(
        self.__deviceHandle,
        direction,
        channel,
        Utility.toString(key)))
end

---
-- Readback a channel sensor given the name.
--
-- @tparam SoapySDR.Direction direction the channel direction (RX or TX)
-- @tparam uint channel an available channel on the device
-- @tparam string key the ID name of an available sensor
-- @return The value of the sensor (type depends on sensor)
-- @see Device:listChannelSensors
function Device:readChannelSensor(direction, channel, key)
    return processDeviceOutput(lib.SoapySDRDevice_readChannelSensor(
        self.__deviceHandle,
        direction,
        channel,
        Utility.toString(key)))
end

---
-- Get a list of available register interfaces by name.
--
-- @treturn table A list of available register interfaces
function Device:listRegisterInterfaces()
    local lengthPtr = ffi.new("size_t[1]")
    return processDeviceOutput(
        lib.SoapySDRDevice_listRegisterInterfaces(self.__deviceHandle, lengthPtr),
        lengthPtr)
end

---
-- Write a register on the device given the interface name.
-- This can represent a register on a soft CPU, FPGA, IC;
-- the interpretation is up the implementation to decide.
--
-- @tparam string name the name of a available register interface
-- @tparam uint32 addr the register address
-- @tparam uint32 value the 32-bit register value
-- @see Device:listRegisterInterfaces
function Device:writeRegister(name, addr, value)
    return processDeviceOutput(lib.SoapySDRDevice_writeRegister(
        self.__deviceHandle,
        Utility.toString(name),
        addr,
        value))
end

---
-- Read a register on the device given the interface name.
--
-- @tparam string name the name of a available register interface
-- @tparam uint32 addr the register address
-- @treturn uint32 The 32-bit register value
-- @see Device:listRegisterInterfaces
function Device:readRegister(name, addr)
    return processDeviceOutput(lib.SoapySDRDevice_readRegister(
        self.__deviceHandle,
        Utility.toString(name),
        addr))
end

---
-- Write a memory block on the device given the interface name.
-- This can represent a memory block on a soft CPU, FPGA, IC;
-- the interpretation is up the implementation to decide.
--
-- @tparam string name the name of a available memory block interface
-- @tparam uint32 addr the memory block start address
-- @tparam table values the memory block content, passed as an array
-- of 32-bit numbers
-- @see Device:listRegisterInterfaces
function Device:writeRegisters(name, addr, values)
    return processDeviceOutput(lib.SoapySDRDevice_writeRegisters(
        self.__deviceHandle,
        Utility.toString(name),
        addr,
        ffi.cast("unsigned*", Utility.luaArrayToFFIArray(values, "unsigned")),
        #values))
end

---
-- Read a memory block on the device given the interface name.
--
-- @tparam string name the name of a available memory block interface
-- @tparam uint32 addr the memory block start address
-- @tparam uint length number of 32-bit words to be read from memory block
-- @treturn table The memory block content, returned as a Lua array of 32-bit numbers
-- @see Device:listRegisterInterfaces
function Device:readRegisters(name, addr, length)
    local lengthPtr = ffi.new("size_t[1]", length)
    return processDeviceOutput(
        lib.SoapySDRDevice_readRegisters(
            self.__deviceHandle,
            Utility.toString(name),
            addr,
            lengthPtr),
        lengthPtr)
end

---
-- Describe the allowed keys and values used for settings.
--
-- @treturn table A list of @{SoapySDR.ArgInfo} tables
function Device:getSettingInfo()
    local lengthPtr = ffi.new("size_t[1]")
    return processDeviceOutput(
        lib.SoapySDRDevice_getSettingInfo(self.__deviceHandle, lengthPtr),
        lengthPtr)
end

---
-- Get information on a specific setting.
-- @tparam string key the setting identifier
-- @treturn table A @{SoapySDR.ArgInfo} table
function Device:getSettingInfoWithKey(key)
    return processDeviceOutput(
        lib.SoapySDRDevice_getSettingInfoWithKey(
            self.__deviceHandle,
            Utility.toString(key)))
end

---
-- Write an arbitrary setting on the device.
-- The interpretation is up the implementation.
--
-- @tparam string key the setting identifier
-- @param value the setting value (any type)
-- @see Device:getSettingInfo
function Device:writeSetting(key, value)
    return processDeviceOutput(lib.SoapySDRDevice_writeSetting(
        self.__deviceHandle,
        Utility.toString(key),
        Utility.soapySettingToString(value)))
end

---
-- Read an arbitrary setting on the device.
--
-- @tparam string key the setting identifier
-- @return The setting value (type depends on setting)
-- @see Device:getSettingInfo
function Device:readSetting(key)
    local keyStr = Utility.toString(key)

    return processDeviceSetting(
        lib.SoapySDRDevice_readSetting(
            self.__deviceHandle,
            keyStr),
        keyStr,
        self:getSettingInfo())
end

---
-- Describe the allowed keys and values used for the given channel's settings.
--
-- @tparam SoapySDR.Direction direction the channel direction (RX or TX)
-- @tparam uint channel an available channel on the device
-- @treturn table A list of @{SoapySDR.ArgInfo} tables
function Device:getChannelSettingInfo(direction, channel)
    local lengthPtr = ffi.new("size_t[1]")
    return processDeviceOutput(
        lib.SoapySDRDevice_getChannelSettingInfo(
            self.__deviceHandle,
            direction,
            channel,
            lengthPtr),
        lengthPtr)
end

---
-- Get information on a specific channel setting.
-- @tparam SoapySDR.Direction direction the channel direction (RX or TX)
-- @tparam uint channel an available channel on the device
-- @tparam string key the setting identifier
-- @treturn table A @{SoapySDR.ArgInfo} table
function Device:getChannelSettingInfoWithKey(direction, channel, key)
    return processDeviceOutput(
        lib.SoapySDRDevice_getChannelSettingInfoWithKey(
            self.__deviceHandle,
            direction,
            channel,
            Utility.toString(key)))
end

---
-- Write an arbitrary channel setting on the device.
-- The interpretation is up the implementation.
--
-- @tparam SoapySDR.Direction direction the channel direction (RX or TX)
-- @tparam uint channel an available channel on the device
-- @tparam string key the setting identifier
-- @param value the setting value (any type)
-- @see Device:getChannelSettingInfo
function Device:writeChannelSetting(direction, channel, key, value)
    return processDeviceOutput(lib.SoapySDRDevice_writeChannelSetting(
        self.__deviceHandle,
        direction,
        channel,
        Utility.toString(key),
        Utility.soapySettingToString(value)))
end

---
-- Read an arbitrary channel setting on the device.
--
-- @tparam SoapySDR.Direction direction the channel direction (RX or TX)
-- @tparam uint channel an available channel on the device
-- @tparam string key the setting identifier
-- @return The setting value (type depends on setting)
-- @see Device:getChannelSettingInfo
function Device:readChannelSetting(direction, channel, key)
    local keyStr = Utility.toString(key)

    return processDeviceSetting(
        lib.SoapySDRDevice_readChannelSetting(
            self.__deviceHandle,
            direction,
            channel,
            keyStr),
        keyStr,
        self:getChannelSettingInfo(direction, channel))
end

---
-- Get a list of available GPIO banks by name.
--
-- @treturn table A list of available GPIO banks
function Device:listGPIOBanks()
    local lengthPtr = ffi.new("size_t[1]")
    return processDeviceOutput(
        lib.SoapySDRDevice_listGPIOBanks(self.__deviceHandle, lengthPtr),
        lengthPtr)
end

---
-- Write the value of a GPIO bank.
--
-- @tparam string bank the name of an available bank
-- @tparam uint32 value a 32-bit integer representing GPIO bits
-- @see Device:listGPIOBanks
function Device:writeGPIO(bank, value)
    return processDeviceOutput(lib.SoapySDRDevice_writeGPIO(
        self.__deviceHandle,
        Utility.toString(bank),
        value))
end

---
-- Write the value of a GPIO bank with a modification mask.
--
-- @tparam string bank the name of an available bank
-- @tparam uint32 value a 32-bit integer representing GPIO bits
-- @tparam uint32 mask a 32-bit modification mask where 1 = modify
-- @see Device:listGPIOBanks
function Device:writeGPIOMasked(bank, value, mask)
    return processDeviceOutput(lib.SoapySDRDevice_writeGPIOMasked(
        self.__deviceHandle,
        Utility.toString(bank),
        value,
        mask))
end

---
-- Read back the value of a GPIO bank.
--
-- @tparam string bank the name of an available bank
-- @treturn uint32 A 32-bit integer representing GPIO bits
-- @see Device:listGPIOBanks
function Device:readGPIO(bank)
    return processDeviceOutput(lib.SoapySDRDevice_readGPIO(
        self.__deviceHandle,
        Utility.toString(bank)))
end

---
-- Write the data direction of a GPIO bank.
-- 1 bits represent outputs, 0 bits represent inputs.
--
-- @tparam string bank the name of an available bank
-- @tparam uint32 dir a 32-bit integer representing data direction bits
-- @see Device:listGPIOBanks
function Device:writeGPIODir(bank, dir)
    return processDeviceOutput(lib.SoapySDRDevice_writeGPIODir(
        self.__deviceHandle,
        Utility.toString(bank),
        dir))
end

---
-- Write the data direction of a GPIO bank with a modification mask.
-- 1 bits represent outputs, 0 bits represent inputs.
--
-- @tparam string bank the name of an available bank
-- @tparam uint32 dir a 32-bit integer representing data direction bits
-- @tparam uint32 mask a 32-bit modification mask where 1 = modify
-- @see Device:listGPIOBanks
function Device:writeGPIODirMasked(bank, dir, mask)
    return processDeviceOutput(lib.SoapySDRDevice_writeGPIODirMasked(
        self.__deviceHandle,
        Utility.toString(bank),
        dir,
        mask))
end

---
-- Read the data direction of a GPIO bank.
-- 1 bits represent outputs, 0 bits represent inputs.
--
-- @tparam string bank the name of an available bank
-- @treturn uint32 A 32-bit integer representing data direction bits
-- @see Device:listGPIOBanks
function Device:readGPIODir(bank)
    return processDeviceOutput(lib.SoapySDRDevice_readGPIODir(
        self.__deviceHandle,
        Utility.toString(bank)))
end

---
-- Write to an available I2C slave.
-- If the device contains multiple I2C masters,
-- the address bits can encode which master.
--
-- @tparam int addr the signed 32-bit address of the slave
-- @tparam string data an array of bytes to write, represented as a string
function Device:writeI2C(addr, data)
    local convertedData = Utility.toString(data)

    return processDeviceOutput(lib.SoapySDRDevice_writeI2C(
        self.__deviceHandle,
        addr,
        convertedData,
        #convertedData))
end

---
-- Read from an available I2C slave.
-- If the device contains multiple I2C masters,
-- the address bits can encode which master.
--
-- @tparam tint32 addr the signed 32-bit address of the slave
-- @tparam uint numBytes the number of bytes to read
-- @treturn string An array of bytes read from the slave, represented as a string
function Device:readI2C(addr, numBytes)
    local lengthPtr = ffi.new("size_t[1]", numBytes)

    -- The underlying C function doesn't return a null-terminated
    -- string, so we need to copy its output into a new string.
    local nativeBuffer = lib.SoapySDRDevice_readI2C(
        self.__deviceHandle,
        addr,
        lengthPtr)
    local ret = ffi.string(nativeBuffer, lengthPtr[0])
    lib.SoapySDR_free(nativeBuffer)

    return ret
end

---
-- Perform a SPI transaction and return the result.
-- It's up to the implementation to set the clock rate,
-- read edge, and the write edge of the SPI core.
-- SPI slaves without a readback pin will return 0.
--
-- If the device contains multiple SPI masters,
-- the address bits can encode which master.
--
-- @tparam int addr a signed 32-bit address of an available SPI slave
-- @tparam uint32 data the SPI data, numBits-1 is first out
-- @tparam uint numBits the number of bits to clock out
-- @treturn uint The readback data, numBits-1 is first in
function Device:transactSPI(addr, data, numBits)
    return processDeviceOutput(lib.SoapySDRDevice_transactSPI(
        self.__deviceHandle,
        addr,
        data,
        numBits))
end

---
-- Enumerate the available UART devices.
--
-- @treturn table A list of names of available UARTs
function Device:listUARTs()
    local lengthPtr = ffi.new("size_t[1]")
    return processDeviceOutput(
        lib.SoapySDRDevice_listUARTs(self.__deviceHandle, lengthPtr),
        lengthPtr)
end

---
-- Write data to a UART device.
-- It's up to the implementation to set the baud rate,
-- carriage return settings, and flushing on newline.
--
-- @tparam string which the name of an available UART
-- @tparam string data an array of bytes to write out, represented as a string
-- @see Device:listUARTs
function Device:writeUART(which, data)
    return lib.SoapySDRDevice_writeUART(
        self.__deviceHandle,
        Utility.toString(which),
        data)
end

---
-- Read bytes from a UART until timeout or newline.
-- It's up to the implementation to set the baud rate,
-- carriage return settings, and flushing on newline.
--
-- @tparam string which the name of an available UART
-- @tparam[opt=100000] uint timeoutUs a timeout in microseconds
-- @treturn string An array of bytes read from the UART, represented as a string
-- @see Device:listUARTs
function Device:readUART(which, timeoutUs)
    -- To allow for optional parameter
    timeoutUs = timeoutUs or 100000

    return processDeviceOutput(lib.SoapySDRDevice_readUART(
        self.__deviceHandle,
        Utility.toString(which),
        timeoutUs))
end

return {enumerateDevices, Device}
