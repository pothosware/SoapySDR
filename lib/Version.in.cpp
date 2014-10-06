// Copyright (c) 2014-2014 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Version.hpp>

std::string SoapySDR::getAPIVersion(void)
{
    return "@SOAPY_SDR_VERSION@";
}

std::string SoapySDR::getABIVersion(void)
{
    return SOAPY_SDR_ABI_VERSION;
}
