// Copyright (c) 2014-2014 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Device.h>
#include <SoapySDR/Device.hpp>
#include <stdlib.h>
#include <string.h>

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
    //convert to map type
    SoapySDR::Kwargs inArgs;
    for (size_t i = 0; i < args->size; i++)
    {
        inArgs[args->keys[i]] = args->vals[i];
    }

    return reinterpret_cast<SoapySDR::Device *>(device)->setupStream(dir, std::vector<size_t>(channels, channels+numChans), inArgs);
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

}
