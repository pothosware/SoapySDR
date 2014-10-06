// Copyright (c) 2014-2014 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Registry.hpp>
#include <iostream>

struct FunctionsEntry
{
    SoapySDR::Registry::FindFunction find;
    SoapySDR::Registry::MakeFunction make;
};

static std::map<std::string, FunctionsEntry> &getFunctionTable(void)
{
    static std::map<std::string, FunctionsEntry> table;
    return table;
}

SoapySDR::Registry::Registry(const std::string &name, const FindFunction &find, const MakeFunction &make, const std::string &abi)
{
    if (abi != SOAPY_SDR_ABI_VERSION)
    {
        std::cerr << "SoapySDR::Registry(" << name << ") failed ABI check" << std::endl;
        std::cerr << "  Library ABI Version: " << SOAPY_SDR_ABI_VERSION << std::endl;
        std::cerr << "  Module ABI Version: " << abi << std::endl;
        std::cerr << "  Rebuild module against installed library..." << std::endl;
        return;
    }
    FunctionsEntry entry;
    entry.find = find;
    entry.make = make;
    getFunctionTable()[name] = entry;
}

std::map<std::string, SoapySDR::Registry::FindFunction> SoapySDR::Registry::listFindFunctions(void)
{
    std::map<std::string, FindFunction> functions;
    for (std::map<std::string, FunctionsEntry>::iterator it = getFunctionTable().begin(); it != getFunctionTable().end(); ++it)
    {
        functions[it->first] = it->second.find;
    }
    return functions;
}

std::map<std::string, SoapySDR::Registry::MakeFunction> SoapySDR::Registry::listMakeFunctions(void)
{
    std::map<std::string, MakeFunction> functions;
    for (std::map<std::string, FunctionsEntry>::iterator it = getFunctionTable().begin(); it != getFunctionTable().end(); ++it)
    {
        functions[it->first] = it->second.make;
    }
    return functions;
}
