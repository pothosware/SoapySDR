// Copyright (c) 2014-2021 Josh Blum
//                    2022 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Logger.h>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <iostream>

/***********************************************************************
 * default log level supports environment variable
 **********************************************************************/
std::string getEnvImpl(const char *name);

static SoapySDRLogLevel getDefaultLogLevel(void)
{
    const std::string logLevelEnvStr = getEnvImpl("SOAPY_SDR_LOG_LEVEL");
    if (logLevelEnvStr.empty()) return SOAPY_SDR_INFO;

    //check string names
    #define checkLogLevelEnvStr(level) \
        if (logLevelEnvStr == #level) return SOAPY_SDR_ ## level
    checkLogLevelEnvStr(FATAL);
    checkLogLevelEnvStr(CRITICAL);
    checkLogLevelEnvStr(ERROR);
    checkLogLevelEnvStr(WARNING);
    checkLogLevelEnvStr(NOTICE);
    checkLogLevelEnvStr(INFO);
    checkLogLevelEnvStr(DEBUG);
    checkLogLevelEnvStr(TRACE);

    //check int values
    const int logLevelInt = std::atoi(logLevelEnvStr.c_str());
    if (logLevelInt < SOAPY_SDR_FATAL) return SOAPY_SDR_FATAL;
    if (logLevelInt > SOAPY_SDR_TRACE) return SOAPY_SDR_TRACE;
    return SoapySDRLogLevel(logLevelInt);
}

/***********************************************************************
 * ANSI terminal colors for default logger
 **********************************************************************/
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define ANSI_COLOR_BOLD    "\x1b[1m"

/***********************************************************************
 * Default log message handler implementation
 **********************************************************************/
static void defaultLogHandler(const SoapySDRLogLevel logLevel, const char *message)
{
    switch (logLevel)
    {
    case SOAPY_SDR_FATAL:    fprintf(stderr, ANSI_COLOR_BOLD ANSI_COLOR_RED "[FATAL] %s" ANSI_COLOR_RESET "\n", message); break;
    case SOAPY_SDR_CRITICAL: fprintf(stderr, ANSI_COLOR_BOLD ANSI_COLOR_RED "[CRITICAL] %s" ANSI_COLOR_RESET "\n", message); break;
    case SOAPY_SDR_ERROR:    fprintf(stderr, ANSI_COLOR_BOLD ANSI_COLOR_RED "[ERROR] %s" ANSI_COLOR_RESET "\n", message); break;
    case SOAPY_SDR_WARNING:  fprintf(stderr, ANSI_COLOR_BOLD ANSI_COLOR_YELLOW "[WARNING] %s" ANSI_COLOR_RESET "\n", message); break;
    case SOAPY_SDR_NOTICE:   fprintf(stderr, ANSI_COLOR_GREEN "[NOTICE] %s" ANSI_COLOR_RESET "\n", message); break;
    case SOAPY_SDR_INFO:     fprintf(stderr, "[INFO] %s\n", message); break;
    case SOAPY_SDR_DEBUG:    fprintf(stderr, "[DEBUG] %s\n", message); break;
    case SOAPY_SDR_TRACE:    fprintf(stderr, "[TRACE] %s\n", message); break;
    case SOAPY_SDR_SSI:      fputs(message, stderr); fflush(stderr); break;
    }
}

static SoapySDRLogHandler registeredLogHandler = &defaultLogHandler;
static SoapySDRLogLevel registeredLogLevel = getDefaultLogLevel();

extern "C" {

void SoapySDR_log(const SoapySDRLogLevel logLevel, const char *message)
{
    if (logLevel > registeredLogLevel and logLevel != SOAPY_SDR_SSI) return;
    return registeredLogHandler(logLevel, message);
}

void SoapySDR_vlogf(const SoapySDRLogLevel logLevel, const char *format, va_list argList)
{
    if (logLevel > registeredLogLevel) return;
    char message[8*1024];
    if (std::vsnprintf(message, sizeof(message), format, argList) > 0)
    {
        SoapySDR_log(logLevel, message);
    }
}

/***********************************************************************
 * Replace the current registeredLogHandler with handler.
 * If nullptr is passed then the default log handler is restored.
 **********************************************************************/
void SoapySDR_registerLogHandler(const SoapySDRLogHandler handler)
{
    if (handler) {
        registeredLogHandler = handler;
    } else {
        registeredLogHandler = defaultLogHandler;
    }
}

void SoapySDR_setLogLevel(const SoapySDRLogLevel logLevel)
{
    registeredLogLevel = logLevel;
}

SoapySDRLogLevel SoapySDR_getLogLevel(void)
{
    return registeredLogLevel;
}

}
