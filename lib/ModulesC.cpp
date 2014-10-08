// Copyright (c) 2014-2014 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Modules.hpp>

extern "C" {

const char *SoapySDR_getRootPath(void)
{
    static const std::string root = SoapySDR::getRootPath();
    return root.c_str();
}

SoapySDRStringList SoapySDR_listModules(void)
{
    const std::vector<std::string> paths = SoapySDR::listModules();
    SoapySDRStringList pathList = {0, NULL};
    for (size_t i = 0; i < paths.size(); i++)
    {
        SoapySDRStringList_append(&pathList, paths[i].c_str());
    }
    return pathList;
}

void SoapySDR_loadModule(const char *path)
{
    SoapySDR::loadModule(path);
}

void SoapySDR_loadModules(void)
{
    SoapySDR::loadModules();
}

}
