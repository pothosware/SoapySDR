// Copyright (c) 2014-2016 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include "TypeHelpers.hpp"
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

char **SoapySDR_listSearchPaths(size_t *length)
{
    return toStrArray(SoapySDR::listSearchPaths(), length);
}

char **SoapySDR_listModules(size_t *length)
{
    return toStrArray(SoapySDR::listModules(), length);
}

char **SoapySDR_listModulesPath(const char *path, size_t *length)
{
    return toStrArray(SoapySDR::listModules(path), length);
}

char *SoapySDR_loadModule(const char *path)
{
    return strdup(SoapySDR::loadModule(path).c_str());
}

SoapySDRKwargs SoapySDR_getLoaderResult(const char *path)
{
    return toKwargs(SoapySDR::getLoaderResult(path));
}

char *SoapySDR_unloadModule(const char *path)
{
    return strdup(SoapySDR::unloadModule(path).c_str());
}

void SoapySDR_loadModules(void)
{
    SoapySDR::loadModules();
}

}
