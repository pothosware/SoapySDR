///
/// \file SoapySDR/Logger.hpp
///
/// Logger API for SoapySDR devices.
/// Implementations should use the logger rather than stdio.
/// The default log handler prints to stderr.
///
/// \copyright
/// Copyright (c) 2014-2015 Josh Blum
///                    2022 Nicholas Corgan
/// SPDX-License-Identifier: BSL-1.0
///

#pragma once
#include <SoapySDR/Logger.h>
#include <string>
#include <cstdarg>

namespace SoapySDR
{

typedef SoapySDRLogLevel LogLevel;

/*!
 * Send a message to the registered logger.
 * \param logLevel a possible logging level
 * \param message a logger message string
 */
SOAPY_SDR_API void log(const LogLevel logLevel, const std::string &message);

/*!
 * Send a message to the registered logger.
 * \param logLevel a possible logging level
 * \param format a printf style format string
 * \param argList an argument list for the formatter
 */
SOAPY_SDR_API void vlogf(const SoapySDRLogLevel logLevel, const char *format, va_list argList);

/*!
 * Send a message to the registered logger.
 * \param logLevel a possible logging level
 * \param format a printf style format string
 */
static inline void logf(const SoapySDRLogLevel logLevel, const char *format, ...)
{
    va_list argList;
    va_start(argList, format);
    SoapySDR::vlogf(logLevel, format, argList);
    va_end(argList);
}

/*!
 * Typedef for the registered log handler function.
 */
typedef SoapySDRLogHandler LogHandler;

/*!
 * Register a new system log handler.
 * Platforms should call this to replace the default stdio handler.
 * Passing `nullptr` restores the default.
 */
SOAPY_SDR_API void registerLogHandler(const LogHandler &handler);

/*!
 * Set the log level threshold.
 * Log messages with lower priority are dropped.
 */
SOAPY_SDR_API void setLogLevel(const LogLevel logLevel);

/*!
 * Get the log level threshold.
 */
SOAPY_SDR_API LogLevel getLogLevel(void);

}
