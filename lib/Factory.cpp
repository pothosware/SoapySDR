// Copyright (c) 2014-2016 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Device.hpp>
#include <SoapySDR/Registry.hpp>
#include <SoapySDR/Modules.hpp>
#include <stdexcept>
#include <iostream>
#include <mutex>

static std::recursive_mutex &getFactoryMutex(void)
{
    static std::recursive_mutex mutex;
    return mutex;
}

typedef std::map<SoapySDR::Kwargs, SoapySDR::Device *> DeviceTable;

static DeviceTable &getDeviceTable(void)
{
    static DeviceTable table;
    return table;
}

typedef std::map<SoapySDR::Device *, size_t> DeviceCounts;

static DeviceCounts &getDeviceCounts(void)
{
    static DeviceCounts table;
    return table;
}

SoapySDR::KwargsList SoapySDR::Device::enumerate(const Kwargs &args)
{
    std::lock_guard<std::recursive_mutex> lock(getFactoryMutex());

    loadModules();
    SoapySDR::KwargsList results;
    for (const auto &it : Registry::listFindFunctions())
    {
        if (args.count("driver") != 0 and args.at("driver") != it.first) continue;
        try
        {
            SoapySDR::KwargsList results0 = it.second(args);
            for (size_t i = 0; i < results0.size(); i++)
            {
                results0[i]["driver"] = it.first;
                results.push_back(results0[i]);
            }
        }
        catch (const std::exception &ex)
        {
            std::cerr << "SoapySDR::Device::enumerate(" << it.first << ") " << ex.what() << std::endl;
        }
        catch (...)
        {
            std::cerr << "SoapySDR::Device::enumerate(" << it.first << ") " << "unknown error" << std::endl;
        }
    }
    return results;
}

SoapySDR::KwargsList SoapySDR::Device::enumerate(const std::string &args)
{
    return enumerate(KwargsFromString(args));
}

SoapySDR::Device* SoapySDR::Device::make(const Kwargs &args_)
{
    std::lock_guard<std::recursive_mutex> lock(getFactoryMutex());

    loadModules();
    Kwargs args = args_;
    Device *device = nullptr;

    //check the device table for an already allocated device
    if (getDeviceTable().count(args_) != 0 and getDeviceCounts().count(getDeviceTable().at(args_)) != 0)
    {
        device = getDeviceTable().at(args_);
    }

    //otherwise call into one of the factory functions
    else
    {
        //the args must always come from an enumeration result
        {
            const auto results = Device::enumerate(args);
            if (not results.empty()) args = results.front();
        }

        //load the enumeration args with missing keys from the make argument
        for (const auto &it : args_)
        {
            if (args.count(it.first) == 0) args[it.first] = it.second;
        }

        //loop through make functions and call on module match
        for (const auto &it : Registry::listMakeFunctions())
        {
            if (args.count("driver") != 0 and args.at("driver") != it.first) continue;
            device = it.second(args);
            break;
        }
    }

    if (device == nullptr) throw std::runtime_error("SoapySDR::Device::make() no match");

    //store into the table
    getDeviceTable()[args_] = device;
    getDeviceCounts()[device]++;

    return device;
}


SoapySDR::Device *SoapySDR::Device::make(const std::string &args)
{
    return make(KwargsFromString(args));
}

void SoapySDR::Device::unmake(Device *device)
{
    std::lock_guard<std::recursive_mutex> lock(getFactoryMutex());

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
        for (auto it = getDeviceTable().begin(); it != getDeviceTable().end(); ++it)
        {
            if (it->second == device)
            {
                getDeviceTable().erase(it);
                return;
            }
        }
    }
}
