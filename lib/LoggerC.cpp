// Copyright (c) 2014-2014 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Logger.h>
#include <cstdio>

void defaultLogHandler(const SoapySDRLogLevel logLevel, const char *message)
{
    switch (logLevel)
    {
    case SOAPY_SDR_FATAL:    fprintf(stderr, "[FATAL] %s\n", message); break;
    case SOAPY_SDR_CRITICAL: fprintf(stderr, "[CRITICAL] %s\n", message); break;
    case SOAPY_SDR_ERROR:    fprintf(stderr, "[ERROR] %s\n", message); break;
    case SOAPY_SDR_WARNING:  fprintf(stderr, "[WARNING] %s\n", message); break;
    case SOAPY_SDR_NOTICE:   fprintf(stdout, "[NOTICE] %s\n", message); break;
    case SOAPY_SDR_INFO:     fprintf(stdout, "[INFO] %s\n", message); break;
    case SOAPY_SDR_DEBUG:    fprintf(stdout, "[DEBUG] %s\n", message); break;
    case SOAPY_SDR_TRACE:    fprintf(stdout, "[TRACE] %s\n", message); break;
    }
}

static SoapySDRLogHandler registeredLogHandler = &defaultLogHandler;

extern "C" {

void SoapySDR_log(const SoapySDRLogLevel logLevel, const char *message)
{
    return registeredLogHandler(logLevel, message);
}

void SoapySDR_registerLogHandler(const SoapySDRLogHandler handler)
{
    registeredLogHandler = handler;
}

}
