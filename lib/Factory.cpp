// Copyright (c) 2014-2018 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Device.hpp>
#include <SoapySDR/Registry.hpp>
#include <SoapySDR/Modules.hpp>
#include <SoapySDR/Logger.hpp>
#include <stdexcept>
#include <future>
#include <chrono>
#include <mutex>

static const auto CACHE_TIMEOUT = std::chrono::seconds(1);

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

void automaticLoadModules(void);

SoapySDR::KwargsList SoapySDR::Device::enumerate(const Kwargs &args)
{
    automaticLoadModules(); //perform one-shot load

    //enumerate cache data structure
    //(driver key, find args) -> (timeout, handles list)
    static std::recursive_mutex cacheMutex;
    static std::map<std::pair<std::string, Kwargs>,
        std::pair<std::chrono::high_resolution_clock::time_point, std::shared_future<KwargsList>>
    > cache;

    //clean expired entries from the cache
    {
        const auto now = std::chrono::high_resolution_clock::now();
        std::lock_guard<std::recursive_mutex> lock(cacheMutex);
        for (auto it = cache.begin(); it != cache.end();)
        {
            if (it->second.first < now) cache.erase(it++);
            else it++;
        }
    }

    //launch futures to enumerate devices for each module
    std::map<std::string, std::shared_future<KwargsList>> futures;
    for (const auto &it : Registry::listFindFunctions())
    {
        const bool specifiedDriver = args.count("driver") != 0;
        if (specifiedDriver and args.at("driver") != it.first) continue;

        //protect the cache to search it for results and update it
        std::lock_guard<std::recursive_mutex> lock(cacheMutex);
        auto &cacheEntry = cache[std::make_pair(it.first, args)];

        //use the cache entry if its been initialized (valid) and not expired
        if (cacheEntry.second.valid() and cacheEntry.first > std::chrono::high_resolution_clock::now())
        {
            futures[it.first] = cacheEntry.second;
        }

        //otherwise create a new future and place it into the cache
        else
        {
            const auto launchType = specifiedDriver?std::launch::deferred:std::launch::async;
            futures[it.first] = std::async(launchType, it.second, args);
            cacheEntry = std::make_pair(std::chrono::high_resolution_clock::now() + CACHE_TIMEOUT, futures[it.first]);
        }
    }

    //collect the asynchronous results
    SoapySDR::KwargsList results;
    for (auto &it : futures)
    {
        try
        {
            for (auto handle : it.second.get())
            {
                handle["driver"] = it.first;
                results.push_back(handle);
            }
        }
        catch (const std::exception &ex)
        {
            SoapySDR::logf(SOAPY_SDR_ERROR, "SoapySDR::Device::enumerate(%s) %s", it.first.c_str(), ex.what());
        }
        catch (...)
        {
            SoapySDR::logf(SOAPY_SDR_ERROR, "SoapySDR::Device::enumerate(%s) unknown error", it.first.c_str());
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
