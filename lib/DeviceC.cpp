// Copyright (c) 2014-2014 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Device.h>
#include <SoapySDR/Device.hpp>
#include <stdlib.h>
#include <string.h>

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

extern "C" {

/*******************************************************************
 * Channels API
 ******************************************************************/
void SoapySDRDevice_setFrontendMapping(SoapySDRDevice *device, const SoapySDRDirection dir, const char *mapping)
{
    reinterpret_cast<SoapySDR::Device *>(device)->setFrontendMapping(dir, mapping);
}

char *SoapySDRDevice_getFrontendMapping(const SoapySDRDevice *device, const SoapySDRDirection dir)
{
    return strdup(reinterpret_cast<const SoapySDR::Device *>(device)->getFrontendMapping(dir).c_str());
}

size_t SoapySDRDevice_getNumChannels(const SoapySDRDevice *device, const SoapySDRDirection dir)
{
    return reinterpret_cast<const SoapySDR::Device *>(device)->getNumChannels(dir);
}

/*******************************************************************
 * Stream API
 ******************************************************************/
void *SoapySDRDevice_setupStream(SoapySDRDevice *device, const SoapySDRDirection dir, const size_t *channels, const size_t numChans, const SoapySDRKwargs *args)
{
    return reinterpret_cast<SoapySDR::Device *>(device)->setupStream(dir, std::vector<size_t>(channels, channels+numChans), toKwargs(args));
}

void SoapySDRDevice_closeStream(SoapySDRDevice *device, void *handle)
{
    return reinterpret_cast<SoapySDR::Device *>(device)->closeStream(handle);
}

int SoapySDRDevice_readStream(SoapySDRDevice *device, void *handle, void * const *buffs, const size_t numElems, int *flags, long long *timeNs, const long timeoutUs)
{
    return reinterpret_cast<SoapySDR::Device *>(device)->readStream(handle, buffs, numElems, *flags, *timeNs, timeoutUs);
}

int SoapySDRDevice_writeStream(SoapySDRDevice *device, void *handle, const void * const *buffs, const size_t numElems, int *flags, const long long timeNs, const long timeoutUs)
{
    return reinterpret_cast<SoapySDR::Device *>(device)->writeStream(handle, buffs, numElems, *flags, timeNs, timeoutUs);
}

/*******************************************************************
 * Antenna API
 ******************************************************************/
char **SoapySDRDevice_listAntennas(const SoapySDRDevice *device, const SoapySDRDirection dir, const size_t channel, size_t *length)
{
    return toStrArray(reinterpret_cast<const SoapySDR::Device *>(device)->listAntennas(dir, channel), length);
}

void SoapySDRDevice_setAntenna(SoapySDRDevice *device, const SoapySDRDirection dir, const size_t channel, const char *name)
{
    reinterpret_cast<SoapySDR::Device *>(device)->setAntenna(dir, channel, name);
}

char *SoapySDRDevice_getAntenna(const SoapySDRDevice *device, const SoapySDRDirection dir, const size_t channel)
{
    return strdup(reinterpret_cast<const SoapySDR::Device *>(device)->getAntenna(dir, channel).c_str());
}

/*******************************************************************
 * Frontend corrections API
 ******************************************************************/
void SoapySDRDevice_setDCOffset(SoapySDRDevice *device, const SoapySDRDirection dir, const size_t channel, const double offsetI, const double offsetQ)
{
    reinterpret_cast<SoapySDR::Device *>(device)->setDCOffset(dir, channel, std::complex<double>(offsetI, offsetQ));
}

void SoapySDRDevice_getDCOffset(const SoapySDRDevice *device, const SoapySDRDirection dir, const size_t channel, double *offsetI, double *offsetQ)
{
    std::complex<double> ret = reinterpret_cast<const SoapySDR::Device *>(device)->getDCOffset(dir, channel);
    *offsetI = ret.real();
    *offsetQ = ret.imag();
}

void SoapySDRDevice_setIQBalance(SoapySDRDevice *device, const SoapySDRDirection dir, const size_t channel, const double balanceI, const double balanceQ)
{
    reinterpret_cast<SoapySDR::Device *>(device)->setDCOffset(dir, channel, std::complex<double>(balanceI, balanceQ));
}

void SoapySDRDevice_getIQBalance(const SoapySDRDevice *device, const SoapySDRDirection dir, const size_t channel, double *balanceI, double *balanceQ)
{
    std::complex<double> ret = reinterpret_cast<const SoapySDR::Device *>(device)->getIQBalance(dir, channel);
    *balanceI = ret.real();
    *balanceQ = ret.imag();
}


/*******************************************************************
 * Gain API
 ******************************************************************/
char **SoapySDRDevice_listGains(const SoapySDRDevice *device, const SoapySDRDirection dir, const size_t channel, size_t *length)
{
    return toStrArray(reinterpret_cast<const SoapySDR::Device *>(device)->listGains(dir, channel), length);
}

void SoapySDRDevice_setGainMode(SoapySDRDevice *device, const SoapySDRDirection dir, const size_t channel, const bool mode)
{
    reinterpret_cast<SoapySDR::Device *>(device)->setGainMode(dir, channel, mode);
}

bool SoapySDRDevice_getGainMode(const SoapySDRDevice *device, const SoapySDRDirection dir, const size_t channel)
{
    return reinterpret_cast<const SoapySDR::Device *>(device)->getGainMode(dir, channel);
}

void SoapySDRDevice_setGain(SoapySDRDevice *device, const SoapySDRDirection dir, const size_t channel, const double value)
{
    reinterpret_cast<SoapySDR::Device *>(device)->setGain(dir, channel, value);
}

void SoapySDRDevice_setGainElement(SoapySDRDevice *device, const SoapySDRDirection dir, const size_t channel, const char *name, const double value)
{
    reinterpret_cast<SoapySDR::Device *>(device)->setGain(dir, channel, name, value);
}

double SoapySDRDevice_getGain(const SoapySDRDevice *device, const SoapySDRDirection dir, const size_t channel)
{
    return reinterpret_cast<const SoapySDR::Device *>(device)->getGain(dir, channel);
}

double SoapySDRDevice_getGainElement(const SoapySDRDevice *device, const SoapySDRDirection dir, const size_t channel, const char *name)
{
    return reinterpret_cast<const SoapySDR::Device *>(device)->getGain(dir, channel, name);
}

SoapySDRRange SoapySDRDevice_getGainRange(const SoapySDRDevice *device, const SoapySDRDirection dir, const size_t channel)
{
    return toRange(reinterpret_cast<const SoapySDR::Device *>(device)->getGainRange(dir, channel));
}

SoapySDRRange SoapySDRDevice_getGainElementRange(const SoapySDRDevice *device, const SoapySDRDirection dir, const size_t channel, const char *name)
{
    return toRange(reinterpret_cast<const SoapySDR::Device *>(device)->getGainRange(dir, channel, name));
}

/*******************************************************************
 * Frequency API
 ******************************************************************/
void SoapySDRDevice_setFrequency(SoapySDRDevice *device, const SoapySDRDirection dir, const size_t channel, const double frequency, const SoapySDRKwargs *args)
{
    return reinterpret_cast<SoapySDR::Device *>(device)->setFrequency(dir, channel, frequency, toKwargs(args));
}

double SoapySDRDevice_getFrequency(const SoapySDRDevice *device, const SoapySDRDirection dir, const size_t channel)
{
    return reinterpret_cast<const SoapySDR::Device *>(device)->getFrequency(dir, channel);
}

double SoapySDRDevice_getFrequencyComponent(const SoapySDRDevice *device, const SoapySDRDirection dir, const size_t channel, const char *name)
{
    return reinterpret_cast<const SoapySDR::Device *>(device)->getFrequency(dir, channel, name);
}

char **SoapySDRDevice_listFrequencies(const SoapySDRDevice *device, const SoapySDRDirection dir, const size_t channel, size_t *length)
{
    return toStrArray(reinterpret_cast<const SoapySDR::Device *>(device)->listFrequencies(dir, channel), length);
}

SoapySDRRange *SoapySDRDevice_getFrequencyRange(const SoapySDRDevice *device, const SoapySDRDirection dir, const size_t channel, size_t *length)
{
    return toRangeList(reinterpret_cast<const SoapySDR::Device *>(device)->getFrequencyRange(dir, channel), length);
}

/*******************************************************************
 * Sample Rate API
 ******************************************************************/
void SoapySDRDevice_setSampleRate(SoapySDRDevice *device, const SoapySDRDirection dir, const size_t channel, const double rate)
{
    return reinterpret_cast<SoapySDR::Device *>(device)->setSampleRate(dir, channel, rate);
}

double SoapySDRDevice_getSampleRate(const SoapySDRDevice *device, const SoapySDRDirection dir, const size_t channel)
{
    return reinterpret_cast<const SoapySDR::Device *>(device)->getSampleRate(dir, channel);
}

double *SoapySDRDevice_listSampleRates(const SoapySDRDevice *device, const SoapySDRDirection dir, const size_t channel, size_t *length)
{
    return toNumericList(reinterpret_cast<const SoapySDR::Device *>(device)->listSampleRates(dir, channel), length);
}

void SoapySDRDevice_setBandwidth(SoapySDRDevice *device, const SoapySDRDirection dir, const size_t channel, const double bw)
{
    return reinterpret_cast<SoapySDR::Device *>(device)->setBandwidth(dir, channel, bw);
}

double SoapySDRDevice_getBandwidth(const SoapySDRDevice *device, const SoapySDRDirection dir, const size_t channel)
{
    return reinterpret_cast<const SoapySDR::Device *>(device)->getBandwidth(dir, channel);
}

double *SoapySDRDevice_listBandwidths(const SoapySDRDevice *device, const SoapySDRDirection dir, const size_t channel, size_t *length)
{
    return toNumericList(reinterpret_cast<const SoapySDR::Device *>(device)->listBandwidths(dir, channel), length);
}

/*******************************************************************
 * Clocking API
 ******************************************************************/
void SoapySDRDevice_setMasterClockRate(SoapySDRDevice *device, const double rate)
{
    return reinterpret_cast<SoapySDR::Device *>(device)->setMasterClockRate(rate);
}

double SoapySDRDevice_getMasterClockRate(const SoapySDRDevice *device)
{
    return reinterpret_cast<const SoapySDR::Device *>(device)->getMasterClockRate();
}

char **SoapySDRDevice_listClockSources(const SoapySDRDevice *device, size_t *length)
{
    return toStrArray(reinterpret_cast<const SoapySDR::Device *>(device)->listClockSources(), length);
}

void SoapySDRDevice_setClockSource(SoapySDRDevice *device, const char *source)
{
    reinterpret_cast<SoapySDR::Device *>(device)->setClockSource(source);
}

char *SoapySDRDevice_getClockSource(const SoapySDRDevice *device)
{
    return strdup(reinterpret_cast<const SoapySDR::Device *>(device)->getClockSource().c_str());
}

char **SoapySDRDevice_listTimeSources(const SoapySDRDevice *device, size_t *length)
{
    return toStrArray(reinterpret_cast<const SoapySDR::Device *>(device)->listTimeSources(), length);
}

void SoapySDRDevice_setTimeSource(SoapySDRDevice *device, const char *source)
{
    reinterpret_cast<SoapySDR::Device *>(device)->setTimeSource(source);
}

char *SoapySDRDevice_getTimeSource(const SoapySDRDevice *device)
{
    return strdup(reinterpret_cast<const SoapySDR::Device *>(device)->getTimeSource().c_str());
}

/*******************************************************************
 * Time API
 ******************************************************************/
long long SoapySDRDevice_getHardwareTime(const SoapySDRDevice *device, const char *what)
{
    return reinterpret_cast<const SoapySDR::Device *>(device)->getHardwareTime(what);
}

void SoapySDRDevice_setHardwareTime(SoapySDRDevice *device, const long long timeNs, const char *what)
{
    reinterpret_cast<SoapySDR::Device *>(device)->setHardwareTime(timeNs, what);
}

void SoapySDRDevice_setCommandTime(SoapySDRDevice *device, const long long timeNs, const char *what)
{
    reinterpret_cast<SoapySDR::Device *>(device)->setCommandTime(timeNs, what);
}

}
