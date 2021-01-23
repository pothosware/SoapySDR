// Copyright (c) 2014-2020 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include "ErrorHelpers.hpp"
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
    __SOAPY_SDR_C_TRY
    return toStrArray(SoapySDR::listSearchPaths(), length);
    __SOAPY_SDR_C_CATCH_RET(nullptr);
}

char **SoapySDR_listModules(size_t *length)
{
    __SOAPY_SDR_C_TRY
    return toStrArray(SoapySDR::listModules(), length);
    __SOAPY_SDR_C_CATCH_RET(nullptr);
}

char **SoapySDR_listModulesPath(const char *path, size_t *length)
{
    __SOAPY_SDR_C_TRY
    return toStrArray(SoapySDR::listModules(path), length);
    __SOAPY_SDR_C_CATCH_RET(nullptr);
}

char *SoapySDR_loadModule(const char *path)
{
    __SOAPY_SDR_C_TRY
    return toCString(SoapySDR::loadModule(path));
    __SOAPY_SDR_C_CATCH_RET(nullptr);
}

SoapySDRKwargs SoapySDR_getLoaderResult(const char *path)
{
    __SOAPY_SDR_C_TRY
    return toKwargs(SoapySDR::getLoaderResult(path));
    __SOAPY_SDR_C_CATCH_RET(toKwargs(SoapySDR::Kwargs()));
}

char *SoapySDR_getModuleVersion(const char *path)
{
    __SOAPY_SDR_C_TRY
    return toCString(SoapySDR::getModuleVersion(path));
    __SOAPY_SDR_C_CATCH_RET(nullptr);
}

char *SoapySDR_unloadModule(const char *path)
{
    __SOAPY_SDR_C_TRY
    return toCString(SoapySDR::unloadModule(path));
    __SOAPY_SDR_C_CATCH_RET(nullptr);
}

void SoapySDR_loadModules(void)
{
    SoapySDR::loadModules();
}

void SoapySDR_unloadModules(void)
{
    SoapySDR::unloadModules();
}

}
