// Copyright (c) 2014-2014 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Modules.h>
#include <SoapySDR/Modules.hpp>
#include <cstdlib>
#include <cstring>

extern "C" {

const char *SoapySDR_getRootPath(void)
{
    static const std::string root = SoapySDR::getRootPath();
    return root.c_str();
}

char **SoapySDR_listModules(size_t *length)
{
    const std::vector<std::string> paths = SoapySDR::listModules();
    char **pathArray = (char **) malloc(sizeof(char *)*paths.size());
    for (size_t i = 0; i < paths.size(); i++)
    {
        pathArray[i] = strdup(paths[i].c_str());
    }
    *length = paths.size();
    return pathArray;
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
