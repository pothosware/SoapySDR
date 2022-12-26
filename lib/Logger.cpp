// Copyright (c) 2014-2015 Josh Blum
//                    2022 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Logger.hpp>

void SoapySDR::log(const LogLevel logLevel, const std::string &message)
{
    return SoapySDR_log(logLevel, message.c_str());
}

void SoapySDR::vlogf(const SoapySDRLogLevel logLevel, const char *format, va_list argList)
{
    return SoapySDR_vlogf(logLevel, format, argList);
}

void SoapySDR::registerLogHandler(const LogHandler &handler)
{
    return SoapySDR_registerLogHandler(handler);
}

void SoapySDR::setLogLevel(const LogLevel logLevel)
{
    SoapySDR_setLogLevel(logLevel);
}

SoapySDR::LogLevel SoapySDR::getLogLevel(void)
{
    return SoapySDR_getLogLevel();
}
