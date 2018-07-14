// Copyright (c) 2014-2018 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Registry.hpp>
#include <mutex>

/***********************************************************************
 * Function table holds all registration entries by name
 **********************************************************************/
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

static std::recursive_mutex &getRegistryMutex(void)
{
    static std::recursive_mutex mutex;
    return mutex;
}

/***********************************************************************
 * Module loader shared data structures
 **********************************************************************/
std::string &getModuleLoading(void);

std::map<std::string, SoapySDR::Kwargs> &getLoaderResults(void);

/***********************************************************************
 * Registry entry-point implementation
 **********************************************************************/
SoapySDR::Registry::Registry(const std::string &name, const FindFunction &find, const MakeFunction &make, const std::string &abi)
{
    std::lock_guard<std::recursive_mutex> lock(getRegistryMutex());

    //create an entry for the loader result
    std::string &errorMsg = getLoaderResults()[getModuleLoading()][name];

    //abi check
    if (abi != SOAPY_SDR_ABI_VERSION)
    {
        errorMsg = name + " failed ABI check: Library ABI=" SOAPY_SDR_ABI_VERSION ", Module ABI="+abi;
        return;
    }

    //duplicate check
    if (getFunctionTable().count(name) != 0)
    {
        errorMsg = "duplicate entry for " + name + " ("+getFunctionTable()[name].modulePath + ")";
        return;
    }

    //register functions
    FunctionsEntry entry;
    entry.modulePath = getModuleLoading();
    entry.find = find;
    entry.make = make;
    getFunctionTable()[name] = entry;
    _name = name;
}

SoapySDR::Registry::~Registry(void)
{
    //erase entry
    if (_name.empty()) return;
    getFunctionTable().erase(_name);
}

/***********************************************************************
 * Registry access API
 **********************************************************************/
SoapySDR::FindFunctions SoapySDR::Registry::listFindFunctions(void)
{
    std::lock_guard<std::recursive_mutex> lock(getRegistryMutex());

    FindFunctions functions;
    for (const auto &it : getFunctionTable())
    {
        functions[it.first] = it.second.find;
    }
    return functions;
}

SoapySDR::MakeFunctions SoapySDR::Registry::listMakeFunctions(void)
{
    std::lock_guard<std::recursive_mutex> lock(getRegistryMutex());

    MakeFunctions functions;
    for (const auto &it : getFunctionTable())
    {
        functions[it.first] = it.second.make;
    }
    return functions;
}
