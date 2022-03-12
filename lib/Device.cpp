// Copyright (c) 2014-2019 Josh Blum
// Copyright (c) 2016-2016 Bastille Networks
//                    2019 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Device.hpp>
#include <SoapySDR/Formats.hpp>
#include <cstdlib>
#include <algorithm> //min/max/find

SoapySDR::Device::~Device(void)
{
    return;
}

/*******************************************************************
 * Identification API
 ******************************************************************/
std::string SoapySDR::Device::getDriverKey(void) const
{
    return "";
}

std::string SoapySDR::Device::getHardwareKey(void) const
{
    return "";
}

SoapySDR::Kwargs SoapySDR::Device::getHardwareInfo(void) const
{
    return SoapySDR::Kwargs();
}

/*******************************************************************
 * Channels API
 ******************************************************************/
void SoapySDR::Device::setFrontendMapping(const int, const std::string &)
{
    return;
}

std::string SoapySDR::Device::getFrontendMapping(const int) const
{
    return "";
}

size_t SoapySDR::Device::getNumChannels(const int) const
{
    return 0;
}

SoapySDR::Kwargs SoapySDR::Device::getChannelInfo(const int, const size_t) const
{
    return SoapySDR::Kwargs();
}

bool SoapySDR::Device::getFullDuplex(const int, const size_t) const
{
    auto numRxChs = this->getNumChannels(SOAPY_SDR_RX);
    auto numTxChs = this->getNumChannels(SOAPY_SDR_TX);
    if (numRxChs > 0 and numTxChs == 0) return false; //no tx channels
    if (numRxChs == 0 and numTxChs > 0) return false; //no rx channels
    return true; //assume full duplex (usually true minus some tdd devices)
}

/*******************************************************************
 * Stream API
 ******************************************************************/
std::vector<std::string> SoapySDR::Device::getStreamFormats(const int, const size_t) const
{
    return std::vector<std::string>();
}

std::string SoapySDR::Device::getNativeStreamFormat(const int, const size_t, double &fullScale) const
{
    fullScale = double(1 << 15);
    return SOAPY_SDR_CS16;
}

SoapySDR::ArgInfoList SoapySDR::Device::getStreamArgsInfo(const int, const size_t) const
{
    return SoapySDR::ArgInfoList();
}

SoapySDR::Stream *SoapySDR::Device::setupStream(const int, const std::string &, const std::vector<size_t> &, const Kwargs &)
{
    return nullptr;
}

void SoapySDR::Device::closeStream(Stream *)
{
    return;
}

size_t SoapySDR::Device::getStreamMTU(Stream *) const
{
    //provide a non-zero default when the implementation does not overload the MTU
    return 1024;
}

int SoapySDR::Device::activateStream(Stream *, const int flags, const long long, const size_t)
{
    return (flags == 0)? 0 : SOAPY_SDR_NOT_SUPPORTED;
}

int SoapySDR::Device::deactivateStream(Stream *, const int flags, const long long)
{
    return (flags == 0)? 0 : SOAPY_SDR_NOT_SUPPORTED;
}

int SoapySDR::Device::readStream(Stream *, void * const *, const size_t, int &, long long &, const long)
{
    return SOAPY_SDR_NOT_SUPPORTED;
}

int SoapySDR::Device::writeStream(Stream *, const void * const *, const size_t, int &, const long long, const long)
{
    return SOAPY_SDR_NOT_SUPPORTED;
}

int SoapySDR::Device::readStreamStatus(Stream *, size_t &, int &, long long &, const long)
{
    return SOAPY_SDR_NOT_SUPPORTED;
}

/*******************************************************************
 * Direct buffer access API
 ******************************************************************/
size_t SoapySDR::Device::getNumDirectAccessBuffers(Stream *)
{
    return 0;
}

int SoapySDR::Device::getDirectAccessBufferAddrs(Stream *, const size_t, void **)
{
    return SOAPY_SDR_NOT_SUPPORTED;
}

int SoapySDR::Device::acquireReadBuffer(Stream *, size_t &, const void **, int &, long long &, const long)
{
    return SOAPY_SDR_NOT_SUPPORTED;
}

void SoapySDR::Device::releaseReadBuffer(Stream *, const size_t)
{
    return;
}

int SoapySDR::Device::acquireWriteBuffer(Stream *, size_t &, void **, const long)
{
    return SOAPY_SDR_NOT_SUPPORTED;
}

void SoapySDR::Device::releaseWriteBuffer(Stream *, const size_t, const size_t, int &, const long long)
{
    return;
}

/*******************************************************************
 * Antenna API
 ******************************************************************/
std::vector<std::string> SoapySDR::Device::listAntennas(const int, const size_t) const
{
    return std::vector<std::string>();
}

void SoapySDR::Device::setAntenna(const int, const size_t, const std::string &)
{
    return;
}

std::string SoapySDR::Device::getAntenna(const int, const size_t) const
{
    return "";
}

/*******************************************************************
 * Frontend corrections API
 ******************************************************************/

bool SoapySDR::Device::hasDCOffsetMode(const int, const size_t) const
{
    return false;
}

void SoapySDR::Device::setDCOffsetMode(const int, const size_t, const bool)
{
    return;
}

bool SoapySDR::Device::getDCOffsetMode(const int, const size_t) const
{
    return false;
}

bool SoapySDR::Device::hasDCOffset(const int, const size_t) const
{
    return false;
}

void SoapySDR::Device::setDCOffset(const int, const size_t, const std::complex<double> &)
{
    return;
}

std::complex<double> SoapySDR::Device::getDCOffset(const int, const size_t) const
{
    return std::complex<double>();
}

bool SoapySDR::Device::hasIQBalance(const int, const size_t) const
{
    return false;
}

void SoapySDR::Device::setIQBalance(const int, const size_t, const std::complex<double> &)
{
    return;
}

std::complex<double> SoapySDR::Device::getIQBalance(const int, const size_t) const
{
    return std::complex<double>();
}

bool SoapySDR::Device::hasIQBalanceMode(const int, const size_t) const
{
    return false;
}

void SoapySDR::Device::setIQBalanceMode(const int, const size_t, const bool)
{
    return;
}

bool SoapySDR::Device::getIQBalanceMode(const int, const size_t) const
{
    return false;
}

bool SoapySDR::Device::hasFrequencyCorrection(const int direction, const size_t channel) const
{
    //backwards compatibility with "CORR" string arg
    const auto components = this->listFrequencies(direction, channel);
    return (std::find(components.begin(), components.end(), "CORR") != components.end());
}

void SoapySDR::Device::setFrequencyCorrection(const int direction, const size_t channel, const double value)
{
    //backwards compatibility with "CORR" string arg
    const auto components = this->listFrequencies(direction, channel);
    if (std::find(components.begin(), components.end(), "CORR") != components.end())
    {
        this->setFrequency(direction, channel, "CORR", value);
    }
}

double SoapySDR::Device::getFrequencyCorrection(const int direction, const size_t channel) const
{
    //backwards compatibility with "CORR" string arg
    const auto components = this->listFrequencies(direction, channel);
    if (std::find(components.begin(), components.end(), "CORR") != components.end())
    {
        return this->getFrequency(direction, channel, "CORR");
    }
    return 0.0;
}

/*******************************************************************
 * Gain API
 ******************************************************************/
std::vector<std::string> SoapySDR::Device::listGains(const int, const size_t) const
{
    return std::vector<std::string>();
}

bool SoapySDR::Device::hasGainMode(const int, const size_t) const
{
    return false;
}

void SoapySDR::Device::setGainMode(const int, const size_t, const bool)
{
    return;
}

bool SoapySDR::Device::getGainMode(const int, const size_t) const
{
    return false;
}

void SoapySDR::Device::setGain(const int dir, const size_t channel, double gain)
{
    //algorithm to distribute overall gain (TX gets BB first, RX gets RF first)
    const auto names = this->listGains(dir, channel);
    if (dir == SOAPY_SDR_TX)
    {
        for (int i = names.size()-1; i >= 0; i--)
        {
            const auto r = this->getGainRange(dir, channel, names[i]);
            const double g = std::min(gain, r.maximum()-r.minimum());
            this->setGain(dir, channel, names[i], g+r.minimum());
            gain -= this->getGain(dir, channel, names[i])-r.minimum();
        }
    }
    if (dir == SOAPY_SDR_RX)
    {
        for (size_t i = 0; i < names.size(); i++)
        {
            const auto r = this->getGainRange(dir, channel, names[i]);
            const double g = std::min(gain, r.maximum()-r.minimum());
            this->setGain(dir, channel, names[i], g+r.minimum());
            gain -= this->getGain(dir, channel, names[i])-r.minimum();
        }
    }
}

void SoapySDR::Device::setGain(const int, const size_t, const std::string &, const double)
{
    return;
}

double SoapySDR::Device::getGain(const int dir, const size_t channel) const
{
    //algorithm to return an overall gain (summing each normalized gain)
    double gain = 0.0;
    for (const auto &name : this->listGains(dir, channel))
    {
        const auto r = this->getGainRange(dir, channel, name);
        gain += this->getGain(dir, channel, name)-r.minimum();
    }
    return gain;
}

double SoapySDR::Device::getGain(const int, const size_t, const std::string &) const
{
    return 0.0;
}

SoapySDR::Range SoapySDR::Device::getGainRange(const int, const size_t, const std::string &) const
{
    return SoapySDR::Range(0.0, 0.0);
}

SoapySDR::Range SoapySDR::Device::getGainRange(const int dir, const size_t channel) const
{
    //algorithm to return an overall gain range (use 0 to max possible on each element)
    double gain = 0.0;
    for (const auto &name : this->listGains(dir, channel))
    {
        const auto r = this->getGainRange(dir, channel, name);
        gain += r.maximum()-r.minimum();
    }
    return SoapySDR::Range(0.0, gain);
}

/*******************************************************************
 * Frequency API
 ******************************************************************/
void SoapySDR::Device::setFrequency(const int dir, const size_t chan, double freq, const Kwargs &args)
{
    const auto comps = this->listFrequencies(dir, chan);
    if (comps.empty()) return;

    //optional offset, use on RF element when specified
    const double offset = (args.count("OFFSET") != 0)?std::atof(args.at("OFFSET").c_str()):0.0;

    //distribute freq into RF then baseband components
    for (size_t comp_i = 0; comp_i < comps.size(); comp_i++)
    {
        const std::string &name = comps[comp_i];

        //add offset for RF element (first element)
        if (comp_i == 0) freq += offset;

        if (args.count(name) != 0 and args.at(name) == "IGNORE")
        {
            //do nothing, dont change component
        }
        else if (args.count(name) != 0 and args.at(name) != "DEFAULT")
        {
            //specific frequency for component specified
            const double f(std::atof(args.at(name).c_str()));
            this->setFrequency(dir, chan, name, f, args);
        }
        else
        {
            //otherwise use the remainder frequency
            this->setFrequency(dir, chan, name, freq, args);
        }

        //adjust overall freq for the remainder
        freq -= this->getFrequency(dir, chan, name);

        //then remove to compensate
        if (comp_i == 0) freq -= offset;
    }
}

void SoapySDR::Device::setFrequency(const int, const size_t, const std::string &, const double, const Kwargs &)
{
    return;
}

double SoapySDR::Device::getFrequency(const int dir, const size_t chan) const
{
    double freq = 0.0;

    //overall frequency is the sum of components
    for (const auto &comp : this->listFrequencies(dir, chan))
    {
        const std::string &name = comp;
        freq += this->getFrequency(dir, chan, name);
    }

    return freq;
}

double SoapySDR::Device::getFrequency(const int, const size_t, const std::string &) const
{
    return 0.0;
}

std::vector<std::string> SoapySDR::Device::listFrequencies(const int, const size_t) const
{
    return std::vector<std::string>();
}

SoapySDR::RangeList SoapySDR::Device::getFrequencyRange(const int dir, const size_t chan) const
{
    //get a list of tunable components
    const auto comps = this->listFrequencies(dir, chan);
    if (comps.empty()) return SoapySDR::RangeList();

    //get the range list for the RF component
    auto ranges = this->getFrequencyRange(dir, chan, comps.front());

    //use bandwidth setting to clip the range
    const double bw = this->getBandwidth(dir, chan);

    //add to the range list given subsequent components
    for (size_t comp_i = 1; comp_i < comps.size(); comp_i++)
    {
        const auto subRange = this->getFrequencyRange(dir, chan, comps[comp_i]);
        if (subRange.empty()) continue;

        double subRangeLow = subRange.front().minimum();
        if (bw > 0.0) subRangeLow = std::max(-bw/2, subRangeLow);

        double subRangeHigh = subRange.back().maximum();
        if (bw > 0.0) subRangeHigh = std::min(bw/2, subRangeHigh);

        for (size_t range_i = 0; range_i < ranges.size(); range_i++)
        {
            SoapySDR::Range &range = ranges[range_i];
            range = SoapySDR::Range(
                range.minimum() + subRangeLow,
                range.maximum() + subRangeHigh);
        }
    }

    return ranges;
}

SoapySDR::RangeList SoapySDR::Device::getFrequencyRange(const int, const size_t, const std::string &) const
{
    return SoapySDR::RangeList();
}

SoapySDR::ArgInfoList SoapySDR::Device::getFrequencyArgsInfo(const int dir, const size_t chan) const
{
    SoapySDR::ArgInfoList args;

    const auto comps = this->listFrequencies(dir, chan);

    if (comps.size() < 2) return args; //no tuning options with single components

    //support offset tuning
    {
        SoapySDR::ArgInfo info;
        info.key = "OFFSET";
        info.name = "LO Offset";
        info.value = "0.0";
        info.units = "Hz";
        info.type = SoapySDR::ArgInfo::FLOAT;
        info.description = "Tune the LO with an offset and compensate with the baseband CORDIC.";
        SoapySDR::RangeList ranges = this->getFrequencyRange(dir, chan, comps.at(1));
        if (not ranges.empty()) info.range = ranges.front();
        args.push_back(info);
    }

    //every tunable component becomes an option
    for (size_t comp_i = 1; comp_i < comps.size(); comp_i++)
    {
        SoapySDR::ArgInfo info;
        info.key = comps[comp_i];
        info.value = "DEFAULT";
        info.units = "Hz";
        info.type = SoapySDR::ArgInfo::FLOAT;
        info.description = "Specify a specific value for this component or IGNORE to skip tuning it.";
        info.options.push_back("DEFAULT");
        info.optionNames.push_back("Default");
        info.options.push_back("IGNORE");
        info.optionNames.push_back("Ignore");
        SoapySDR::RangeList ranges = this->getFrequencyRange(dir, chan, comps.at(comp_i));
        if (not ranges.empty()) info.range = ranges.front();
        args.push_back(info);
    }

    return args;
}

/*******************************************************************
 * Sample Rate API
 ******************************************************************/
void SoapySDR::Device::setSampleRate(const int, const size_t, const double)
{
    return;
}

double SoapySDR::Device::getSampleRate(const int, const size_t) const
{
    return 0.0;
}

std::vector<double> SoapySDR::Device::listSampleRates(const int, const size_t) const
{
    return std::vector<double>();
}

SoapySDR::RangeList SoapySDR::Device::getSampleRateRange(const int direction, const size_t channel) const
{
    SoapySDR::RangeList ranges;
    //call into the older deprecated listSampleRates() call
    for (auto &bw : this->listSampleRates(direction, channel))
    {
        ranges.push_back(SoapySDR::Range(bw, bw));
    }
    return ranges;
}

/*******************************************************************
 * Bandwidth API
 ******************************************************************/
void SoapySDR::Device::setBandwidth(const int, const size_t, const double)
{
    return;
}

double SoapySDR::Device::getBandwidth(const int, const size_t) const
{
    return 0.0;
}

std::vector<double> SoapySDR::Device::listBandwidths(const int, const size_t) const
{
    return std::vector<double>();
}

SoapySDR::RangeList SoapySDR::Device::getBandwidthRange(const int direction, const size_t channel) const
{
    SoapySDR::RangeList ranges;
    //call into the older deprecated listBandwidths() call
    for (auto &bw : this->listBandwidths(direction, channel))
    {
        ranges.push_back(SoapySDR::Range(bw, bw));
    }
    return ranges;
}

/*******************************************************************
 * Clocking API
 ******************************************************************/
void SoapySDR::Device::setMasterClockRate(const double)
{
    return;
}

double SoapySDR::Device::getMasterClockRate(void) const
{
    return 0.0;
}

SoapySDR::RangeList SoapySDR::Device::getMasterClockRates(void) const
{
    return SoapySDR::RangeList();
}

void SoapySDR::Device::setReferenceClockRate(const double)
{
    return;
}

double SoapySDR::Device::getReferenceClockRate(void) const
{
    return 0.0;
}

SoapySDR::RangeList SoapySDR::Device::getReferenceClockRates(void) const
{
    return SoapySDR::RangeList();
}

std::vector<std::string> SoapySDR::Device::listClockSources(void) const
{
    return std::vector<std::string>();
}

void SoapySDR::Device::setClockSource(const std::string &)
{
    return;
}

std::string SoapySDR::Device::getClockSource(void) const
{
    return "";
}

/*******************************************************************
 * Time API
 ******************************************************************/

std::vector<std::string> SoapySDR::Device::listTimeSources(void) const
{
    return std::vector<std::string>();
}

void SoapySDR::Device::setTimeSource(const std::string &)
{
    return;
}

std::string SoapySDR::Device::getTimeSource(void) const
{
    return "";
}

bool SoapySDR::Device::hasHardwareTime(const std::string &) const
{
    return false;
}

long long SoapySDR::Device::getHardwareTime(const std::string &) const
{
    return 0;
}

void SoapySDR::Device::setHardwareTime(const long long timeNs, const std::string &what)
{
    if (what == "CMD") this->setCommandTime(timeNs, what);
}

void SoapySDR::Device::setCommandTime(const long long, const std::string &)
{
    return;
}

/*******************************************************************
 * Sensor API
 ******************************************************************/
std::vector<std::string> SoapySDR::Device::listSensors(void) const
{
    return std::vector<std::string>();
}

SoapySDR::ArgInfo SoapySDR::Device::getSensorInfo(const std::string &) const
{
    return SoapySDR::ArgInfo();
}

std::string SoapySDR::Device::readSensor(const std::string &) const
{
    return "";
}

std::vector<std::string> SoapySDR::Device::listSensors(const int, const size_t) const
{
    return std::vector<std::string>();
}

SoapySDR::ArgInfo SoapySDR::Device::getSensorInfo(const int, const size_t, const std::string &) const
{
    return SoapySDR::ArgInfo();
}

std::string SoapySDR::Device::readSensor(const int, const size_t, const std::string &) const
{
    return "";
}

/*******************************************************************
 * Register API
 ******************************************************************/
std::vector<std::string> SoapySDR::Device::listRegisterInterfaces(void) const
{
    return std::vector<std::string>();
}

void SoapySDR::Device::writeRegister(const std::string &, const unsigned addr, const unsigned value)
{
    this->writeRegister(addr, value);
}

unsigned SoapySDR::Device::readRegister(const std::string &, const unsigned addr) const
{
    return this->readRegister(addr);
}

void SoapySDR::Device::writeRegister(const unsigned, const unsigned)
{
    return;
}

unsigned SoapySDR::Device::readRegister(const unsigned) const
{
    return 0;
}

void SoapySDR::Device::writeRegisters(const std::string &, const unsigned, const std::vector<unsigned> &)
{
    return;
}

std::vector<unsigned> SoapySDR::Device::readRegisters(const std::string &, const unsigned, size_t length) const
{
    return std::vector<unsigned>(length, 0);
}

/*******************************************************************
 * Settings API
 ******************************************************************/
SoapySDR::ArgInfoList SoapySDR::Device::getSettingInfo(void) const
{
    return SoapySDR::ArgInfoList();
}

SoapySDR::ArgInfo SoapySDR::Device::getSettingInfo(const std::string &key) const
{
    const auto allArgInfos = this->getSettingInfo();
    auto argInfoIter = std::find_if(
        allArgInfos.begin(),
        allArgInfos.end(),
        [&key](const SoapySDR::ArgInfo &argInfo)
        {
            return (argInfo.key == key);
        });

    return (argInfoIter != allArgInfos.end()) ? (*argInfoIter) : SoapySDR::ArgInfo();
}

void SoapySDR::Device::writeSetting(const std::string &, const std::string &)
{
    return;
}

std::string SoapySDR::Device::readSetting(const std::string &) const
{
    return "";
}

SoapySDR::ArgInfoList SoapySDR::Device::getSettingInfo(const int, const size_t) const
{
    return SoapySDR::ArgInfoList();
}

SoapySDR::ArgInfo SoapySDR::Device::getSettingInfo(const int direction, const size_t channel, const std::string &key) const
{
    const auto allArgInfos = this->getSettingInfo(direction, channel);
    auto argInfoIter = std::find_if(
        allArgInfos.begin(),
        allArgInfos.end(),
        [&key](const SoapySDR::ArgInfo &argInfo)
        {
            return (argInfo.key == key);
        });

    return (argInfoIter != allArgInfos.end()) ? (*argInfoIter) : SoapySDR::ArgInfo();
}

void SoapySDR::Device::writeSetting(const int, const size_t, const std::string &, const std::string &)
{
    return;
}

std::string SoapySDR::Device::readSetting(const int, const size_t, const std::string &) const
{
    return "";
}

/*******************************************************************
 * GPIO API
 ******************************************************************/
std::vector<std::string> SoapySDR::Device::listGPIOBanks(void) const
{
    return std::vector<std::string>();
}

void SoapySDR::Device::writeGPIO(const std::string &, const unsigned)
{
    return;
}

void SoapySDR::Device::writeGPIO(const std::string &bank, const unsigned value, const unsigned mask)
{
    //read, modify, write
    unsigned readback = this->readGPIO(bank);
    unsigned newValue = value | (readback & (~mask));
    this->writeGPIO(bank, newValue);
}

unsigned SoapySDR::Device::readGPIO(const std::string &) const
{
    return 0;
}

void SoapySDR::Device::writeGPIODir(const std::string &, const unsigned)
{
    return;
}

void SoapySDR::Device::writeGPIODir(const std::string &bank, const unsigned dir, const unsigned mask)
{
    //read, modify, write
    unsigned readback = this->readGPIODir(bank);
    unsigned newValue = dir | (readback & (~mask));
    this->writeGPIODir(bank, newValue);
}

unsigned SoapySDR::Device::readGPIODir(const std::string &) const
{
    return 0;
}


/*******************************************************************
 * I2C API
 ******************************************************************/
void SoapySDR::Device::writeI2C(const int, const std::string &)
{
    return;
}

std::string SoapySDR::Device::readI2C(const int, const size_t)
{
    return "";
}

/*******************************************************************
 * SPI API
 ******************************************************************/
unsigned SoapySDR::Device::transactSPI(const int, const unsigned, const size_t)
{
    return 0;
}

/*******************************************************************
 * UART API
 ******************************************************************/
std::vector<std::string> SoapySDR::Device::listUARTs(void) const
{
    return std::vector<std::string>();
}

void SoapySDR::Device::writeUART(const std::string &, const std::string &)
{
    return;
}

std::string SoapySDR::Device::readUART(const std::string &, const long) const
{
    return "";
}

/*******************************************************************
 * Native Access API
 ******************************************************************/
void* SoapySDR::Device::getNativeDeviceHandle(void) const
{
    return nullptr;
}
