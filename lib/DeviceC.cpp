// Copyright (c) 2014-2014 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Device.h>
#include <SoapySDR/Device.hpp>
#include <stdlib.h>
#include <string.h>

/*******************************************************************
 * Simple subclass definition for device
 ******************************************************************/
struct SoapySDRDevice : SoapySDR::Device {};

/*******************************************************************
 * Helpful converters
 ******************************************************************/
static char **toStrArray(const std::vector<std::string> &strs, size_t *length)
{
    char **out = (char **)calloc(strs.size(), sizeof(char *));
    for (size_t i = 0; i < strs.size(); i++)
    {
        out[i] = strdup(strs[i].c_str());
    }
    *length = strs.size();
    return out;
}

static SoapySDRRange toRange(const SoapySDR::Range &range)
{
    SoapySDRRange out;
    out.minimum = range.minimum();
    out.maximum = range.maximum();
    return out;
}

static SoapySDRRange *toRangeList(const std::vector<SoapySDR::Range> &ranges, size_t *length)
{
    SoapySDRRange *out = (SoapySDRRange *)calloc(ranges.size(), sizeof(SoapySDRRange));
    for (size_t i = 0; i < ranges.size(); i++) out[i] = toRange(ranges[i]);
    *length = ranges.size();
    return out;
}

static double *toNumericList(const std::vector<double> &values, size_t *length)
{
    double *out = (double *)calloc(values.size(), sizeof(double));
    for (size_t i = 0; i < values.size(); i++) out[i] = values[i];
    *length = values.size();
    return out;
}

SoapySDR::Kwargs toKwargs(const SoapySDRKwargs *args)
{
    SoapySDR::Kwargs out;
    for (size_t i = 0; i < args->size; i++)
    {
        out[args->keys[i]] = args->vals[i];
    }
    return out;
}

SoapySDRKwargs tokwargs(const SoapySDR::Kwargs &args)
{
    SoapySDRKwargs out;
    for (SoapySDR::Kwargs::const_iterator it = args.begin(); it != args.end(); ++it)
    {
        SoapySDRKwargs_set(&out, it->first.c_str(), it->second.c_str());
    }
    return out;
}

extern "C" {

/*******************************************************************
 * Identification API
 ******************************************************************/
char *SoapySDRDevice_getDriverKey(const SoapySDRDevice *device)
{
    return strdup(device->getDriverKey().c_str());
}

char *SoapySDRDevice_getHardwareKey(const SoapySDRDevice *device)
{
    return strdup(device->getHardwareKey().c_str());
}

SoapySDRKwargs SoapySDRDevice_getHardwareInfo(const SoapySDRDevice *device)
{
    return tokwargs(device->getHardwareInfo());
}

/*******************************************************************
 * Channels API
 ******************************************************************/
void SoapySDRDevice_setFrontendMapping(SoapySDRDevice *device, const int direction, const char *mapping)
{
    device->setFrontendMapping(direction, mapping);
}

char *SoapySDRDevice_getFrontendMapping(const SoapySDRDevice *device, const int direction)
{
    return strdup(device->getFrontendMapping(direction).c_str());
}

size_t SoapySDRDevice_getNumChannels(const SoapySDRDevice *device, const int direction)
{
    return device->getNumChannels(direction);
}

/*******************************************************************
 * Stream API
 ******************************************************************/
SoapySDRStream *SoapySDRDevice_setupStream(SoapySDRDevice *device, const int direction, const char *format, const size_t *channels, const size_t numChans, const SoapySDRKwargs *args, char **errorMsg)
{
    try
    {
        return reinterpret_cast<SoapySDRStream *>(device->setupStream(direction, format, std::vector<size_t>(channels, channels+numChans), toKwargs(args)));
    }
    catch (const std::exception &ex)
    {
        *errorMsg = strdup(ex.what());
    }
    catch (...)
    {
        *errorMsg = strdup("unknown");
    }
    return NULL;
}

void SoapySDRDevice_closeStream(SoapySDRDevice *device, SoapySDRStream *stream)
{
    return device->closeStream(reinterpret_cast<SoapySDR::Stream *>(stream));
}

int SoapySDRDevice_activateStream(SoapySDRDevice *device,
    SoapySDRStream *stream,
    const int flags,
    const long long timeNs,
    const size_t numElems)
{
    return device->activateStream(reinterpret_cast<SoapySDR::Stream *>(stream), flags, timeNs, numElems);
}

int SoapySDRDevice_deactivateStream(SoapySDRDevice *device,
    SoapySDRStream *stream,
    const int flags,
    const long long timeNs)
{
    return device->deactivateStream(reinterpret_cast<SoapySDR::Stream *>(stream), flags, timeNs);
}

int SoapySDRDevice_readStream(SoapySDRDevice *device, SoapySDRStream *stream, void * const *buffs, const size_t numElems, int *flags, long long *timeNs, const long timeoutUs)
{
    return device->readStream(reinterpret_cast<SoapySDR::Stream *>(stream), buffs, numElems, *flags, *timeNs, timeoutUs);
}

int SoapySDRDevice_writeStream(SoapySDRDevice *device, SoapySDRStream *stream, const void * const *buffs, const size_t numElems, int *flags, const long long timeNs, const long timeoutUs)
{
    return device->writeStream(reinterpret_cast<SoapySDR::Stream *>(stream), buffs, numElems, *flags, timeNs, timeoutUs);
}

/*******************************************************************
 * Antenna API
 ******************************************************************/
char **SoapySDRDevice_listAntennas(const SoapySDRDevice *device, const int direction, const size_t channel, size_t *length)
{
    return toStrArray(device->listAntennas(direction, channel), length);
}

void SoapySDRDevice_setAntenna(SoapySDRDevice *device, const int direction, const size_t channel, const char *name)
{
    device->setAntenna(direction, channel, name);
}

char *SoapySDRDevice_getAntenna(const SoapySDRDevice *device, const int direction, const size_t channel)
{
    return strdup(device->getAntenna(direction, channel).c_str());
}

/*******************************************************************
 * Frontend corrections API
 ******************************************************************/
void SoapySDRDevice_setDCOffset(SoapySDRDevice *device, const int direction, const size_t channel, const double offsetI, const double offsetQ)
{
    device->setDCOffset(direction, channel, std::complex<double>(offsetI, offsetQ));
}

void SoapySDRDevice_getDCOffset(const SoapySDRDevice *device, const int direction, const size_t channel, double *offsetI, double *offsetQ)
{
    std::complex<double> ret = device->getDCOffset(direction, channel);
    *offsetI = ret.real();
    *offsetQ = ret.imag();
}

void SoapySDRDevice_setIQBalance(SoapySDRDevice *device, const int direction, const size_t channel, const double balanceI, const double balanceQ)
{
    device->setDCOffset(direction, channel, std::complex<double>(balanceI, balanceQ));
}

void SoapySDRDevice_getIQBalance(const SoapySDRDevice *device, const int direction, const size_t channel, double *balanceI, double *balanceQ)
{
    std::complex<double> ret = device->getIQBalance(direction, channel);
    *balanceI = ret.real();
    *balanceQ = ret.imag();
}

/*******************************************************************
 * Gain API
 ******************************************************************/
char **SoapySDRDevice_listGains(const SoapySDRDevice *device, const int direction, const size_t channel, size_t *length)
{
    return toStrArray(device->listGains(direction, channel), length);
}

void SoapySDRDevice_setGainMode(SoapySDRDevice *device, const int direction, const size_t channel, const bool mode)
{
    device->setGainMode(direction, channel, mode);
}

bool SoapySDRDevice_getGainMode(const SoapySDRDevice *device, const int direction, const size_t channel)
{
    return device->getGainMode(direction, channel);
}

void SoapySDRDevice_setGain(SoapySDRDevice *device, const int direction, const size_t channel, const double value)
{
    device->setGain(direction, channel, value);
}

void SoapySDRDevice_setGainElement(SoapySDRDevice *device, const int direction, const size_t channel, const char *name, const double value)
{
    device->setGain(direction, channel, name, value);
}

double SoapySDRDevice_getGain(const SoapySDRDevice *device, const int direction, const size_t channel)
{
    return device->getGain(direction, channel);
}

double SoapySDRDevice_getGainElement(const SoapySDRDevice *device, const int direction, const size_t channel, const char *name)
{
    return device->getGain(direction, channel, name);
}

SoapySDRRange SoapySDRDevice_getGainRange(const SoapySDRDevice *device, const int direction, const size_t channel)
{
    return toRange(device->getGainRange(direction, channel));
}

SoapySDRRange SoapySDRDevice_getGainElementRange(const SoapySDRDevice *device, const int direction, const size_t channel, const char *name)
{
    return toRange(device->getGainRange(direction, channel, name));
}

/*******************************************************************
 * Frequency API
 ******************************************************************/
void SoapySDRDevice_setFrequency(SoapySDRDevice *device, const int direction, const size_t channel, const double frequency, const SoapySDRKwargs *args)
{
    return device->setFrequency(direction, channel, frequency, toKwargs(args));
}

double SoapySDRDevice_getFrequency(const SoapySDRDevice *device, const int direction, const size_t channel)
{
    return device->getFrequency(direction, channel);
}

double SoapySDRDevice_getFrequencyComponent(const SoapySDRDevice *device, const int direction, const size_t channel, const char *name)
{
    return device->getFrequency(direction, channel, name);
}

char **SoapySDRDevice_listFrequencies(const SoapySDRDevice *device, const int direction, const size_t channel, size_t *length)
{
    return toStrArray(device->listFrequencies(direction, channel), length);
}

SoapySDRRange *SoapySDRDevice_getFrequencyRange(const SoapySDRDevice *device, const int direction, const size_t channel, size_t *length)
{
    return toRangeList(device->getFrequencyRange(direction, channel), length);
}

/*******************************************************************
 * Sample Rate API
 ******************************************************************/
void SoapySDRDevice_setSampleRate(SoapySDRDevice *device, const int direction, const size_t channel, const double rate)
{
    return device->setSampleRate(direction, channel, rate);
}

double SoapySDRDevice_getSampleRate(const SoapySDRDevice *device, const int direction, const size_t channel)
{
    return device->getSampleRate(direction, channel);
}

double *SoapySDRDevice_listSampleRates(const SoapySDRDevice *device, const int direction, const size_t channel, size_t *length)
{
    return toNumericList(device->listSampleRates(direction, channel), length);
}

void SoapySDRDevice_setBandwidth(SoapySDRDevice *device, const int direction, const size_t channel, const double bw)
{
    return device->setBandwidth(direction, channel, bw);
}

double SoapySDRDevice_getBandwidth(const SoapySDRDevice *device, const int direction, const size_t channel)
{
    return device->getBandwidth(direction, channel);
}

double *SoapySDRDevice_listBandwidths(const SoapySDRDevice *device, const int direction, const size_t channel, size_t *length)
{
    return toNumericList(device->listBandwidths(direction, channel), length);
}

/*******************************************************************
 * Clocking API
 ******************************************************************/
void SoapySDRDevice_setMasterClockRate(SoapySDRDevice *device, const double rate)
{
    return device->setMasterClockRate(rate);
}

double SoapySDRDevice_getMasterClockRate(const SoapySDRDevice *device)
{
    return device->getMasterClockRate();
}

char **SoapySDRDevice_listClockSources(const SoapySDRDevice *device, size_t *length)
{
    return toStrArray(device->listClockSources(), length);
}

void SoapySDRDevice_setClockSource(SoapySDRDevice *device, const char *source)
{
    device->setClockSource(source);
}

char *SoapySDRDevice_getClockSource(const SoapySDRDevice *device)
{
    return strdup(device->getClockSource().c_str());
}

char **SoapySDRDevice_listTimeSources(const SoapySDRDevice *device, size_t *length)
{
    return toStrArray(device->listTimeSources(), length);
}

void SoapySDRDevice_setTimeSource(SoapySDRDevice *device, const char *source)
{
    device->setTimeSource(source);
}

char *SoapySDRDevice_getTimeSource(const SoapySDRDevice *device)
{
    return strdup(device->getTimeSource().c_str());
}

/*******************************************************************
 * Time API
 ******************************************************************/
long long SoapySDRDevice_getHardwareTime(const SoapySDRDevice *device, const char *what)
{
    return device->getHardwareTime(what);
}

void SoapySDRDevice_setHardwareTime(SoapySDRDevice *device, const long long timeNs, const char *what)
{
    device->setHardwareTime(timeNs, what);
}

void SoapySDRDevice_setCommandTime(SoapySDRDevice *device, const long long timeNs, const char *what)
{
    device->setCommandTime(timeNs, what);
}

/*******************************************************************
 * Sensor API
 ******************************************************************/
char **SoapySDRDevice_listSensors(const SoapySDRDevice *device, size_t *length)
{
    return toStrArray(device->listSensors(), length);
}

char *SoapySDRDevice_readSensor(const SoapySDRDevice *device, const char *name)
{
    return strdup(device->readSensor(name).c_str());
}

/*******************************************************************
 * Register API
 ******************************************************************/
void SoapySDRDevice_writeRegister(SoapySDRDevice *device, const unsigned addr, const unsigned value)
{
    return device->writeRegister(addr, value);
}

unsigned SoapySDRDevice_readRegister(const SoapySDRDevice *device, const unsigned addr)
{
    return device->readRegister(addr);
}

/*******************************************************************
 * GPIO API
 ******************************************************************/
char **SoapySDRDevice_listGPIOBanks(const SoapySDRDevice *device, size_t *length)
{
    return toStrArray(device->listGPIOBanks(), length);
}

void SoapySDRDevice_writeGPIO(SoapySDRDevice *device, const char *bank, const unsigned value)
{
    return device->writeGPIO(bank, value);
}

unsigned SoapySDRDevice_readGPIO(const SoapySDRDevice *device, const char *bank)
{
    return device->readGPIO(bank);
}

/*******************************************************************
 * I2C API
 ******************************************************************/
void SoapySDRDevice_writeI2C(SoapySDRDevice *device, const int addr, const char *data, const size_t numBytes)
{
    return device->writeI2C(addr, std::string(data, numBytes));
}

char *SoapySDRDevice_readI2C(SoapySDRDevice *device, const int addr, const size_t numBytes)
{
    return strndup(device->readI2C(addr, numBytes).c_str(), numBytes);
}

/*******************************************************************
 * SPI API
 ******************************************************************/
unsigned SoapySDRDevice_transactSPI(SoapySDRDevice *device, const int addr, const unsigned data, const size_t numBits)
{
    return device->transactSPI(addr, data, numBits);
}

/*******************************************************************
 * UART API
 ******************************************************************/
char **SoapySDRDevice_listUARTs(const SoapySDRDevice *device, size_t *length)
{
    return toStrArray(device->listUARTs(), length);
}

void SoapySDRDevice_writeUART(SoapySDRDevice *device, const char *which, const char *data)
{
    return device->writeUART(which, data);
}

char *SoapySDRDevice_readUART(const SoapySDRDevice *device, const char *which, const long timeoutUs)
{
    return strdup(device->readUART(which, timeoutUs).c_str());
}

} //extern "C"
