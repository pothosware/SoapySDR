// Copyright (c) 2014-2014 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Interface.hpp>

std::vector<std::string> SoapySDR::SDRDevice::getRxChannels(void) const
{
    return std::vector<std::string>();
}

std::vector<std::string> SoapySDR::SDRDevice::getTxChannels(void) const
{
    return std::vector<std::string>();
}

std::vector<std::string> SoapySDR::SDRDevice::listAntennas(const std::string &) const
{
    return std::vector<std::string>();
}

void SoapySDR::SDRDevice::setAntenna(const std::string &, const std::string &)
{
    return;
}

std::string SoapySDR::SDRDevice::getAntenna(const std::string &) const
{
    return "";
}

void SoapySDR::SDRDevice::setDCOffset(const std::string &, const std::complex<double> &)
{
    return;
}

std::complex<double> SoapySDR::SDRDevice::getDCOffset(const std::string &) const
{
    return std::complex<double>();
}

void SoapySDR::SDRDevice::setIQBalance(const std::string &, const std::complex<double> &)
{
    return;
}

std::complex<double> SoapySDR::SDRDevice::getIQBalance(const std::string &) const
{
    return std::complex<double>();
}

std::vector<std::string> SoapySDR::SDRDevice::listGains(const std::string &) const
{
    return std::vector<std::string>();
}

void SoapySDR::SDRDevice::setGainMode(const std::string &, const bool)
{
    return;
}

bool SoapySDR::SDRDevice::getGainMode(const std::string &channel) const
{
    return false;
}

void SoapySDR::SDRDevice::setGain(const std::string &, const std::string &, const double)
{
    return;
}

void SoapySDR::SDRDevice::setGains(const std::string &, const NumericDict &)
{
    return;
}

double SoapySDR::SDRDevice::getGainValue(const std::string &, const std::string &) const
{
    return 0.0;
}

SoapySDR::NumericDict SoapySDR::SDRDevice::getGainValues(const std::string &) const
{
    return SoapySDR::NumericDict();
}

SoapySDR::RangeList SoapySDR::SDRDevice::getGainRange(const std::string &, const std::string &) const
{
    return SoapySDR::RangeList();
}

void SoapySDR::SDRDevice::setFrequency(const std::string &, const double, const Kwargs &)
{
    return;
}

void SoapySDR::SDRDevice::setFrequency(const std::string &, const NumericDict &, const Kwargs &)
{
    return;
}

double SoapySDR::SDRDevice::getFrequency(const std::string &) const
{
    return 0.0;
}

SoapySDR::NumericDict SoapySDR::SDRDevice::getFrequencyComponents(const std::string &) const
{
    return SoapySDR::NumericDict();
}

SoapySDR::RangeList SoapySDR::SDRDevice::getFrequencyRange(const std::string &) const
{
    return SoapySDR::RangeList();
}

void SoapySDR::SDRDevice::setSampleRate(const std::string &, const double)
{
    return;
}

double SoapySDR::SDRDevice::getSampleRate(const std::string &) const
{
    return 0.0;
}

SoapySDR::RangeList SoapySDR::SDRDevice::getSampleRateRange(const std::string &) const
{
    return SoapySDR::RangeList();
}

void SoapySDR::SDRDevice::setBandwidth(const std::string &, const double)
{
    return;
}

double SoapySDR::SDRDevice::getBandwidth(const std::string &) const
{
    return 0.0;
}

SoapySDR::RangeList SoapySDR::SDRDevice::getBandwidthRange(const std::string &) const
{
    return SoapySDR::RangeList();
}

void SoapySDR::SDRDevice::setMasterClockRate(const double)
{
    return;
}

double SoapySDR::SDRDevice::setMasterClockRate(void) const
{
    return 0.0;
}

std::vector<std::string> SoapySDR::SDRDevice::listClockSources(void) const
{
    return std::vector<std::string>();
}

void SoapySDR::SDRDevice::setClockSource(const std::string &)
{
    return;
}

std::string SoapySDR::SDRDevice::getClockSource(void) const
{
    return "";
}

std::vector<std::string> SoapySDR::SDRDevice::listTimeSources(void) const
{
    return std::vector<std::string>();
}

void SoapySDR::SDRDevice::setTimeSource(const std::string &)
{
    return;
}

std::string SoapySDR::SDRDevice::getTimeSource(void) const
{
    return "";
}
