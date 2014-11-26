// Copyright (c) 2014-2014 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Device.hpp>
#include <SoapySDR/Registry.hpp>
#include <SoapySDR/Logger.hpp>

class NullDevice : public SoapySDR::Device
{
public:
    NullDevice(void)
    {
        SoapySDR::logf(SOAPY_SDR_ERROR, "This is a test %s log message - %d", "error", 0);
        SoapySDR::logf(SOAPY_SDR_WARNING, "This is a test %s log message - %d", "warning", 1);
        SoapySDR::logf(SOAPY_SDR_INFO, "This is a test %s log message - %d", "information", 2);
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

std::vector<SoapySDR::Kwargs> findNullDevice(const SoapySDR::Kwargs &args)
{
    std::vector<SoapySDR::Kwargs> results;

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
