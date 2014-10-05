// Copyright (c) 2014-2014 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Device.hpp>

SoapySDR::Device::~Device(void)
{
    return;
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

void SoapySDR::Device::setGain(const Direction, const size_t, const std::string &, const double)
{
    return;
}

void SoapySDR::Device::setGains(const Direction, const size_t, const NumericDict &)
{
    return;
}

double SoapySDR::Device::getGainValue(const Direction, const size_t, const std::string &) const
{
    return 0.0;
}

SoapySDR::NumericDict SoapySDR::Device::getGainValues(const Direction, const size_t) const
{
    return SoapySDR::NumericDict();
}

SoapySDR::RangeList SoapySDR::Device::getGainRange(const Direction, const size_t, const std::string &) const
{
    return SoapySDR::RangeList();
}

void SoapySDR::Device::setFrequency(const Direction, const size_t, const double, const Kwargs &)
{
    return;
}

void SoapySDR::Device::setFrequency(const Direction, const size_t, const NumericDict &, const Kwargs &)
{
    return;
}

double SoapySDR::Device::getFrequency(const Direction, const size_t) const
{
    return 0.0;
}

SoapySDR::NumericDict SoapySDR::Device::getFrequencyComponents(const Direction, const size_t) const
{
    return SoapySDR::NumericDict();
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

SoapySDR::RangeList SoapySDR::Device::getSampleRateRange(const Direction, const size_t) const
{
    return SoapySDR::RangeList();
}

void SoapySDR::Device::setBandwidth(const Direction, const size_t, const double)
{
    return;
}

double SoapySDR::Device::getBandwidth(const Direction, const size_t) const
{
    return 0.0;
}

SoapySDR::RangeList SoapySDR::Device::getBandwidthRange(const Direction, const size_t) const
{
    return SoapySDR::RangeList();
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
