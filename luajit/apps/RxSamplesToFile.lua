#!/usr/bin/env luajit
-- Copyright (c) 2021-2022 Nicholas Corgan
-- SPDX-License-Identifier: BSL-1.0

local SoapySDR = require("SoapySDR")

local argparse = require("argparse")

local bit = require("bit")
local ffi = require("ffi")
local math = require("math")

--
--
--

ffi.cdef[[
    typedef int mode_t;

    char *strerror(int errnum);

    int open(const char *pathname, int flags, mode_t mode);
    ssize_t write(int fd, const void *buf, size_t count);
    int close(int fd);
]]

O_RDONLY = 00000000
O_WRONLY = 00000001
O_RDWR   = 00000002
O_CREAT  = 00000100
S_IRUSR  = 00000400
S_IWUSR  = 00000200

--
--
--

local function toboolean(str)
    return (string.lower(str) == "true")
end

local parser = argparse("RxSamplesToFile")
parser:option("--args", "Device args", "")
parser:option("--output", "Output filepath")
parser:option("--nsamps", "Number of samples to write to file"):convert(tonumber)
parser:option("--rate", "Sample rate", 1e6):convert(tonumber)
parser:option("--freq", "Frequency (Hz)"):convert(tonumber)
parser:option("--bw", "Optional RX filter bandwidth (Hz)", 5e6):convert(tonumber)
parser:option("--chan", "RX channel", 0):convert(tonumber)
parser:option("--gain", "Optional RX gain", 0):convert(tonumber)
parser:option("--ant", "Optional RX antenna")
parser:option("--clock-rate", "Optional clock rate (Hz)"):convert(tonumber)
parser:option("--debug", "Output debug messages", false):convert(toboolean)

local args = parser:parse()

local function cleanup(fd, sdr, stream)
    local function __cleanup(fd, sdr, stream)
        if fd >= 0 then
            if args.debug then print("Closing file"); end
            local closeRet = ffi.C.close(fd)
            if closeRet ~= 0 then print(string.format("close() returned error: %s", ffi.string(ffi.C.strerror(ffi.errno())))); end
        end
        if sdr ~= nil and stream ~= nil then
            if args.debug then print("Deactivating and closing stream"); end
            sdr:deactivateStream(stream)
            sdr:closeStream(stream)
        end
    end

    print("Cleanup...")
    pcall(__cleanup, fd, sdr, stream)
end

--
--
--

-- Configure logger
local function logFunction(logLevel, message)
    print(string.format("%s: %s", os.date("%Y-%m-%d %H:%M:%S"), message))
end

local logLevel
if args.debug then logLevel = SoapySDR.Logger.Level.DEBUG
else               logLevel = SoapySDR.Logger.Level.WARNING
end
SoapySDR.Logger.registerHandler(logFunction)
SoapySDR.Logger.setLevel(logLevel)

-- Initialize device
local sdr = SoapySDR.Device.new(args.args)
print("----------")
print(string.format("Device:  %s", sdr))
print(string.format("Channel: %d", args.chan))

-- Set master clock rate
if args.clock_rate then
    print("----------")
    print(string.format("Setting master clock rate: %f Hz", args.clock_rate))
    sdr:setMasterClockRate(args.clock_rate)
    print(string.format("Actual:                    %f Hz", sdr:getMasterClockRate()))
end

-- Set sample rate
print("----------")
print(string.format("Setting RX sample rate: %f Hz", args.rate))
sdr:setSampleRate(SoapySDR.Direction.RX, args.chan, args.rate)
print(string.format("Actual:                 %f Hz", sdr:getSampleRate(SoapySDR.Direction.RX, args.chan)))

-- Set bandwidth
print("----------")
print(string.format("Setting RX bandwidth: %f Hz", args.bw))
sdr:setBandwidth(SoapySDR.Direction.RX, args.chan, args.bw)
print(string.format("Actual:               %f Hz", sdr:getBandwidth(SoapySDR.Direction.RX, args.chan)))

-- Set antenna
if args.ant then
    print("----------")
    print(string.format("Setting RX antenna: %s", args.ant))
    sdr:setAntenna(SoapySDR.Direction.RX, args.chan, args.ant)
    print(string.format("Actual:             %s", sdr:getAntenna(SoapySDR.Direction.RX, args.chan)))
end

-- Set overall gain
if args.gain then
    print("----------")
    print(string.format("Setting RX gain: %f dB", args.gain))
    sdr:setGain(SoapySDR.Direction.RX, args.chan, args.gain)
    print(string.format("Actual:          %f dB", sdr:getGain(SoapySDR.Direction.RX, args.chan)))
end

-- Tune frontend
print("----------")
print(string.format("Setting RX frequency: %f Hz", args.freq))
sdr:setFrequency(SoapySDR.Direction.RX, args.chan, args.freq)
print(string.format("Actual:               %f Hz", sdr:getFrequency(SoapySDR.Direction.RX, args.chan)))

-- Initialize stream
print("----------")
print("Initializing stream...")
local stream = sdr:setupStream(SoapySDR.Direction.RX, SoapySDR.Format.CF32, {args.chan})
if stream == nil then
    error("Failed to set up RX stream")
end

-- Activate stream
print("Activating stream...")
local errorCode = sdr:activateStream(stream)
if errorCode ~= 0 then
    cleanup(-1, sdr, stream)
    error("activateStream returned " .. SoapySDR.ErrorToString(errorCode))
end
local streamMTU = sdr:getStreamMTU(stream)

-- Set up samples
local samps = ffi.new("complex float[?]", streamMTU)
local samps2D = ffi.new("complex float*[1]", {samps})
local timeoutUs = 100000

-- Prepare file
print("----------")
print(string.format("Opening \"%s\" for write...", args.output))
local fd = ffi.C.open(args.output, bit.bor(O_RDWR, O_CREAT), bit.bor(S_IRUSR, S_IWUSR))
if fd < 0 then
    cleanup(fd, sdr, stream)
    error(string.format("open() returned error: %s", ffi.string(ffi.C.strerror(ffi.errno()))))
end

-- Run until we have all our samples
print("----------")
print(string.format("Reading %d samples...", args.nsamps))
local totalSamps = 0
while totalSamps < args.nsamps do
    local nsamps = math.min(streamMTU, (args.nsamps - totalSamps))
    if args.debug then print(string.format("About to read %d samples from device...", nsamps)); end

    local readStreamRet, flags, timeNs = unpack(sdr:readStream(stream, samps2D, nsamps, timeoutUs))
    if readStreamRet < 0 then
        cleanup(fd, sdr, stream)
        error(string.format("readStream() returned error: %s", SoapySDR.Error.ToString(readStreamRet)))
    elseif readStreamRet ~= nsamps then
        cleanup(fd, sdr, stream)
        error(string.format("Expected readStream() to read %d samples. Read %d", nsamps, readStreamRet))
    else
        local nsampsBytes = nsamps * 8
        if args.debug then print(string.format("About to write %d bytes to file...", nsampsBytes)); end
        local writeRet = tonumber(ffi.C.write(fd, samps, nsampsBytes))
        if writeRet < 0 then
            cleanup(fd, sdr, stream)
            error(string.format("write() returned error: %s", ffi.string(ffi.C.strerror(ffi.errno()))))
        elseif writeRet ~= nsampsBytes then
            cleanup(fd, sdr, stream)
            error(string.format("Expected to write %d bytes to file. Wrote %d.", nsamps, writeRet))
        else
            totalSamps = totalSamps + nsamps 
        end
    end
end

-- Clean up
cleanup(fd, sdr, stream)
print("Done!")
