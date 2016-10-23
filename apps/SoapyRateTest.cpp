// Copyright (c) 2016-2016 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Device.hpp>
#include <string>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <csignal>

static sig_atomic_t loopDone = false;
void sigIntHandler(const int)
{
    loopDone = true;
}

void runRateTestStreamLoop(SoapySDR::Device *device, SoapySDR::Stream *stream)
{
    device->activateStream(stream);
    signal(SIGINT, sigIntHandler);
    while (not loopDone)
    {
        
    }
    device->deactivateStream(stream);
}

int SoapySDRRateTest(
    const std::string &argStr,
    const double sampleRate,
    const std::string &channelStr,
    const std::string &directionStr)
{
    SoapySDR::Device *device(nullptr);

    try
    {
        device = SoapySDR::Device::make(argStr);

        //parse the direction to the integer enum
        int direction(-1);
        if (directionStr == "TX" or directionStr == "tx") direction = SOAPY_SDR_TX;
        if (directionStr == "RX" or directionStr == "rx") direction = SOAPY_SDR_RX;
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
        const auto format = device->getNativeStreamFormat(direction, channels.front(), fullScale);
        auto stream = device->setupStream(direction, format, channels);

        //run the rate test one setup is complete
        runRateTestStreamLoop(device, stream);

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
