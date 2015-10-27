// Copyright (c) 2014-2015 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include "TypeHelpers.hpp"
#include <SoapySDR/Device.h>
#include <SoapySDR/Device.hpp>
#include <cstdlib>
#include <cstring>

extern "C" {

SoapySDRKwargs *SoapySDRDevice_enumerate(const SoapySDRKwargs *args, size_t *length)
{
    return toKwargsList(SoapySDR::Device::enumerate(toKwargs(args)), length);
}

SOAPY_SDR_API SoapySDRKwargs *SoapySDRDevice_enumerateStrArgs(const char *args, size_t *length)
{
    return toKwargsList(SoapySDR::Device::enumerate(args), length);
}

SoapySDRDevice *SoapySDRDevice_make(const SoapySDRKwargs *args)
{
    return (SoapySDRDevice *)SoapySDR::Device::make(toKwargs(args));
}

SoapySDRDevice *SoapySDRDevice_makeStrArgs(const char *args)
{
    return (SoapySDRDevice *)SoapySDR::Device::make(args);
}

void SoapySDRDevice_unmake(SoapySDRDevice *device)
{
    SoapySDR::Device::unmake((SoapySDR::Device *)device);
}

}
