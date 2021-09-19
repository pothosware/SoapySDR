// Copyright (c) 2014-2018 Josh Blum
//                    2021 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Device.hpp>
#include <SoapySDR/Registry.hpp>
#include <SoapySDR/Modules.hpp>
#include <SoapySDR/Logger.hpp>
#include <algorithm>
#include <stdexcept>
#include <exception>
#include <future>
#include <iterator>
#include <chrono>
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

void automaticLoadModules(void);

SoapySDR::KwargsList SoapySDR::Device::enumerate(const Kwargs &args)
{
    automaticLoadModules(); //perform one-shot load

    //enumerate cache data structure
    //(driver key, find args) -> (timestamp, handles list)
    //Since available devices should not change rapidly,
    //the cache allows the enumerate results to persist for some time
    //across multiple concurrent callers or subsequent sequential calls.
    static std::recursive_mutex cacheMutex;
    static std::map<std::pair<std::string, Kwargs>,
        std::pair<std::chrono::high_resolution_clock::time_point, std::shared_future<KwargsList>>
    > cache;

    //clean expired entries from the cache
    {
        static const auto CACHE_TIMEOUT = std::chrono::seconds(1);
        std::lock_guard<std::recursive_mutex> lock(cacheMutex);
        const auto now = std::chrono::high_resolution_clock::now();
        for (auto it = cache.begin(); it != cache.end();)
        {
            if (it->second.first+CACHE_TIMEOUT < now) cache.erase(it++);
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
            cacheEntry = std::make_pair(std::chrono::high_resolution_clock::now(), futures[it.first]);
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
    const auto it = getDeviceTable().find(args);
    if (it == getDeviceTable().end()) return nullptr;
    const auto device = it->second;
    if (device == nullptr) throw std::runtime_error("SoapySDR::Device::make() device deletion in-progress");
    getDeviceCounts()[device]++;
    return device;
}

SoapySDR::Device* SoapySDR::Device::make(const Kwargs &inputArgs)
{
    std::unique_lock<std::recursive_mutex> lock(getFactoryMutex());

    //the arguments may have already come from enumerate and been used to open a device
    auto device = getDeviceFromTable(inputArgs);
    if (device != nullptr) return device;

    //otherwise the args must always come from an enumeration result
    //unlock the mutex to block on the enumeration call
    Kwargs discoveredArgs;
    lock.unlock();
    const auto results = Device::enumerate(inputArgs);
    if (not results.empty()) discoveredArgs = results.front();
    lock.lock();

    //check the device table for an already allocated device
    device = getDeviceFromTable(discoveredArgs);
    if (device != nullptr) return device;

    //load the enumeration args with missing keys from the make argument
    Kwargs hybridArgs = discoveredArgs;
    for (const auto &it : inputArgs)
    {
        if (hybridArgs.count(it.first) == 0) hybridArgs[it.first] = it.second;
    }

    //dont continue when driver is unspecified,
    //unless there is only one available driver option
    const bool specifiedDriver = hybridArgs.count("driver") != 0;
    const auto makeFunctions = Registry::listMakeFunctions();
    if (not specifiedDriver and makeFunctions.size() > 2) //more than factory: null + one loaded driver
    {
        throw std::runtime_error("SoapySDR::Device::make() no driver specified and no enumeration results");
    }

    //search for a cache entry or launch a future if not found
    std::map<Kwargs, std::shared_future<Device *>> cache;
    std::shared_future<Device *> deviceFuture;
    for (const auto &it : makeFunctions)
    {
        if (not specifiedDriver and it.first == "null") continue; //skip null unless explicitly specified
        if (specifiedDriver and hybridArgs.at("driver") != it.first) continue; //filter for driver match
        auto &cacheEntry = cache[discoveredArgs];
        if (not cacheEntry.valid()) cacheEntry = std::async(std::launch::deferred, it.second, hybridArgs);
        deviceFuture = cacheEntry;
        break;
    }

    //no match found for the arguments in the loop above
    if (not deviceFuture.valid()) throw std::runtime_error("SoapySDR::Device::make() no match");

    //unlock the mutex to block on the factory call
    lock.unlock();
    deviceFuture.wait();
    lock.lock();

    //the future is complete, erase the cache entry
    //other callers have a copy of the shared future copy or a device table entry
    cache.erase(discoveredArgs);

    //store into the table
    device = deviceFuture.get(); //may throw
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
    if (device == nullptr) return; //safe to unmake a null device

    std::unique_lock<std::recursive_mutex> lock(getFactoryMutex());

    auto countIt = getDeviceCounts().find(device);
    if (countIt == getDeviceCounts().end())
    {
        throw std::runtime_error("SoapySDR::Device::unmake() unknown device");
    }

    if ((--countIt->second) != 0) return;

    //cleanup case for last instance of open device
    getDeviceCounts().erase(countIt);

    //nullify matching entries in the device table
    //make throws if it matches handles which are being deleted
    KwargsList argsList;
    for (auto &it : getDeviceTable())
    {
        if (it.second != device) continue;
        argsList.push_back(it.first);
        it.second = nullptr;
    }

    //do not block other callers while we wait on destructor
    lock.unlock();
    delete device;
    lock.lock();

    //now clean the device table to signal that deletion is complete
    for (const auto &args : argsList) getDeviceTable().erase(args);
}

/*******************************************************************
 * Parallel support
 ******************************************************************/
std::vector<SoapySDR::Device *> SoapySDR::Device::make(const KwargsList &argsList)
{
    std::vector<std::future<Device *>> futures;
    for (const auto &args : argsList)
    {
        futures.push_back(std::async(std::launch::async, [args]{return SoapySDR::Device::make(args);}));
    }

    std::vector<Device *> devices;
    try
    {
        for (auto &future : futures) devices.push_back(future.get());
    }
    catch(...)
    {
        //cleanup all devices made so far, and squelch their errors
        try{SoapySDR::Device::unmake(devices);}
        catch(...){}

        //and then rethrow the exception after cleanup
        throw;
    }
    return devices;
}

std::vector<SoapySDR::Device *> SoapySDR::Device::make(const std::vector<std::string> &argsList)
{
    SoapySDR::KwargsList kwargsList;
    std::transform(
        argsList.begin(),
        argsList.end(),
        std::back_inserter(kwargsList),
        SoapySDR::KwargsFromString);

    return make(kwargsList);
}

void SoapySDR::Device::unmake(const std::vector<Device *> &devices)
{
    std::vector<std::future<void>> futures;
    for (const auto &device : devices)
    {
        futures.push_back(std::async(std::launch::async, [device]{SoapySDR::Device::unmake(device);}));
    }

    //unmake will only throw the last exception
    //Since unmake only throws for unknown handles, this is probably API misuse.
    //The actual particular exception and its associated device is not important.
    std::exception_ptr eptr;
    for (auto &future : futures)
    {
        try {future.get();}
        catch(...){eptr = std::current_exception();}
    }
    if (eptr) std::rethrow_exception(eptr);
}
