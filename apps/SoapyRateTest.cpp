// Copyright (c) 2016-2017 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Device.hpp>
#include <SoapySDR/Formats.hpp>
#include <SoapySDR/Errors.hpp>
#include <string>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <csignal>
#include <chrono>
#include <cstdio>

static sig_atomic_t loopDone = false;
static void sigIntHandler(const int)
{
    loopDone = true;
}

void runRateTestStreamLoop(
    SoapySDR::Device *device,
    SoapySDR::Stream *stream,
    const int direction,
    const size_t numChans,
    const size_t elemSize)
{
    //allocate buffers for the stream read/write
    const size_t numElems = device->getStreamMTU(stream);
    std::vector<std::vector<char>> buffMem(numChans, std::vector<char>(elemSize*numElems));
    std::vector<void *> buffs(numChans);
    for (size_t i = 0; i < numChans; i++) buffs[i] = buffMem[i].data();

    //state collected in this loop
    unsigned int overflows(0);
    unsigned int underflows(0);
    unsigned long long totalSamples(0);
    const auto startTime = std::chrono::high_resolution_clock::now();
    auto timeLastPrint = std::chrono::high_resolution_clock::now();
    auto timeLastSpin = std::chrono::high_resolution_clock::now();
    auto timeLastStatus = std::chrono::high_resolution_clock::now();
    int spinIndex(0);

    std::cout << "Starting stream loop, press Ctrl+C to exit..." << std::endl;
    device->activateStream(stream);
    signal(SIGINT, sigIntHandler);
    while (not loopDone)
    {
        int ret(0);
        int flags(0);
        long long timeNs(0);
        switch(direction)
        {
        case SOAPY_SDR_RX:
            ret = device->readStream(stream, buffs.data(), numElems, flags, timeNs);
            break;
        case SOAPY_SDR_TX:
            ret = device->writeStream(stream, buffs.data(), numElems, flags, timeNs);
            break;
        }

        if (ret == SOAPY_SDR_TIMEOUT) continue;
        if (ret == SOAPY_SDR_OVERFLOW)
        {
            overflows++;
            continue;
        }
        if (ret == SOAPY_SDR_UNDERFLOW)
        {
            underflows++;
            continue;
        }
        if (ret < 0)
        {
            std::cerr << "Unexpected stream error " << SoapySDR::errToStr(ret) << std::endl;
            break;
        }
        totalSamples += ret;

        const auto now = std::chrono::high_resolution_clock::now();
        if (timeLastSpin + std::chrono::milliseconds(300) < now)
        {
            timeLastSpin = now;
            static const char spin[] = {"|/-\\"};
            printf("\b%c", spin[(spinIndex++)%4]);
            fflush(stdout);
        }
        //occasionally read out the stream status (non blocking)
        if (timeLastStatus + std::chrono::seconds(1) < now)
        {
            timeLastStatus = now;
            while (true)
            {
                size_t chanMask; int flags; long long timeNs;
                ret = device->readStreamStatus(stream, chanMask, flags, timeNs, 0);
                if (ret == SOAPY_SDR_OVERFLOW) overflows++;
                else if (ret == SOAPY_SDR_UNDERFLOW) underflows++;
                else if (ret == SOAPY_SDR_TIME_ERROR) {}
                else break;
            }
        }
        if (timeLastPrint + std::chrono::seconds(5) < now)
        {
            timeLastPrint = now;
            const auto timePassed = std::chrono::duration_cast<std::chrono::microseconds>(now - startTime);
            const auto sampleRate = double(totalSamples)/timePassed.count();
            printf("\b%g Msps\t%g MBps", sampleRate, sampleRate*numChans*elemSize);
            if (overflows != 0) printf("\tOverflows %u", overflows);
            if (underflows != 0) printf("\tUnderflows %u", underflows);
            printf("\n ");
        }

    }
    device->deactivateStream(stream);
}

int SoapySDRRateTest(
    const std::string &argStr,
    const double sampleRate,
    const std::string &formatStr,
    const std::string &channelStr,
    const std::string &directionStr)
{
    SoapySDR::Device *device(nullptr);

    try
    {
        device = SoapySDR::Device::make(argStr);

        //parse the direction to the integer enum
        int direction(-1);
        if (directionStr == "RX" or directionStr == "rx") direction = SOAPY_SDR_RX;
        if (directionStr == "TX" or directionStr == "tx") direction = SOAPY_SDR_TX;
        if (direction == -1) throw std::invalid_argument("direction not in RX/TX: " + directionStr);

        //build channels list, using KwargsFromString is a easy parsing hack
        std::vector<size_t> channels;
        for (const auto &pair : SoapySDR::KwargsFromString(channelStr))
        {
            channels.push_back(std::stoi(pair.first));
        }
        if (channels.empty()) channels.push_back(0);

        //initialize the sample rate for all channels
        for (const auto &chan : channels)
        {
            device->setSampleRate(direction, chan, sampleRate);
        }

        //create the stream, use the native format
        double fullScale(0.0);
        const auto format = formatStr.empty() ? device->getNativeStreamFormat(direction, channels.front(), fullScale) : formatStr;
        const size_t elemSize = SoapySDR::formatToSize(format);
        auto stream = device->setupStream(direction, format, channels);

        //run the rate test one setup is complete
        std::cout << "Stream format: " << format << std::endl;
        std::cout << "Num channels: " << channels.size() << std::endl;
        std::cout << "Element size: " << elemSize << " bytes" << std::endl;
        std::cout << "Begin " << directionStr << " rate test at " << (sampleRate/1e6) << " Msps" << std::endl;
        runRateTestStreamLoop(device, stream, direction, channels.size(), elemSize);

        //cleanup stream and device
        device->closeStream(stream);
        SoapySDR::Device::unmake(device);
    }
    catch (const std::exception &ex)
    {
        std::cerr << "Error in rate test: " << ex.what() << std::endl;
        SoapySDR::Device::unmake(device);
        return EXIT_FAILURE;
    }
    return EXIT_FAILURE;
}
