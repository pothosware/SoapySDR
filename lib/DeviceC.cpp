// Copyright (c) 2014-2014 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Device.h>
#include <SoapySDR/Device.hpp>
#include <stdlib.h>
#include <string.h>

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

}
