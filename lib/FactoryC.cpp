// Copyright (c) 2014-2014 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Device.h>
#include <SoapySDR/Device.hpp>
#include <cstdlib>
#include <cstring>

SoapySDR::Kwargs toKwargs(const SoapySDRKwargs *args);
SoapySDRKwargs toKwargs(const SoapySDR::Kwargs &args);
SoapySDRKwargs *toKwargsList(const std::vector<SoapySDR::Kwargs> &args, size_t *length)
{
    SoapySDRKwargs *outArgs = (SoapySDRKwargs *)calloc(args.size(), sizeof(SoapySDRKwargs));
    for (size_t i = 0; i < args.size(); i++) outArgs[i] = toKwargs(args[i]);
    *length = args.size();
    return outArgs;
}

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

void SoapySDRDevice_unmake(SoapySDRDevice *device)
{
    SoapySDR::Device::unmake((SoapySDR::Device *)device);
}

}
