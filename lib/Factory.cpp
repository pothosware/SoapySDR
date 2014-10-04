// Copyright (c) 2014-2014 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Interface.hpp>
#ifdef SOAPY_SDR_BOOST
#include <boost/thread/mutex.hpp>
#else
#include <mutex>
#endif

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

static void loadModules(void)
{
    
}

#endif

SoapySDR::SDRDevice::Sptr SoapySDR::SDRDevice::make(const std::string &args)
{
    //load the modules on the first call to make
    {
        #ifdef SOAPY_SDR_BOOST
        static boost::mutex mutex;
        boost::mutex::scoped_lock lock(mutex);
        #else
        static std::mutex mutex;
        std::lock_guard<std::mutex> lock(mutex);
        #endif

        static bool loaded = false;
        if (not loaded)
        {
            loadModules();
            loaded = true;
        }
    }

    
}
