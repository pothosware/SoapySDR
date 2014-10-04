// Copyright (c) 2014-2014 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Registry.hpp>
#include <map>

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

SoapySDR::Registry::Registry(const std::string &name, const FindFunction &find, const MakeFunction &make)
{
    FunctionsEntry entry;
    entry.find = find;
    entry.make = make;
    getFunctionTable()[name] = entry;
}
