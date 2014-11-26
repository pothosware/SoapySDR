///
/// \file SoapySDR/Logger.h
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
#include <SoapySDR/Config.h>
#include <stdarg.h>

typedef enum
{
    SOAPY_SDR_FATAL    = 1, //!< A fatal error. The application will most likely terminate. This is the highest priority.
    SOAPY_SDR_CRITICAL = 2, //!< A critical error. The application might not be able to continue running successfully.
    SOAPY_SDR_ERROR    = 3, //!< An error. An operation did not complete successfully, but the application as a whole is not affected.
    SOAPY_SDR_WARNING  = 4, //!< A warning. An operation completed with an unexpected result.
    SOAPY_SDR_NOTICE   = 5, //!< A notice, which is an information with just a higher priority.
    SOAPY_SDR_INFO     = 6, //!< An informational message, usually denoting the successful completion of an operation.
    SOAPY_SDR_DEBUG    = 7, //!< A debugging message.
    SOAPY_SDR_TRACE    = 8, //!< A tracing message. This is the lowest priority.
} SoapySDRLogLevel;

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * Send a message to the registered logger.
 * \param logLevel a possible logging level
 * \param message a logger message string
 */
SOAPY_SDR_API void SoapySDR_log(const SoapySDRLogLevel logLevel, const char *message);

/*!
 * Send a message to the registered logger.
 * \param logLevel a possible logging level
 * \param format a printf style format string
 * \param argList an argument list for the formatter
 */
SOAPY_SDR_API void SoapySDR_vlogf(const SoapySDRLogLevel logLevel, const char *format, va_list argList);

/*!
 * Send a message to the registered logger.
 * \param logLevel a possible logging level
 * \param format a printf style format string
 */
static inline void SoapySDR_logf(const SoapySDRLogLevel logLevel, const char *format, ...)
{
    va_list argList;
    va_start(argList, format);
    SoapySDR_vlogf(logLevel, format, argList);
    va_end(argList);
}

/*!
 * Typedef for the registered log handler function.
 */
typedef void (*SoapySDRLogHandler)(const SoapySDRLogLevel logLevel, const char *message);

/*!
 * Register a new system log handler.
 * Platforms should call this to replace the default stdio handler.
 */
SOAPY_SDR_API void SoapySDR_registerLogHandler(const SoapySDRLogHandler handler);

#ifdef __cplusplus
}
#endif
