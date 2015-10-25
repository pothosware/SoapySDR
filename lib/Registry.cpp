// Copyright (c) 2014-2015 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Registry.hpp>
#include <SoapySDR/Logger.hpp>
#include <iostream>

std::string &getModuleLoading(void);

struct FunctionsEntry
{
    std::string modulePath;
    SoapySDR::FindFunction find;
    SoapySDR::MakeFunction make;
};

typedef std::map<std::string, FunctionsEntry> FunctionTable;

static FunctionTable &getFunctionTable(void)
{
    static FunctionTable table;
    return table;
}

SoapySDR::Registry::Registry(const std::string &name, const FindFunction &find, const MakeFunction &make, const std::string &abi)
{
    if (abi != SOAPY_SDR_ABI_VERSION)
    {
        SoapySDR::logf(SOAPY_SDR_ERROR,
            "SoapySDR::Registry(%s) failed ABI check\n"
            "  Library ABI Version: %s\n"
            "  Module ABI Version: %s\n"
            "  Rebuild module against installed library...",
            name.c_str(), SOAPY_SDR_ABI_VERSION, abi.c_str());
        return;
    }

    if (getFunctionTable().count(name) != 0)
    {
        SoapySDR::logf(SOAPY_SDR_ERROR,
            "SoapySDR::Registry(%s) already registered\n"
            "  From: %s\n"
            "  Loading: %s",
            name.c_str(), getFunctionTable()[name].modulePath.c_str(), getModuleLoading().c_str());
        return;
    }

    FunctionsEntry entry;
    entry.modulePath = getModuleLoading();
    entry.find = find;
    entry.make = make;
    getFunctionTable()[name] = entry;
    _name = name;
}

SoapySDR::Registry::~Registry(void)
{
    if (_name.empty()) return;
    getFunctionTable().erase(_name);
}

SoapySDR::FindFunctions SoapySDR::Registry::listFindFunctions(void)
{
    FindFunctions functions;
    for (FunctionTable::const_iterator it = getFunctionTable().begin(); it != getFunctionTable().end(); ++it)
    {
        functions[it->first] = it->second.find;
    }
    return functions;
}

SoapySDR::MakeFunctions SoapySDR::Registry::listMakeFunctions(void)
{
    MakeFunctions functions;
    for (FunctionTable::const_iterator it = getFunctionTable().begin(); it != getFunctionTable().end(); ++it)
    {
        functions[it->first] = it->second.make;
    }
    return functions;
}
