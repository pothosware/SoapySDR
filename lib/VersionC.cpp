// Copyright (c) 2014-2014 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Version.hpp>

extern "C" {

const char *SoapySDR_getAPIVersion(void)
{
    static const std::string api = SoapySDR::getAPIVersion();
    return api.c_str();
}

const char *SoapySDR_getABIVersion(void)
{
    static const std::string abi = SoapySDR::getABIVersion();
    return abi.c_str();
}

}
