// Copyright (c) 2015-2017 Josh Blum
// Copyright (c) 2016-2016 Bastille Networks
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Device.hpp>
#include <sstream>
#include <limits>

template <typename Type>
std::string toString(const std::vector<Type> &options)
{
    std::stringstream ss;
    if (options.empty()) return "";
    for (size_t i = 0; i < options.size(); i++)
    {
        if (not ss.str().empty()) ss << ", ";
        ss << options[i];
    }
    return ss.str();
}

std::string toString(const SoapySDR::Range &range)
{
    std::stringstream ss;
    ss << "[" << range.minimum() << ", " << range.maximum();
    if (range.step() != 0.0) ss << ", " << range.step();
    ss << "]";
    return ss.str();
}

std::string toString(const SoapySDR::RangeList &range, const double scale)
{
    const size_t MAXRLEN = 10; //for abbreviating long lists
    std::stringstream ss;
    for (size_t i = 0; i < range.size(); i++)
    {
        if (range.size() >= MAXRLEN and i >= MAXRLEN/2 and i < (range.size()-MAXRLEN/2))
        {
            if (i == MAXRLEN) ss << ", ...";
            continue;
        }
        if (not ss.str().empty()) ss << ", ";
        if (range[i].minimum() == range[i].maximum()) ss << (range[i].minimum()/scale);
        else ss << "[" << (range[i].minimum()/scale) << ", " << (range[i].maximum()/scale) << "]";
    }
    return ss.str();
}

std::string toString(const std::vector<double> &nums, const double scale)
{
    std::stringstream ss;

    if (nums.size() > 3)
    {
        ss << "[" << (nums.front()/scale) << ", " << (nums.back()/scale) << "]";
        return ss.str();
    }

    for (size_t i = 0; i < nums.size(); i++)
    {
        if (not ss.str().empty()) ss << ", ";
        ss << (nums[i]/scale);
    }
    return "[" + ss.str() + "]";
}

std::string toString(const SoapySDR::ArgInfo &argInfo, const std::string indent = "    ")
{
    std::stringstream ss;

    //name, or use key if missing
    std::string name = argInfo.name;
    if (argInfo.name.empty()) name = argInfo.key;
    ss << indent << " * " << name;

    //optional description
    std::string desc = argInfo.description;
    const std::string replace("\n"+indent+"   ");
    for (size_t pos = 0; (pos=desc.find("\n", pos)) != std::string::npos; pos+=replace.size())
    {
        desc.replace(pos, 1, replace);
    }
    if (not desc.empty()) ss << " - " << desc << std::endl << indent << "  ";

    //other fields
    ss << " [key=" << argInfo.key;
    if (not argInfo.units.empty()) ss << ", units=" << argInfo.units;
    if (not argInfo.value.empty()) ss << ", default=" << argInfo.value;

    //type
    switch (argInfo.type)
    {
    case SoapySDR::ArgInfo::BOOL: ss << ", type=bool"; break;
    case SoapySDR::ArgInfo::INT: ss << ", type=int"; break;
    case SoapySDR::ArgInfo::FLOAT: ss << ", type=float"; break;
    case SoapySDR::ArgInfo::STRING: ss << ", type=string"; break;
    }

    //optional range/enumeration
    if (argInfo.range.minimum() < argInfo.range.maximum()) ss << ", range=" << toString(argInfo.range);
    if (not argInfo.options.empty()) ss << ", options=(" << toString(argInfo.options) << ")";

    ss << "]";

    return ss.str();
}

std::string toString(const SoapySDR::ArgInfoList &argInfos)
{
    std::stringstream ss;

    for (size_t i = 0; i < argInfos.size(); i++)
    {
        ss << toString(argInfos[i]) << std::endl;
    }

    return ss.str();
}

std::string sensorReadings(SoapySDR::Device *device)
{
    std::stringstream ss;

    /*******************************************************************
     * Sensor readings
     ******************************************************************/
    std::vector<std::string> sensors = device->listSensors();

    for (size_t i = 0; i < sensors.size(); i++)
    {
        std::string key = sensors[i];
        SoapySDR::ArgInfo info = device->getSensorInfo(key);
        std::string reading = device->readSensor(key);

        ss << "     * " << sensors[i];
        if (not info.name.empty()) ss << " (" << info.name << ")";
        ss << ":";
        if (info.range.maximum() > std::numeric_limits<double>::min()) ss << toString(info.range);
        ss << toString(info.options);
        ss << " " << reading;
        if (not info.units.empty()) ss << " " << info.units;
        ss << std::endl;
        if (not info.description.empty()) ss << "        " << info.description << std::endl;
    }

    return ss.str();
}

std::string channelSensorReadings(SoapySDR::Device *device, const int dir, const size_t chan)
{
    std::stringstream ss;

    /*******************************************************************
     * Channel sensor readings
     ******************************************************************/
    std::vector<std::string> sensors = device->listSensors(dir, chan);

    for (size_t i = 0; i < sensors.size(); i++)
    {
        std::string key = sensors[i];
        SoapySDR::ArgInfo info = device->getSensorInfo(dir, chan, key);
        std::string reading = device->readSensor(dir, chan, key);

        ss << "     * " << sensors[i];
        if (not info.name.empty()) ss << " (" << info.name << ")";
        ss << ":";
        if (info.range.maximum() > std::numeric_limits<double>::min()) ss << toString(info.range);
        ss << toString(info.options);
        ss << " " << reading;
        if (not info.units.empty()) ss << " " << info.units;
        ss << std::endl;
        if (not info.description.empty()) ss << "        " << info.description << std::endl;
    }

    return ss.str();
}

static std::string probeChannel(SoapySDR::Device *device, const int dir, const size_t chan)
{
    std::stringstream ss;

    std::string dirName = (dir==SOAPY_SDR_TX)?"TX":"RX";
    ss << std::endl;
    ss << "----------------------------------------------------" << std::endl;
    ss << "-- " << dirName << " Channel " << chan << std::endl;
    ss << "----------------------------------------------------" << std::endl;

    // info
    const auto info = device->getChannelInfo(dir, chan);
    if (info.size() > 0)
    {
        ss << "  Channel Information:" << std::endl;
        for (const auto &it : info)
        {
            ss << "    " << it.first << "=" << it.second << std::endl;
        }
    }

    ss << "  Full-duplex: " << (device->getFullDuplex(dir, chan)?"YES":"NO") << std::endl;
    ss << "  Supports AGC: " << (device->hasGainMode(dir, chan)?"YES":"NO") << std::endl;

    //formats
    std::string formats = toString(device->getStreamFormats(dir, chan));
    if (not formats.empty()) ss << "  Stream formats: " << formats << std::endl;

    //native
    double fullScale = 0.0;
    std::string native = device->getNativeStreamFormat(dir, chan, fullScale);
    ss << "  Native format: " << native << " [full-scale=" << fullScale << "]" << std::endl;    

    //stream args
    std::string streamArgs = toString(device->getStreamArgsInfo(dir, chan));
    if (not streamArgs.empty()) ss << "  Stream args:" << std::endl << streamArgs;

    //antennas
    std::string antennas = toString(device->listAntennas(dir, chan));
    if (not antennas.empty()) ss << "  Antennas: " << antennas << std::endl;

    //corrections
    std::vector<std::string> correctionsList;
    if (device->hasDCOffsetMode(dir, chan)) correctionsList.push_back("DC removal");
    if (device->hasDCOffset(dir, chan)) correctionsList.push_back("DC offset");
    if (device->hasIQBalance(dir, chan)) correctionsList.push_back("IQ balance");
    std::string corrections = toString(correctionsList);
    if (not corrections.empty()) ss << "  Corrections: " << corrections << std::endl;

    //gains
    ss << "  Full gain range: " << toString(device->getGainRange(dir, chan)) << " dB" << std::endl;
    std::vector<std::string> gainsList = device->listGains(dir, chan);
    for (size_t i = 0; i < gainsList.size(); i++)
    {
        const std::string name = gainsList[i];
        ss << "    " << name << " gain range: " << toString(device->getGainRange(dir, chan, name)) << " dB" << std::endl;
    }

    //frequencies
    ss << "  Full freq range: " << toString(device->getFrequencyRange(dir, chan), 1e6) << " MHz" << std::endl;
    std::vector<std::string> freqsList = device->listFrequencies(dir, chan);
    for (size_t i = 0; i < freqsList.size(); i++)
    {
        const std::string name = freqsList[i];
        ss << "    " << name << " freq range: " << toString(device->getFrequencyRange(dir, chan, name), 1e6) << " MHz" << std::endl;
    }

    //freq args
    std::string freqArgs = toString(device->getFrequencyArgsInfo(dir, chan));
    if (not freqArgs.empty()) ss << "  Tune args:" << std::endl << freqArgs;

    //rates
    ss << "  Sample rates: " << toString(device->getSampleRateRange(dir, chan), 1e6) << " MSps" << std::endl;

    //bandwidths
    const auto bws = device->getBandwidthRange(dir, chan);
    if (not bws.empty()) ss << "  Filter bandwidths: " << toString(bws, 1e6) << " MHz" << std::endl;

    //sensors
    std::string sensors = toString(device->listSensors(dir, chan));
    if (not sensors.empty()) ss << "  Sensors: " << sensors << std::endl;
    ss << channelSensorReadings(device, dir, chan);

    //settings
    std::string settings = toString(device->getSettingInfo(dir, chan));
    if (not settings.empty()) ss << "  Other Settings:" << std::endl << settings;

    return ss.str();
}

std::string SoapySDRDeviceProbe(SoapySDR::Device *device)
{
    std::stringstream ss;

    /*******************************************************************
     * Identification info
     ******************************************************************/
    ss << std::endl;
    ss << "----------------------------------------------------" << std::endl;
    ss << "-- Device identification" << std::endl;
    ss << "----------------------------------------------------" << std::endl;

    ss << "  driver=" << device->getDriverKey() << std::endl;
    ss << "  hardware=" << device->getHardwareKey() << std::endl;
    for (const auto &it : device->getHardwareInfo())
    {
        ss << "  " << it.first << "=" << it.second << std::endl;
    }

    /*******************************************************************
     * Available peripherals
     ******************************************************************/
    ss << std::endl;
    ss << "----------------------------------------------------" << std::endl;
    ss << "-- Peripheral summary" << std::endl;
    ss << "----------------------------------------------------" << std::endl;

    size_t numRxChans = device->getNumChannels(SOAPY_SDR_RX);
    size_t numTxChans = device->getNumChannels(SOAPY_SDR_TX);
    ss << "  Channels: " << numRxChans << " Rx, " << numTxChans << " Tx" << std::endl;

    ss << "  Timestamps: " << (device->hasHardwareTime()?"YES":"NO") << std::endl;

    std::string clockSources = toString(device->listClockSources());
    if (not clockSources.empty()) ss << "  Clock sources: " << clockSources << std::endl;

    std::string timeSources = toString(device->listTimeSources());
    if (not timeSources.empty()) ss << "  Time sources: " << timeSources << std::endl;

    std::string sensors = toString(device->listSensors());
    if (not sensors.empty()) ss << "  Sensors: " << sensors << std::endl;
    ss << sensorReadings(device);

    std::string registers = toString(device->listRegisterInterfaces());
    if (not registers.empty()) ss << "  Registers: " << registers << std::endl;

    std::string settings = toString(device->getSettingInfo());
    if (not settings.empty()) ss << "  Other Settings:" << std::endl << settings;

    std::string gpios = toString(device->listGPIOBanks());
    if (not gpios.empty()) ss << "  GPIOs: " << gpios << std::endl;

    std::string uarts = toString(device->listUARTs());
    if (not uarts.empty()) ss << "  UARTs: " << uarts << std::endl;

    /*******************************************************************
     * Per-channel info
     ******************************************************************/
    for (size_t chan = 0; chan < numRxChans; chan++)
    {
        ss << probeChannel(device, SOAPY_SDR_RX, chan);
    }
    for (size_t chan = 0; chan < numTxChans; chan++)
    {
        ss << probeChannel(device, SOAPY_SDR_TX, chan);
    }

    return ss.str();
}
