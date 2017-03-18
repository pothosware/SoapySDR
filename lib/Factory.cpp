// Copyright (c) 2014-2017 Josh Blum
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

static SoapySDR::Device* getDeviceFromTable(const SoapySDR::Kwargs &args)
{
    if (args.empty()) return nullptr;
    std::lock_guard<std::recursive_mutex> lock(getFactoryMutex());
    if (getDeviceTable().count(args) != 0 and getDeviceCounts().count(getDeviceTable().at(args)) != 0)
    {
        auto device = getDeviceTable().at(args);
        getDeviceCounts()[device]++;
        return device;
    }
    return nullptr;
}

SoapySDR::Device* SoapySDR::Device::make(const Kwargs &inputArgs)
{
    Device *device = nullptr;

    //the arguments may have already come from enumerate and been used to open a device
    device = getDeviceFromTable(inputArgs);
    if (device != nullptr) return device;

    //otherwise the args must always come from an enumeration result
    Kwargs discoveredArgs;
    const auto results = Device::enumerate(inputArgs);
    if (not results.empty()) discoveredArgs = results.front();

    //check the device table for an already allocated device
    device = getDeviceFromTable(discoveredArgs);
    if (device != nullptr) return device;

    //load the enumeration args with missing keys from the make argument
    Kwargs hybridArgs = discoveredArgs;
    for (const auto &it : inputArgs)
    {
        if (hybridArgs.count(it.first) == 0) hybridArgs[it.first] = it.second;
    }

    //lock during device construction
    //make itself can be parallelized, but we need to keep track of in-process factories
    //so that other calling threads with the same args can wait on the result
    std::lock_guard<std::recursive_mutex> lock(getFactoryMutex());

    //loop through make functions and call on module match
    for (const auto &it : Registry::listMakeFunctions())
    {
        if (hybridArgs.count("driver") != 0 and hybridArgs.at("driver") != it.first) continue;
        device = it.second(hybridArgs);
        break;
    }
    if (device == nullptr) throw std::runtime_error("SoapySDR::Device::make() no match");

    //store into the table
    getDeviceTable()[discoveredArgs] = device;
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
