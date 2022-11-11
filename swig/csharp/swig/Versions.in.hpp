// Copyright (c) 2020-2021 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <SoapySDR/Version.hpp>
#include <sstream>

#include <string>

static std::string __getCompileTimeAPIVersion(void)
{
    std::stringstream ss;
    ss << std::hex << int((SOAPY_SDR_API_VERSION >> 24) & 0xff) << "."
       << std::hex << int((SOAPY_SDR_API_VERSION >> 16) & 0xff) << "."
       << std::hex << int((SOAPY_SDR_API_VERSION >> 0) & 0xffff)
       << std::dec;
    return ss.str();
}

struct BuildInfo
{
    struct SWIGModule
    {
        static const std::string ABIVersion;
        static const std::string APIVersion;
        static const std::string LibVersion;
    };

    struct Runtime
    {
        static const std::string ABIVersion;
        static const std::string APIVersion;
        static const std::string LibVersion;
    };
};

const std::string BuildInfo::SWIGModule::ABIVersion = "@SOAPY_SDR_ABI_VERSION@";
const std::string BuildInfo::SWIGModule::APIVersion = __getCompileTimeAPIVersion();
const std::string BuildInfo::SWIGModule::LibVersion = "@SOAPY_SDR_VERSION@";

const std::string BuildInfo::Runtime::ABIVersion = SoapySDR::getABIVersion();
const std::string BuildInfo::Runtime::APIVersion = SoapySDR::getAPIVersion();
const std::string BuildInfo::Runtime::LibVersion = SoapySDR::getLibVersion();