// Copyright (c) 2014-2014 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Device.hpp>

SoapySDR::Device::~Device(void)
{
    return;
}

void SoapySDR::Device::setFrontendMapping(const Direction, const std::string &)
{
    return;
}

std::string SoapySDR::Device::getFrontendMapping(const Direction) const
{
    return "";
}

size_t SoapySDR::Device::getNumChannels(const Direction) const
{
    return 0;
}

std::vector<std::string> SoapySDR::Device::listAntennas(const Direction, const size_t) const
{
    return std::vector<std::string>();
}

void SoapySDR::Device::setAntenna(const Direction, const size_t, const std::string &)
{
    return;
}

std::string SoapySDR::Device::getAntenna(const Direction, const size_t) const
{
    return "";
}

void SoapySDR::Device::setDCOffset(const Direction, const size_t, const std::complex<double> &)
{
    return;
}

std::complex<double> SoapySDR::Device::getDCOffset(const Direction, const size_t) const
{
    return std::complex<double>();
}

void SoapySDR::Device::setIQBalance(const Direction, const size_t, const std::complex<double> &)
{
    return;
}

std::complex<double> SoapySDR::Device::getIQBalance(const Direction, const size_t) const
{
    return std::complex<double>();
}

std::vector<std::string> SoapySDR::Device::listGains(const Direction, const size_t) const
{
    return std::vector<std::string>();
}

void SoapySDR::Device::setGainMode(const Direction, const size_t, const bool)
{
    return;
}

bool SoapySDR::Device::getGainMode(const Direction, const size_t) const
{
    return false;
}

void SoapySDR::Device::setGain(const Direction, const size_t, const double)
{
    return;
}

void SoapySDR::Device::setGain(const Direction, const size_t, const std::string &, const double)
{
    return;
}

double SoapySDR::Device::getGain(const Direction dir, const size_t channel) const
{
    double gain = 0.0;
    std::vector<std::string> names = this->listGains(dir, channel);
    for (size_t i = 0; i < names.size(); i++)
    {
        gain =+ this->getGain(dir, channel, names[i]);
    }
    return gain;
}

double SoapySDR::Device::getGain(const Direction, const size_t, const std::string &) const
{
    return 0.0;
}

SoapySDR::Range SoapySDR::Device::getGainRange(const Direction, const size_t, const std::string &) const
{
    return SoapySDR::Range(0.0, 0.0);
}

SoapySDR::Range SoapySDR::Device::getGainRange(const Direction, const size_t) const
{
    return SoapySDR::Range(0.0, 0.0);
}

void SoapySDR::Device::setFrequency(const Direction, const size_t, const double, const Kwargs &)
{
    return;
}

double SoapySDR::Device::getFrequency(const Direction, const size_t) const
{
    return 0.0;
}

double SoapySDR::Device::getFrequency(const Direction, const size_t, const std::string &) const
{
    return 0.0;
}

std::vector<std::string> SoapySDR::Device::listFrequencies(const Direction, const size_t) const
{
    return std::vector<std::string>();
}

SoapySDR::RangeList SoapySDR::Device::getFrequencyRange(const Direction, const size_t) const
{
    return SoapySDR::RangeList();
}

void SoapySDR::Device::setSampleRate(const Direction, const size_t, const double)
{
    return;
}

double SoapySDR::Device::getSampleRate(const Direction, const size_t) const
{
    return 0.0;
}

std::vector<double> SoapySDR::Device::listSampleRates(const Direction, const size_t) const
{
    return std::vector<double>();
}

void SoapySDR::Device::setBandwidth(const Direction, const size_t, const double)
{
    return;
}

double SoapySDR::Device::getBandwidth(const Direction, const size_t) const
{
    return 0.0;
}

std::vector<double> SoapySDR::Device::listBandwidths(const Direction, const size_t) const
{
    return std::vector<double>();
}

void SoapySDR::Device::setMasterClockRate(const double)
{
    return;
}

double SoapySDR::Device::setMasterClockRate(void) const
{
    return 0.0;
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

void *SoapySDR::Device::setupStream(const Direction, const std::vector<size_t> &, const Kwargs &)
{
    return NULL;
}

void SoapySDR::Device::closeStream(void *)
{
    return;
}

int SoapySDR::Device::readStream(void *, void * const *, const size_t, int &, long long &, const long)
{
    return -1;
}

int SoapySDR::Device::writeStream(void *, const void * const *, const size_t, int &, const long long, const long)
{
    return -1;
}

long long SoapySDR::Device::getHardwareTime(const std::string &) const
{
    return 0;
}

void SoapySDR::Device::setHardwareTime(const long long, const std::string &)
{
    return;
}

void SoapySDR::Device::setCommandTime(const long long, const std::string &)
{
    return;
}
