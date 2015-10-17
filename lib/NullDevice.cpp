// Copyright (c) 2014-2015 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Device.hpp>
#include <SoapySDR/Registry.hpp>

class NullDevice : public SoapySDR::Device
{
public:
    NullDevice(void)
    {
        return;
    }

    std::string getDriverKey(void) const
    {
        return "null";
    }

    std::string getHardwareKey(void) const
    {
        return "null";
    }
};

SoapySDR::KwargsList findNullDevice(const SoapySDR::Kwargs &args)
{
    SoapySDR::KwargsList results;

    //require that the user specify type=null
    if (args.count("type") == 0) return results;
    if (args.at("type") != "null") return results;

    SoapySDR::Kwargs nullArgs;
    nullArgs["type"] = "null";
    results.push_back(nullArgs);

    return results;
}

SoapySDR::Device *makeNullDevice(const SoapySDR::Kwargs &)
{
    return new NullDevice();
}

static SoapySDR::Registry registerNullDevice("null", &findNullDevice, &makeNullDevice, SOAPY_SDR_ABI_VERSION);
