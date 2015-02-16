// Copyright (c) 2014-2014 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Device.hpp>

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

bool SoapySDR::Device::getFullDuplex(const int, const size_t) const
{
    return true;
}

/*******************************************************************
 * Stream API
 ******************************************************************/
SoapySDR::Stream *SoapySDR::Device::setupStream(const int, const std::string &, const std::vector<size_t> &, const Kwargs &)
{
    return NULL;
}

void SoapySDR::Device::closeStream(Stream *)
{
    return;
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

void SoapySDR::Device::setDCOffsetMode(const int, const size_t, const bool)
{
    return;
}

bool SoapySDR::Device::getDCOffsetMode(const int, const size_t) const
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

void SoapySDR::Device::setIQBalance(const int, const size_t, const std::complex<double> &)
{
    return;
}

std::complex<double> SoapySDR::Device::getIQBalance(const int, const size_t) const
{
    return std::complex<double>();
}

/*******************************************************************
 * Gain API
 ******************************************************************/
std::vector<std::string> SoapySDR::Device::listGains(const int, const size_t) const
{
    return std::vector<std::string>();
}

void SoapySDR::Device::setGainMode(const int, const size_t, const bool)
{
    return;
}

bool SoapySDR::Device::getGainMode(const int, const size_t) const
{
    return false;
}

void SoapySDR::Device::setGain(const int, const size_t, const double)
{
    return;
}

void SoapySDR::Device::setGain(const int, const size_t, const std::string &, const double)
{
    return;
}

double SoapySDR::Device::getGain(const int dir, const size_t channel) const
{
    double gain = 0.0;
    std::vector<std::string> names = this->listGains(dir, channel);
    for (size_t i = 0; i < names.size(); i++)
    {
        gain =+ this->getGain(dir, channel, names[i]);
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

SoapySDR::Range SoapySDR::Device::getGainRange(const int, const size_t) const
{
    return SoapySDR::Range(0.0, 0.0);
}

/*******************************************************************
 * Frequency API
 ******************************************************************/
void SoapySDR::Device::setFrequency(const int, const size_t, const double, const Kwargs &)
{
    return;
}

double SoapySDR::Device::getFrequency(const int, const size_t) const
{
    return 0.0;
}

double SoapySDR::Device::getFrequency(const int, const size_t, const std::string &) const
{
    return 0.0;
}

std::vector<std::string> SoapySDR::Device::listFrequencies(const int, const size_t) const
{
    return std::vector<std::string>();
}

SoapySDR::RangeList SoapySDR::Device::getFrequencyRange(const int, const size_t) const
{
    return SoapySDR::RangeList();
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

/*******************************************************************
 * Time API
 ******************************************************************/
bool SoapySDR::Device::hasHardwareTime(const std::string &) const
{
    return false;
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

/*******************************************************************
 * Sensor API
 ******************************************************************/
std::vector<std::string> SoapySDR::Device::listSensors(void) const
{
    return std::vector<std::string>();
}

std::string SoapySDR::Device::readSensor(const std::string &) const
{
    return "";
}

/*******************************************************************
 * Register API
 ******************************************************************/
void SoapySDR::Device::writeRegister(const unsigned, const unsigned)
{
    return;
}

unsigned SoapySDR::Device::readRegister(const unsigned) const
{
    return 0;
}

/*******************************************************************
 * Settings API
 ******************************************************************/
void SoapySDR::Device::writeSetting(const std::string &, const std::string &)
{
    return;
}

std::string SoapySDR::Device::readSetting(const std::string &) const
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

unsigned SoapySDR::Device::readGPIO(const std::string &) const
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
