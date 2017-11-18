// Copyright (c) 2014-2017 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Version.hpp>
#include <sstream>

std::string SoapySDR::getAPIVersion(void)
{
    std::stringstream ss;
    ss << std::hex << int((SOAPY_SDR_API_VERSION >> 24) & 0xff) << "."
       << std::hex << int((SOAPY_SDR_API_VERSION >> 16) & 0xff) << "."
       << std::hex << int((SOAPY_SDR_API_VERSION >> 0) & 0xffff)
       << std::dec;
    return ss.str();
}

std::string SoapySDR::getABIVersion(void)
{
    return SOAPY_SDR_ABI_VERSION;
}

std::string SoapySDR::getLibVersion(void)
{
    return "@SOAPY_SDR_VERSION@";
}
