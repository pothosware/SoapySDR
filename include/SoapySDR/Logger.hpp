///
/// \file SoapySDR/Logger.hpp
///
/// Logger API for SoapySDR devices.
/// Implementations should use the logger rather than stdio.
/// The default log handler prints to the stdout and stderr.
///
/// \copyright
/// Copyright (c) 2014-2014 Josh Blum
/// SPDX-License-Identifier: BSL-1.0
///

#pragma once
#include <SoapySDR/Logger.h>
#include <string>

namespace SoapySDR
{

typedef SoapySDRLogLevel LogLevel;

/*!
 * Log a message to the registered logger.
 * \param logLevel a possible logging level
 * \param message a logger message string
 */
SOAPY_SDR_API void log(const LogLevel logLevel, const std::string &message);

/*!
 * Typedef for the registered log handler function.
 */
typedef SoapySDRLogHandler LogHandler;

/*!
 * Register a new system log handler.
 * Platforms should call this to replace the default stdio handler.
 */
SOAPY_SDR_API void registerLogHandler(LogHandler handler);

}
