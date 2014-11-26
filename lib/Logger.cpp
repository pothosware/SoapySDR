// Copyright (c) 2014-2014 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Logger.hpp>

void SoapySDR::log(const LogLevel logLevel, const std::string &message)
{
    return SoapySDR_log(logLevel, message.c_str());
}

void SoapySDR::registerLogHandler(const LogHandler &handler)
{
    return SoapySDR_registerLogHandler(handler);
}
