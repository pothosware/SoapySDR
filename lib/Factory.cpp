// Copyright (c) 2014-2014 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Device.hpp>

static std::string getModulesPath(void)
{
    return "@ROOT@/lib@LIB_SUFFIX@/SoapySDR/modules"; //TODO
}

//static std::map<std::string, factoryTable;

/***********************************************************************
 * Cross platform implementation to load the modules
 **********************************************************************/
#ifdef _MSC_VER
#include <windows.h>

static void loadModules(void)
{
    
}

#else
#include <dlfcn.h>
#include <glob.h>

static void loadModules(void)
{
    
}

#endif

SoapySDR::Device* SoapySDR::Device::make(const Kwargs &args)
{
    //load the modules on the first call to make
    {
        static bool loaded = false;
        if (not loaded)
        {
            loadModules();
            loaded = true;
        }
    }

    
}
