// Copyright (c) 2014-2014 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Device.hpp>
#include <SoapySDR/Registry.hpp>
#include <SoapySDR/Modules.hpp>
#include <stdexcept>
#include <iostream>
#include <cctype>

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

std::vector<SoapySDR::Kwargs> SoapySDR::Device::enumerate(const Kwargs &args)
{
    loadModules();
    std::vector<SoapySDR::Kwargs> results;
    const FindFunctions findFunctions = Registry::listFindFunctions();
    for (FindFunctions::const_iterator it = findFunctions.begin(); it != findFunctions.end(); ++it)
    {
        if (args.count("driver") != 0 and args.at("driver") != it->first) continue;
        try
        {
            std::vector<SoapySDR::Kwargs> results0 = it->second(args);
            for (size_t i = 0; i < results0.size(); i++)
            {
                results0[i]["driver"] = it->first;
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

static std::string trim(const std::string &s)
{
    std::string out = s;
    while (not out.empty() and std::isspace(out[0])) out = out.substr(1);
    while (not out.empty() and std::isspace(out[out.size()-1])) out = out.substr(0, out.size()-1);
    return out;
}

static SoapySDR::Kwargs argsStrToKwargs(const std::string &args)
{
    SoapySDR::Kwargs kwargs;

    bool inKey = true;
    std::string key, val;
    for (size_t i = 0; i < args.size(); i++)
    {
        const char ch = args[i];
        if (inKey)
        {
            if (ch == '=') inKey = false;
            else if (ch == ',') inKey = true;
            else key += ch;
        }
        else
        {
            if (ch == ',') inKey = true;
            else val += ch;
        }
        if ((inKey and not val.empty()) or ((i+1) == args.size()))
        {
            key = trim(key);
            val = trim(val);
            if (not key.empty()) kwargs[key] = val;
            key = "";
            val = "";
        }
    }

    return kwargs;
}

std::vector<SoapySDR::Kwargs> SoapySDR::Device::enumerate(const std::string &args)
{
    return enumerate(argsStrToKwargs(args));
}

SoapySDR::Device* SoapySDR::Device::make(const Kwargs &args_)
{
    loadModules();
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
        if (args.count("driver") == 0)
        {
            std::vector<SoapySDR::Kwargs> results = Device::enumerate(args);
            if (not results.empty()) args = results.front();
        }

        //loop through make functions and call on module match
        MakeFunctions makeFunctions = Registry::listMakeFunctions();
        for (MakeFunctions::const_iterator it = makeFunctions.begin(); it != makeFunctions.end(); ++it)
        {
            if (args.count("driver") != 0 and args.at("driver") != it->first) continue;
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
        for (DeviceTable::iterator it = getDeviceTable().begin(); it != getDeviceTable().end(); ++it)
        {
            if (it->second == device)
            {
                getDeviceTable().erase(it);
                return;
            }
        }
    }
}
