#!/usr/bin/env luajit
-- Copyright (c) 2021-2022 Nicholas Corgan
-- SPDX-License-Identifier: BSL-1.0

local SoapySDR = require("SoapySDR")

local ffi = require("ffi")

local argparse = require("argparse")
local complex = require("complex")
local signal = require("posix.signal")

local math = require("math")
local os = require("os")

--
--
--

local function toboolean(str)
    return (string.lower(str) == "true")
end

local parser = argparse("SimpleSiggen")
parser:option("--args", "Device args", "")
parser:option("--rate", "Sample rate", 1e6):convert(tonumber)
parser:option("--ampl", "TX digital amplitude", 0.7):convert(tonumber)
parser:option("--tx-ant", "Optional TX antenna")
parser:option("--tx-gain", "Optional TX gain"):convert(tonumber)
parser:option("--tx-chan", "TX channel", 0):convert(tonumber)
parser:option("--freq", "Optional frequency (Hz)"):convert(tonumber)
parser:option("--tx-bw", "Optional TX filter bandwidth (Hz)", 5e6):convert(tonumber)
parser:option("--wave-freq", "Optional wave frequency (Hz)"):convert(tonumber)
parser:option("--clock-rate", "Optional clock rate (Hz)"):convert(tonumber)
parser:option("--debug", "Output debug messages", false):convert(toboolean)

local args = parser:parse()

local function cleanup(sdr, stream)
    local function __cleanup(sdr, stream)
        if sdr ~= nil and stream ~= nil then
            if args.debug then print("Deactivating and closing stream"); end
            sdr:deactivateStream(stream)
            sdr:closeStream(stream)
        end
    end

    print("Cleanup...")
    pcall(__cleanup, sdr, stream)
end

local function linspace(startval, endval, steps)
    interval = (endval / math.abs(endval)) * math.abs(endval - startval) / (steps - 1)
    local ret = {}

    for i = 1,steps do
        ret[i] = startval + (steps[i] * interval)
    end

    return ret
end

local function getRealAsComplexArray(real, len)
    local ret = {}
    for i = 1,len do
        ret[i] = ffi.new("complex", real, 0)
    end
    return ret
end

local function populateSampsWithReal(real, len, samps)
    for i = 0,len-1 do
        samps[i] = ffi.new("complex", real, 0)
    end
end

local function getSamp(ampl, phase)
    local lcomplexRet = complex.new(ampl, 0) * (complex.I * phase):exp()
    return ffi.new("complex", lcomplexRet:real(), lcomplexRet:imag())
end

local function populateSamps(ampl, phases, samps)
    local ret = {}
    for i = 0,#phases-1 do
        samps[i] = getSamp(ampl, phases[i+1])
    end
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
print(string.format("Channel: %d", args.tx_chan))

-- Set master clock rate
if args.clock_rate then
    print("----------")
    print(string.format("Setting master clock rate: %f Hz", args.clock_rate))
    sdr:setMasterClockRate(args.clock_rate)
    print(string.format("Actual:                    %f Hz", sdr:getMasterClockRate()))
end

-- Set sample rate
print("----------")
print(string.format("Setting TX sample rate: %f Hz", args.rate))
sdr:setSampleRate(SoapySDR.Direction.TX, args.tx_chan, args.rate)
print(string.format("Actual:                 %f Hz", sdr:getSampleRate(SoapySDR.Direction.TX, args.tx_chan)))

-- Set bandwidth
print("----------")
print(string.format("Setting TX bandwidth: %f Hz", args.tx_bw))
sdr:setBandwidth(SoapySDR.Direction.TX, args.tx_chan, args.tx_bw)
print(string.format("Actual:               %f Hz", sdr:getBandwidth(SoapySDR.Direction.TX, args.tx_chan)))

-- Set antenna
if args.tx_ant then
    print("----------")
    print(string.format("Setting TX antenna: %s", args.tx_ant))
    sdr:setAntenna(SoapySDR.Direction.TX, args.tx_chan, args.tx_ant)
    print(string.format("Actual:             %s", sdr:getAntenna(SoapySDR.Direction.TX, args.tx_chan)))
end

-- Set overall gain
if args.tx_gain then
    print("----------")
    print(string.format("Setting TX gain: %f dB", args.tx_gain))
    sdr:setGain(SoapySDR.Direction.TX, args.tx_chan, args.tx_gain)
    print(string.format("Actual:          %f dB", sdr:getGain(SoapySDR.Direction.TX, args.tx_chan)))
end

-- Tune frontend
if args.freq then
    print("----------")
    print(string.format("Setting TX frequency: %f Hz", args.freq))
    sdr:setFrequency(SoapySDR.Direction.TX, args.tx_chan, args.freq)
    print(string.format("Actual:               %f Hz", sdr:getFrequency(SoapySDR.Direction.TX, args.tx_chan)))
end

-- Initialize stream
local stream = sdr:setupStream(SoapySDR.Direction.TX, SoapySDR.Format.CF32, {args.tx_chan})
if stream == nil then
    error("Failed to set up TX stream")
end

-- Activate stream
local errorCode = sdr:activateStream(stream)
if errorCode ~= 0 then
    error("activateStream returned " .. SoapySDR.ErrorToString(errorCode))
end
local streamMTU = sdr:getStreamMTU(stream)

-- Set up samples
local samps = ffi.new("complex float[?]", mtu)
local samps2D = ffi.new("complex float*[1]", {samps})
local timeoutUs = 1000000

local waveFreq = args.wave_freq or (args.rate / 10)
local phaseAcc = 0
local phaseInc = 2 * math.pi * waveFreq / args.rate
populateSampsWithReal(ampl*streamMTU, streamMTU, samps)

-- Set up signal handling
local running = true

local function sighandler(signo)
    running = false
end

signal.signal(signal.SIGINT, sighandler)

-- Run siggen until interrupted
local totalSamps = 0
local lastTime = os.clock()

while running do
    local phaseAccNext = phaseAcc + (streamMTU * phaseInc)
    local phases = linspace(phaseAcc, phaseAccNext, streamMTU)
    populateSamps(ampl, phases, samps)
    phaseAcc = phaseAccNext

    while phaseAcc > (math.pi * 2) do
        phaseAcc = (phaseAcc - (math.pi * 2))
    end

    local writeStreamRet = sdr:writeStream(stream, samps2D, streamMTU, timeoutUs)
    if writeStreamRet < 0 then
        cleanup(sdr, stream)
        error(string.format("writeStream() returned error: %s", SoapySDR.Error.ToString(writeStreamRet)))
    elseif writeStreamRet ~= streamMTU then
        cleanup(sdr, stream)
        error(string.format("Expected writeStream() to consume all samples. Consumed %d.", writeStreamRet))
    end

    totalSamps = totalSamps + writeStreamRet
    local now = os.clock()
    local duration = now - lastTime
    if duration >= 5 then
        local rate = totalSamps / duration / 1e6
        print(string.format("LuaJIT siggen rate: %f Msps", rate))
        totalSamps = 0
        lasTime = now
    end
end

-- Clean up
cleanup(fd, sdr, stream)
print("Done!")
