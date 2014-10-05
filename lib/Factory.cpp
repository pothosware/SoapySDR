// Copyright (c) 2014-2014 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Device.hpp>
#include <SoapySDR/Registry.hpp>
#include <stdexcept>
#include <iostream>

static std::map<SoapySDR::Kwargs, SoapySDR::Device *> &getDeviceTable(void)
{
    static std::map<SoapySDR::Kwargs, SoapySDR::Device *> table;
    return table;
}

static std::map<SoapySDR::Device *, size_t> &getDeviceCounts(void)
{
    static std::map<SoapySDR::Device *, size_t> table;
    return table;
}

std::vector<SoapySDR::Kwargs> SoapySDR::Device::enumerate(const Kwargs &args)
{
    std::vector<SoapySDR::Kwargs> results;
    std::map<std::string, Registry::FindFunction> findFunctions = Registry::listFindFunctions();
    for (std::map<std::string, Registry::FindFunction>::iterator it = findFunctions.begin(); it != findFunctions.end(); ++it)
    {
        if (args.count("module") != 0 and args.at("module") != it->first) continue;
        try
        {
            std::vector<SoapySDR::Kwargs> results0 = it->second(args);
            for (size_t i = 0; i < results0.size(); i++)
            {
                results0[i]["module"] = it->first;
                results.push_back(results0[i]);
            }
        }
        catch (const std::exception &ex)
        {
            std::cerr << "SoapySDR::Device::enumerate(" << it->first << ") " << ex.what() << std::endl;
        }
        catch (...)
        {
            std::cerr << "SoapySDR::Device::enumerate(" << it->first << ") " << "unknown error" << std::endl;
        }
    }
    return results;
}

SoapySDR::Device* SoapySDR::Device::make(const Kwargs &args_)
{
    Kwargs args = args_;
    Device *device = NULL;

    //check the device table for an already allocated device
    if (getDeviceTable().count(args_) != 0 and getDeviceCounts().count(getDeviceTable().at(args_)) != 0)
    {
        device = getDeviceTable().at(args_);
    }

    //otherwise call into one of the factory functions
    else
    {
        //no module specified, perform an enumeration and take first result
        if (args.count("module") == 0)
        {
            std::vector<SoapySDR::Kwargs> results = Device::enumerate(args);
            if (not results.empty()) args = results.front();
        }

        //loop through make functions and call on module match
        std::map<std::string, Registry::MakeFunction> makeFunctions = Registry::listMakeFunctions();
        for (std::map<std::string, Registry::MakeFunction>::iterator it = makeFunctions.begin(); it != makeFunctions.end(); ++it)
        {
            if (args.count("module") != 0 and args.at("module") != it->first) continue;
            device = it->second(args);
            break;
        }
    }

    if (device == NULL) throw std::runtime_error("SoapySDR::Device::make() no match");

    //store into the table
    getDeviceTable()[args_] = device;
    getDeviceCounts()[device]++;

    return device;
}

void SoapySDR::Device::unmake(Device *device)
{
    if (getDeviceCounts().count(device) == 0)
    {
        throw std::runtime_error("SoapySDR::Device::unmake() unknown device");
    }

    getDeviceCounts()[device]--;
    if (getDeviceCounts()[device] == 0)
    {
        getDeviceCounts().erase(device);
        delete device;

        //cleanup the argument to device table
        for (std::map<SoapySDR::Kwargs, SoapySDR::Device *>::iterator it = getDeviceTable().begin(); it != getDeviceTable().end(); ++it)
        {
            if (it->second == device)
            {
                getDeviceTable().erase(it);
                return;
            }
        }
    }
}
